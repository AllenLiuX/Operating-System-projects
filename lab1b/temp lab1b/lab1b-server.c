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

#define ETX '\3'
#define EOT '\4'
#define CR '\r'
#define LF '\n'

char crlf[2] = {CR, LF};
struct termios saved;
int fd2p[2], fd2c[2], pid;
int debug_flag = 0;

int sockfd, newsockfd, clilen;

z_stream client_to_server;
z_stream server_to_client;

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

void create_pipe(int p[2]){
	if (pipe(p) != 0){
		fprintf(stderr, "Error: Creating pipe failed.\n");
		exit(1);
	}
}

// void child_shell(){
// 	close(fd2c[1]); //close write to child
// 	close(fd2p[0]); //close read to parent

// 	dup2(fd2c[0], STDIN_FILENO); //read to child -> stdin
// 	dup2(fd2p[1], STDOUT_FILENO); //write to parent -> stdout

// 	close(fd2c[0]);
// 	close(fd2p[1]);

// 	//execute the shell
// 	char *arguments[2];
// 	char filename[] = "/bin/bash";
// 	arguments[0] = filename;
// 	arguments[1] = NULL;
// 	if (execvp(filename, arguments) == -1) {	//executing
// 		fprintf(stderr, "Error: Exec a shell failed.\n");
// 		exit(1);
// 	}
// }

// void parent_shell(){
// 	close(fd2c[0]); //close read to child
// 	close(fd2p[1]); //close write to parent

// 	struct pollfd poll_fds[] = {
// 		{newsockfd, POLLIN, 0},
// 		{fd2p[0], POLLIN, 0}};

// 	int EOT_Flag = 0;
// 	int status;
// 	while (!EOT_Flag) {
// 		if (waitpid (pid, &status, WNOHANG) != 0) {
// 			close(sockfd);
// 			close(newsockfd);
// 			close(fd2p[0]);
// 			close(fd2c[1]);
// 			fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
// 			exit(0);
// 		}

// 		if (poll(poll_fds, 2, 0) > 0) {
// 			short stdin_revents = poll_fds[0].revents;
// 			short shell_revents = poll_fds[1].revents;
// 			if (stdin_revents == POLLIN) { //socket has input
// 				char buff[256];
// 				// int readbytes = read(STDIN_FILENO, &buff, 256);
// 				int readbytes = read(newsockfd, &buff, 256);
// 				int i;
// 				char c = LF;
// 				for (i = 0; i < readbytes; i++) {
// 					char cur = buff[i];
// 					switch(cur){
// 						case ETX:	//process ^C immediately when inputting
// 							kill(pid, SIGINT);
// 							break;
// 						case EOT:	//process EOF from stdin
// 							EOT_Flag = 1;
// 							break;
// 						case CR:
// 						case LF:
// 							write(fd2c[1], &c, 1);
// 							break;
// 						default:
// 							write(fd2c[1], (buff + i), 1);
// 					}
// 				}
// 			}
// 			else if (stdin_revents & POLLERR)
// 				error("Error: Poll with socket failed.");

// 			if (shell_revents == POLLIN) {	//the shell has input
// 				char buff[256];
// 				int readbytes = read(fd2p[0], &buff, 256); 
// 				int count = 0;
// 				int i, j;
// 				for (i = 0, j = 0; i < readbytes; i++) {
// 					if (buff[i] == EOT) //process EOF from shell
// 						EOT_Flag = 1;
// 					else if (buff[i] == LF) {
// 						// write(STDOUT_FILENO, (buff + j), count);
// 						// write(STDOUT_FILENO, crlf, 2);	//new line
// 						if(write(newsockfd, (buff+j), count)<0)
// 							error("Error: writing to socket failed.");
// 						write(newsockfd, crlf, 2);
// 						j = j+count+1;
// 						count = 0;
// 						continue;
// 					}
// 					count++;
// 				}
// 				// write(STDOUT_FILENO, (buff+j), count);	
// 				write(newsockfd, (buff+j), count);
// 			} 
// 			else if (shell_revents & POLLERR || shell_revents & POLLHUP) { //detect POLLERR and POLLHUP
// 				EOT_Flag = 1;
// 			}
// 		} 
// 	}
// 	//close the remaining pipes
// 	close(sockfd);
// 	close(newsockfd);
// 	close(fd2p[0]);
// 	close(fd2c[1]);
//   exit_shell();
//   exit(0);
// }

