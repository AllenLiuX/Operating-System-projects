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

#define CR '\015' //carriage return
#define LF '\012' //line feed
#define EOT '\004' //^D (End of transmission)
#define ETX '\003' //^C (End of text)

struct termios saved;
static int shell_flag = 0;
int     pipe_ctop[2], pipe_ptoc[2], pid;
// pid_t   childpid;
char crlf[2] = {CR, LF};

void restore_input_mode (void) {
	tcsetattr (STDIN_FILENO, TCSANOW, &saved);
}

void set_input_mode (void){
	struct termios tattr;
	// char *name;

  /* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO)){
		fprintf (stderr, "Error. Not a terminal.\n");
		exit (EXIT_FAILURE);
	}

  /* Save the terminal attributes so we can restore them later. */
	tcgetattr (STDIN_FILENO, &saved);
	atexit (restore_input_mode);

  /* Set the funny terminal modes. */
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
	if (pipe(p) != 0){ //create pipe
		fprintf(stderr, "Error. Creating pipe failed.\n");
		exit(1);
	}
}

void shell_exit_status() {
	int status;
	waitpid(pid, &status, 0);
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
}

void handle_sigpipe() {
	close(pipe_ptoc[1]);
	close(pipe_ctop[0]);
	kill(pid, SIGINT);
	shell_exit_status();
	exit(0);
}

int main (int argc, char **argv){
	char c;
	// const char *optstring = "s";
	struct option opts[] = {{"shell", 0, NULL, 's'}};
	while((c=getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 's':
				shell_flag = 1;
				break;
			case '?':
				fprintf(stderr, "invald option.\n");
				exit(1);
		}
	}
	if (shell_flag){
		create_pipe(pipe_ctop);
		create_pipe(pipe_ptoc);
		set_input_mode();
		signal(SIGPIPE, handle_sigpipe);

		pid = fork();
		//child shell process
		if(pid == 0){
			close(pipe_ptoc[1]); //write end from parent to child
			close(pipe_ctop[0]); //read end from child to parent

			dup2(pipe_ptoc[0], STDIN_FILENO); //read from parent to child
			dup2(pipe_ctop[1], STDOUT_FILENO); //write from child to parent
			// dup2(pipe_ctop[1], STDERR_FILENO);

			close(pipe_ptoc[0]);
			close(pipe_ctop[1]);

			//execute the shell
			char *arguments[2];
			char filename[] = "/bin/bash";
			arguments[0] = filename;
			arguments[1] = NULL;
			if (execvp(filename, arguments) == -1) {
				fprintf(stderr, "Failed to exec a shell.\n");
				exit(1);
			}
		}
		else if (pid > 0) { //parent (terminal) process
			close(pipe_ptoc[0]); //read end from parent to child
			close(pipe_ctop[1]); //write end from child to parent
		

			struct pollfd descriptors[] = {
				{STDIN_FILENO, POLLIN, 0}, //keyboard (stdin)
				{pipe_ctop[0], POLLIN, 0} //output from shell
			};

			int EOT_flag = 0;
			int i;

			while (!EOT_flag) {
				if (poll(descriptors, 2, 0) > 0) {

					short revents_stdin = descriptors[0].revents;
					short revents_shell = descriptors[1].revents;

					/* check that stdin has pending input */
					if (revents_stdin == POLLIN) {
						char input[256];
						int num = read(STDIN_FILENO, &input, 256);
						for (i = 0; i < num; i++) {
							if (input[i] == ETX) {
								kill(pid, SIGINT); 

						    	} else if (input[i] == EOT) {
						    		EOT_flag = 1;

						    	} else if (input[i] == CR || input[i] == LF) { 
						    		char lf = LF;
								write(STDOUT_FILENO, crlf, 2);
								write(pipe_ptoc[1], &lf, 1);
							
							} else { 
								write(STDOUT_FILENO, (input + i), 1);
								write(pipe_ptoc[1], (input + i), 1);
							}
						}

					} else if (revents_stdin == POLLERR) {
						fprintf(stderr, "Error with poll with STDIN.\n");
						exit(1);
					}

					/* check that the shell has pending input */
					if (revents_shell == POLLIN) {
						/* "Upon receiving EOF or polling error from
						 * the shell, we know that there will be no
						 * more output coming from the shell."
						 */
						char input[256];
						int num = read(pipe_ctop[0], &input, 256); 
						int count = 0;
						int j;
						for (i = 0, j = 0; i < num; i++) {
							if (input[i] == EOT) { //EOF from shell
								EOT_flag = 1;
							} 
							else if (input[i] == LF) {
								write(STDOUT_FILENO, (input + j), count);
								write(STDOUT_FILENO, crlf, 2);
								j += count + 1;
								count = 0;
								continue;
							}
							count++;
						}
						write(STDOUT_FILENO, (input+j), count);		

					} else if (revents_shell & POLLERR || revents_shell & POLLHUP) { //polling error
						EOT_flag = 1;
					} 
				} 
			}

			close(pipe_ctop[0]);
			close(pipe_ptoc[1]);
		    	shell_exit_status();
			exit(0);
		}
		else{ //fork failed
			fprintf(stderr, "Error. Fork process failed.\n");
			exit(1);
		}
	}

	set_input_mode ();

	char input;

	while (read(STDIN_FILENO, &input, 10) > 0 && input != EOT) {
		//carriage return or line feed should become <cr><lf>
		if (input == CR || input == LF) { 
			input = CR;
			write(STDOUT_FILENO, &input, 1);
			input = LF;
			write(STDOUT_FILENO, &input, 1);
		} else {
			write(STDOUT_FILENO, &input, 1);
		}
	}

	// while (1) {
	// 	read (STDIN_FILENO, &c, 1);
	// 	if (c == '\004')          
	// 		break;
	// 	else
	// 		putchar (c);
	// }

	return EXIT_SUCCESS;
}
