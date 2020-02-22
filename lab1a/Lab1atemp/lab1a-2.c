//NAME: Wenxuan Liu
//EMAIL: allenliux01@163.com
//ID: 805152602

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>

#define ETX '\3'
#define EOT '\4'
#define CR '\r'
#define LF '\n'

char crlf[2] = {CR, LF};
struct termios saved;
int fd2p[2], fd2c[2], pid;
static int shell_flag = 0;
static int debug_flag = 0;

void restore_input_mode (void) {
	tcsetattr (STDIN_FILENO, TCSANOW, &saved);
}

void set_input_mode (void){
	struct termios tattr;

  /* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO)){
		fprintf (stderr, "Error. Not a terminal.\n");
		exit (EXIT_FAILURE);
	}

  /* Save the terminal attributes so we can restore them later. */
	tcgetattr (STDIN_FILENO, &saved);
	atexit (restore_input_mode);

  /* Set the terminal modes. */
	tcgetattr (STDIN_FILENO, &tattr);

	tattr.c_iflag = ISTRIP;	/* only lower 7 bits	*/
	tattr.c_lflag = 0;
	tattr.c_oflag = 0;
	// tattr.c_cc[VMIN] = 1;
	// tattr.c_cc[VTIME] = 0;
	if (tcsetattr (STDIN_FILENO, TCSANOW, &tattr)<0){
		fprintf(stderr, "Error. Setting terminal mode failed.\n");
		exit(1);
	}
}

void create_pipe(int p[2]){
	if (pipe(p) != 0){
		fprintf(stderr, "Error. Creating pipe failed.\n");
		exit(1);
	}
}

void exit_shell() {
	int status;
	if ( waitpid(pid, &status, 0)==-1 ){
		fprintf(stderr, "Error. Waitpid failed.");
		exit(1);
	}
	if(WIFEXITED(status)){
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
		exit(0);
	}
}

void sig_handler() {
	close(fd2c[1]);
	close(fd2p[0]);
	kill(pid, SIGINT);
	exit_shell();
}

void child_shell(){
	close(fd2c[1]); //close write to child
	close(fd2p[0]); //close read to parent

	dup2(fd2c[0], STDIN_FILENO); //read to child -> stdin
	dup2(fd2p[1], STDOUT_FILENO); //write to parent -> stdout

	close(fd2c[0]);
	close(fd2p[1]);

	//execute the shell
	char *arguments[2];
	char filename[] = "/bin/bash";
	arguments[0] = filename;
	arguments[1] = NULL;
	if (execvp(filename, arguments) == -1) {	//executing
		fprintf(stderr, "Error. Exec a shell failed.\n");
		exit(1);
	}
}

void parent_shell(){
	close(fd2c[0]); //close read to child
	close(fd2p[1]); //close write to parent


	struct pollfd poll_fds[] = {
		{STDIN_FILENO, POLLIN, 0},
		{fd2p[0], POLLIN, 0}};

	int EOT_Flag = 0;
	while (!EOT_Flag) {
		if (poll(poll_fds, 2, 0) > 0) {
			short revents_stdin = poll_fds[0].revents;
			short revents_shell = poll_fds[1].revents;

			if (revents_stdin == POLLIN) { //stdin has input
				char buff[256];
				int readbytes = read(STDIN_FILENO, &buff, 256);
				int i;
				for (i = 0; i < readbytes; i++) {
					char cur = buff[i];
					switch(cur){
						case ETX:	//process ^C immediately when inputting
							kill(pid, SIGINT);
							break;
						case EOT:	//process EOF from stdin
							EOT_Flag = 1;
							break;
						case CR:
						case LF:
							write(STDOUT_FILENO, crlf, 2);
							char c = LF;
							write(fd2c[1], &c, 1);
							break;
						default:
							write(STDOUT_FILENO, (buff + i), 1);
							write(fd2c[1], (buff + i), 1);
					}
					// if (buff[i] == ETX)	//process ^C immediately when inputting
					// 	kill(pid, SIGINT); 
				 //  else if (buff[i] == EOT)	//process EOF from stdin
				 //    		EOT_Flag = 1;
				 //  else if (buff[i] == CR || buff[i] == LF) { 
					// 	write(STDOUT_FILENO, crlf, 2);
					// 	char c = LF;
					// 	write(fd2c[1], &c, 1);
					// } 
					// else { 
					// 	write(STDOUT_FILENO, (buff + i), 1);
					// 	write(fd2c[1], (buff + i), 1);
					// }
				}
			}
			else if (revents_stdin == POLLERR) {
				fprintf(stderr, "Error. Poll with STDIN failed.\n");
				exit(1);
			}

			if (revents_shell == POLLIN) {	//the shell has input
				char buff[256];
				int readbytes = read(fd2p[0], &buff, 256); 
				int count = 0;
				int i, j;
				for (i = 0, j = 0; i < readbytes; i++) {
					if (buff[i] == EOT) { //process EOF from shell
						EOT_Flag = 1;
					} 
					else if (buff[i] == LF) {
						write(STDOUT_FILENO, (buff + j), count);
						write(STDOUT_FILENO, crlf, 2);
						j += count + 1;
						count = 0;
						continue;
					}
					count++;
				}
				write(STDOUT_FILENO, (buff+j), count);		

			} else if (revents_shell & POLLERR || revents_shell & POLLHUP) { //polling error
				EOT_Flag = 1;
			} 
		} 
	}

	close(fd2p[0]);
	close(fd2c[1]);
  exit_shell();
}

int main (int argc, char **argv){
	char c;
	// const char *optstring = "sd";
	struct option opts[] = {
		{"shell", 0, NULL, 's'},
		{"debug", 0, NULL, 'd'}};
	while((c=getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 's':
				shell_flag = 1;
				break;
			case 'd':
				debug_flag = 1;
				break;
			case '?':
				fprintf(stderr, "Error. Invald option.\n");
				exit(1);
		}
	}

	set_input_mode ();
	
	if (shell_flag){
		create_pipe(fd2p);
		create_pipe(fd2c);
		set_input_mode();
		signal(SIGPIPE, sig_handler);

		pid = fork();
		if(pid == 0){ //(child) shell process
			child_shell();
		}
		else if (pid > 0) { //(parent) terminal process
			parent_shell();
		}
		else{ //fork failed
			fprintf(stderr, "Error. Fork process failed.\n");
			exit(1);
		}
	}

	char in;

	while (read(STDIN_FILENO, &in, 10) > 0 && in != EOT) {
		//carriage return or line feed -> <cr><lf>
		if (in == CR || in == LF) { 
			in = CR;
			write(STDOUT_FILENO, &in, 1);
			in = LF;
			write(STDOUT_FILENO, &in, 1);
		} 
		else {	//normal input
			write(STDOUT_FILENO, &in, 1);
		}
	}

	// while (1) {
	// 	read (STDIN_FILENO, &c, 1);
	// 	if (c == '\004')          
	// 		break;
	// 	else
	// 		putchar (c);
	// }

	return 0;
}