int main (int argc, char **argv){
	char c;

	int compress_flag = 0;
	int port_flag = 0;
	int portno;

	struct option opts[] = {
		{"port", 1, NULL, 'p'},
		{"compress", 0, NULL, 'c'}
	};
	while((c=getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 'p':
				port_flag = 1;
				portno = atoi(optarg);
				break;
			case 'c':
				compress_flag = 1;

				server_to_client.zalloc = Z_NULL;
				server_to_client.zfree = Z_NULL;
				server_to_client.opaque = Z_NULL;

				if (deflateInit(&server_to_client, Z_DEFAULT_COMPRESSION) != Z_OK) {
					fprintf(stderr, "Failure to deflateInit on client side.\n");
					exit(1);
				}

				client_to_server.zalloc = Z_NULL;
				client_to_server.zfree = Z_NULL;
				client_to_server.opaque = Z_NULL;

				if (inflateInit(&client_to_server) != Z_OK) {
					fprintf(stderr, "Failure to inflateInit on client side.\n");
					exit(1);
				}

				break;
			case '?':
				error("Error: Invald option.");
		}
	}

	if(!port_flag){
		error("Error: --port= is mandatory.");
	}

	// set_input_mode ();

//======connection=====
	// char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	// int n;

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
 	listen(sockfd,8);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
	
	create_pipe(fd2p);
	create_pipe(fd2c);

	signal(SIGPIPE, sig_handler);

	pid = fork();
	if(pid == 0){ //(child) shell process
		// child_shell();
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
			fprintf(stderr, "Error: Exec a shell failed.\n");
			exit(1);
		}
	}
	else if (pid > 0) { //(parent) terminal process
		// parent_shell();
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

			if (poll(poll_fds, 2, 0) > 0) {
				short stdin_revents = poll_fds[0].revents;
				short shell_revents = poll_fds[1].revents;
				if (stdin_revents == POLLIN) { //socket has input
					char buff[256];
					int readbytes = read(newsockfd, &buff, 256);
					if(compress_flag) {
						char compression_buf[1024];
						client_to_server.avail_in = readbytes;
						client_to_server.next_in = (unsigned char *) buff;
						client_to_server.avail_out = 1024;
						client_to_server.next_out = (unsigned char *) compression_buf;

						do {
							inflate(&client_to_server, Z_SYNC_FLUSH);
						} while (client_to_server.avail_in > 0);

						for (i = 0; (unsigned int) i < 1024 - client_to_server.avail_out; i++) {
							if (compression_buf[i] == ETX) {
								kill(pid, SIGINT); 
							} else if (compression_buf[i] == EOT) {
							    EOT_Flag = 1;
							} else if (compression_buf[i] == CR || compression_buf[i] == LF) { 
								char lf = LF;
								write(fd2c[1], &lf, 1);
							} else { 
								write(fd2c[1], (compression_buf + i), 1);
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
				else if (stdin_revents & POLLERR)
					error("Error: Poll with socket failed.");

				if (shell_revents == POLLIN) {	//the shell has input
					char buff[256];
					int readbytes = read(fd2p[0], &buff, 256); 
					int count = 0;
					int j;
					for (i = 0, j = 0; i < readbytes; i++) {
						if (buff[i] == EOT) //process EOF from shell
							EOT_Flag = 1;
						else if (buff[i] == LF) {
							if(compress_flag){
								char compression_buf[256];
								server_to_client.avail_in = count;
								server_to_client.next_in = (unsigned char *) (buff + j);
								server_to_client.avail_out = 256;
								server_to_client.next_out = (unsigned char *) compression_buf;

								do {
									deflate(&server_to_client, Z_SYNC_FLUSH);
								} while (server_to_client.avail_in > 0);

								write(newsockfd, compression_buf, 256 - server_to_client.avail_out);

								//compress crlf
								char compression_buf2[256];
								char crlf_copy[2] = {CR, LF};
								server_to_client.avail_in = 2;
								server_to_client.next_in = (unsigned char *) (crlf_copy);
								server_to_client.avail_out = 256;
								server_to_client.next_out = (unsigned char *) compression_buf2;

								do {
									deflate(&server_to_client, Z_SYNC_FLUSH);
								} while (server_to_client.avail_in > 0);

								write(newsockfd, compression_buf2, 256 - server_to_client.avail_out);
							}
							else{
								write(newsockfd, (buff+j), count);
								// if(write(newsockfd, (buff+j), count)<0)
									// error("Error: writing to socket failed.");
								write(newsockfd, crlf, 2);
							}
							j = j+count+1;
							count = 0;
							continue;
						}
						count++;
					}
					// write(STDOUT_FILENO, (buff+j), count);	
					write(newsockfd, (buff+j), count);
				} 
				else if (shell_revents & POLLERR || shell_revents & POLLHUP) { //detect POLLERR and POLLHUP
					EOT_Flag = 1;
				}
			} 
		}
		//close the remaining pipes
		close(sockfd);
		close(newsockfd);
		close(fd2p[0]);
		close(fd2c[1]);
	  exit_shell();
	  // exit(0);
	}
	else{ //fork failed
		fprintf(stderr, "Error: Fork process failed.\n");
		exit(1);
	}
	if (compress_flag) {
		inflateEnd(&client_to_server);
		deflateEnd(&server_to_client);
	}
	return 0;
}
