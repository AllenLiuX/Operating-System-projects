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
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zlib.h>

#define ETX '\003'
#define EOT '\004'
#define CR '\r'
#define LF '\n'

char crlf[2] = {CR, LF};
struct termios saved;
int fd2p[2], fd2c[2], pid;

int sockfd, newsockfd;

int compress_flag = 0;
int port_flag = 0;
int portno = 0;

z_stream cli2ser;
z_stream ser2cli;

void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void exit_shell() {
	int status;
	waitpid(pid, &status, 0);
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
}

void sig_handler() {
	close(fd2c[1]);
	close(fd2p[0]);
	kill(pid, SIGINT);
	exit_shell();
	exit(0);
}

void set_compress(){
	cli2ser.zalloc = Z_NULL;
	cli2ser.zfree = Z_NULL;
	cli2ser.opaque = Z_NULL;
	
	ser2cli.zalloc = Z_NULL;
	ser2cli.zfree = Z_NULL;
	ser2cli.opaque = Z_NULL;
}

void create_pipe(int p[2]){
	if (pipe(p) != 0){
		error("Error: creating pipe failed.");
	}
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
		fprintf(stderr, "Error: execute shell command failed.\n");
		exit(1);
	}
}

void parent_shell(){
	close(fd2c[0]); //close read to child
		close(fd2p[1]); //close write to parent

		struct pollfd poll_fds[] = {
			{newsockfd, POLLIN, 0},
			{fd2p[0], POLLIN, 0}
		};

		int EOT_Flag = 0;
		int status, i;
		while (!EOT_Flag) {
			if (waitpid (pid, &status, WNOHANG) != 0) {
				close(sockfd);
				close(newsockfd);
				close(fd2p[0]);
				close(fd2c[1]);
				fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
				exit(0);
			}
			int pollres = poll(poll_fds, 2, 0);
			if (pollres > 0) {
				short stdin_revents = poll_fds[0].revents;
				short shell_revents = poll_fds[1].revents;
				if (stdin_revents == POLLIN) { //socket has input
					char buff[256];
					int readbytes = read(newsockfd, &buff, 256);
					if(compress_flag) {
						char combuff[1024];
						cli2ser.avail_in = readbytes;
						cli2ser.next_in = (unsigned char *) buff;
						cli2ser.avail_out = 1024;
						cli2ser.next_out = (unsigned char *) combuff;
						do {
							inflate(&cli2ser, Z_SYNC_FLUSH);
						} while (cli2ser.avail_in > 0);
						for (i = 0; (unsigned int) i < 1024 - cli2ser.avail_out; i++) {
							if (combuff[i] == ETX) {
								kill(pid, SIGINT); 
							} else if (combuff[i] == EOT) {
							    EOT_Flag = 1;
							} else if (combuff[i] == CR || combuff[i] == LF) { 
								char lf = LF;
								write(fd2c[1], &lf, 1);
							} else { 
								write(fd2c[1], (combuff + i), 1);
							}
						}
					}
					else{
						char c = LF;
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
									write(fd2c[1], &c, 1);
									break;
								default:
									write(fd2c[1], (buff + i), 1);
							}
						}
					}
				}
				else if (stdin_revents == POLLERR)
					error("Error: poll with socket failed.");

				if (shell_revents == POLLIN) {	//the shell has output
					char buff[256];
					int readbytes = read(fd2p[0], &buff, 256); 
					int chnum = 0;
					int j;
					for (i = 0, j = 0; i < readbytes; i++) {
						if (buff[i] == EOT) //process EOF from shell
							EOT_Flag = 1;
						else if (buff[i] == LF) {
							if(compress_flag && readbytes>0){
								char combuff[256];
								ser2cli.avail_in = chnum;
								ser2cli.avail_out = 256;
								ser2cli.next_in = (unsigned char *) (buff + j);
								ser2cli.next_out = (unsigned char *) combuff;
								do {
									deflate(&ser2cli, Z_SYNC_FLUSH);
								} while (ser2cli.avail_in > 0);
								write(newsockfd, combuff, 256 - ser2cli.avail_out);
								char combuff2[256];
								ser2cli.avail_in = 2;
								ser2cli.avail_out = 256;
								ser2cli.next_in = (unsigned char *) crlf;
								ser2cli.next_out = (unsigned char *) combuff2;
								do {
									deflate(&ser2cli, Z_SYNC_FLUSH);
								} while (ser2cli.avail_in > 0);
								write(newsockfd, combuff2, 256 - ser2cli.avail_out);
							}
							else{
								if(write(newsockfd, (buff+j), chnum)<0)
									error("Error: writing to socket failed.");
								write(newsockfd, crlf, 2);
							}
							j = j+chnum+1;
							chnum = 0;
							continue;
						}
						chnum++;
					}
					// write(STDOUT_FILENO, (buff+j), chnum);	
					write(newsockfd, (buff+j), chnum);
				} 
				else if (shell_revents & POLLERR || shell_revents & POLLHUP) { //detect POLLERR and POLLHUP
					EOT_Flag = 1;
				}
			} 
			else if(pollres < 0)
				error("Error: Poll failed");
		}
		//close the remaining pipes
		close(sockfd);
		close(newsockfd);
		close(fd2p[0]);
		close(fd2c[1]);
	  exit_shell();
	}

int main (int argc, char* argv[]){
	struct option opts[] = {
		{"port", 1, NULL, 'p'},
		{"compress", 0, NULL, 'c'}
	};
	char c;
	while((c = getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 'p':
				port_flag = 1;
				portno = atoi(optarg);
				break;
			case 'c':
				compress_flag = 1;
				set_compress();
				if (inflateInit(&cli2ser) != Z_OK) {
					error("Error: inflateInit on client side failed.");
				}
				if (deflateInit(&ser2cli, Z_DEFAULT_COMPRESSION) != Z_OK) {
					error("Error: deflateInit on client side failed.");
				}
				break;
			case '?':
				error("Error: Invald option.");
				break;
		}
	}

	if(!port_flag){
		error("Error: --port= is mandatory.");
	}

	// set_input_mode ();

//======connection=====
	unsigned int clilen;
	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
    error("ERROR opening socket");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
 	serv_addr.sin_addr.s_addr = INADDR_ANY;
 	serv_addr.sin_port = htons(portno);
 	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
 		error("ERROR on binding");
 	}
 	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
	
	create_pipe(fd2p);
	create_pipe(fd2c);
	signal(SIGPIPE, sig_handler);

	pid = fork();
	if(pid == 0){ //(child) shell process
		child_shell();
	}
	else if (pid > 0) { //(parent) terminal process
		parent_shell();
	}
	else{ //fork failed
		fprintf(stderr, "Error: Fork process failed.\n");
		exit(1);
	}
	if (compress_flag) {
		inflateEnd(&cli2ser);
		deflateEnd(&ser2cli);
	}
	return 0;
}
