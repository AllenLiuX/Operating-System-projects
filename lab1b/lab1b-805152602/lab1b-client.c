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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h> 
#include <zlib.h>

#define ETX '\3'
#define EOT '\4'
#define CR '\r'
#define LF '\n'
char crlf[2] = {CR, LF};

char *host_name = "localhost";

struct termios saved;

int sockfd, log_fd, n;
struct sockaddr_in serv_addr;
struct hostent* server;

z_stream cli2ser;
z_stream ser2cli;

void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void restore_input_mode (void) {
	close(sockfd);
	close(log_fd);
	tcsetattr (STDIN_FILENO, TCSANOW, &saved);
}

void set_input_mode (void){
	struct termios tattr;

  /* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO)){
		fprintf (stderr, "Error: Not a terminal.\n");
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
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	if (tcsetattr (STDIN_FILENO, TCSANOW, &tattr)<0){
		fprintf(stderr, "Error: Setting terminal mode failed.\n");
		exit(1);
	}
}

void set_compress(){
	cli2ser.zalloc = Z_NULL;
	cli2ser.zfree = Z_NULL;
	cli2ser.opaque = Z_NULL;
	
	ser2cli.zalloc = Z_NULL;
	ser2cli.zfree = Z_NULL;
	ser2cli.opaque = Z_NULL;
}

int main(int argc, char **argv){
	int portno = 0;
	int port_flag = 0;
	int log_flag = 0;
	int compress_flag = 0;

	struct option opts[] = {
		{"port", 1, NULL, 'p'},
		{"log", 1, NULL, 'l'},
		{"compress", 0, NULL, 'c'},
		// {"host", 1, NULL, 'h'},
		{"debug", 0, NULL, 'd'}};

	char c;
	while((c=getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 'p':
				port_flag = 1;
				portno = atoi(optarg);
				break;
			case 'l':
				log_flag = 1;
				log_fd = creat(optarg, 0666);
				if(log_fd<0)
					error("Error: create or write to log file failed.");
				break;
			case 'c':
				compress_flag = 1;
				set_compress();
				if (inflateInit(&ser2cli) != Z_OK) {
					error("Error: inflateInit on client failed.");
				}
				if (deflateInit(&cli2ser, Z_DEFAULT_COMPRESSION) != Z_OK) {
					error("Error: deflateInit on client failed.");
				}
				break;
			// case 'd':
			// 	debug_flag = 1;
			// 	break;
			// case 'h':
			// 	host_name = optarg;
			// 	break;
			case '?':
				fprintf(stderr, "Error: Invald option.\n");
				exit(1);
				break;
		}
	}

	if(!port_flag){
		printf("Error: --port is mandatory.\n");
		exit(1);
	}

	set_input_mode();
	//======setting up client=======
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    error("ERROR opening socket");
  }
  server = gethostbyname(host_name);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  // serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("Error: connecting to server failed.");
  }

  // write(sockfd, crlf, 2);	//fix minor bug, clear the first input.  

  struct pollfd poll_fds[] = {
		{STDIN_FILENO, POLLIN, 0}, //keyboard (stdin)
		{sockfd, POLLIN, 0} //socket
	};
	int i, num;
	char str[20];
	char nl = '\n';
	char sent[10] = "SENT ";
  char receive[20] = "RECEIVED ";
  char bytes[20] = " bytes: ";
	while(1){
		if (poll(poll_fds, 2, 0) > 0) {
			short stdin_revents = poll_fds[0].revents;
			short socket_revents = poll_fds[1].revents;
			if (stdin_revents == POLLIN) {
				char buff[256];
				num = read(STDIN_FILENO, &buff, 256);
				for (i = 0; i < num; i++) {
					if (buff[i] == CR || buff[i] == LF) {
						write(STDOUT_FILENO, crlf, 2);
					} else {
						write(STDOUT_FILENO, (buff + i), 1);
					}
				}
				if(compress_flag && num>0){
					char combuff[256];
					cli2ser.avail_in = num;
					cli2ser.next_in = (unsigned char *) buff;
					cli2ser.avail_out = 256;
					cli2ser.next_out = (unsigned char *) combuff;
					do {
						deflate(&cli2ser, Z_SYNC_FLUSH);
					} while (cli2ser.avail_in > 0);
					write(sockfd, combuff, 256 - cli2ser.avail_out);
					if(log_flag){
						sprintf(str, "%d", 256-cli2ser.avail_out);
						write(log_fd, sent, strlen(sent));
						write(log_fd, str, strlen(str));
						write(log_fd, bytes, strlen(bytes));
						write(log_fd, buff, 256-cli2ser.avail_out);
						write(log_fd, &nl, 1);
					}
				}
				else{
					write(sockfd, buff, num);
					if(log_flag){
						sprintf(str, "%d", num);
						write(log_fd, sent, strlen(sent));
						write(log_fd, str, strlen(str));
						write(log_fd, bytes, strlen(bytes));
						write(log_fd, buff, num);
						write(log_fd, &nl, 1);
					}
				}
			}
			else if (stdin_revents == POLLERR) {
				error("Error with poll with STDIN.");
			}
			if (socket_revents == POLLIN) {
				char buff[256];
				num = read(sockfd, &buff, 256);
				if (num == 0) {
					break;
				}
				if(compress_flag && num>0){
					char combuff[1024];
					ser2cli.avail_in = num;
					ser2cli.next_in = (unsigned char *) buff;
					ser2cli.avail_out = 1024;
					ser2cli.next_out = (unsigned char *) combuff;

					do {
						if (inflate(&ser2cli, Z_SYNC_FLUSH) != Z_OK)
							error("Error: inflate failed");
					} while (ser2cli.avail_in > 0);

					write(STDOUT_FILENO, combuff, 1024 - ser2cli.avail_out);
				}
				else{
					write(STDOUT_FILENO, buff, num);
				}

				if(log_flag){
					sprintf(str, "%d", num);
					write(log_fd, receive, strlen(receive));
					write(log_fd, str, strlen(str));
					write(log_fd, bytes, strlen(bytes));
					write(log_fd, buff, num);
					write(log_fd, &nl, 1);
				}
			}
			else if (socket_revents & POLLERR || socket_revents & POLLHUP) { //polling error
				exit(0);
			} 
		}
	}
	if (compress_flag) {
		inflateEnd(&ser2cli);
		deflateEnd(&cli2ser);
	}

	return 0;
}