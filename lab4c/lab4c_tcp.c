//NAME: Wenxuan Liu
//EMAIL: allenliux01@163.com
//ID: 805152602
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <getopt.h>
#include <poll.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <ctype.h>

#define A0 1

char scale = 'F';
int period = 1;
FILE *file = 0;
int report = 1;
int port = -1;
char* id = "";

struct tm *now;
struct timeval clocker;
time_t next_t = 0;
mraa_aio_context temp_sensor; 

struct hostent *server;
struct sockaddr_in serv_addr;
int sock;
char* host = "";


void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void outputer(char *str, int server) {
	if (server) {
		dprintf(sock, "%s\n", str);
	}
	if (file!=0){
		fprintf(file, "%s\n", str);
		fflush(file);
	}
}

void end(){
	now = localtime(&clocker.tv_sec);
	char out[100];
	sprintf(out, "%02d:%02d:%02d SHUTDOWN", now->tm_hour, now->tm_min, now->tm_sec);
  outputer(out, 1);
  exit(0);
}

void init_socket(){
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error("Error: failed to create socket.");
	}
	if ((server = gethostbyname(host)) == NULL) {
		fprintf(stderr, "Error: cannot find host\n");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  // serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(port);
  if (connect(sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("Error: connecting to server failed.");
  }
}

void init_mraa(){
	temp_sensor = mraa_aio_init(A0);
	if (!temp_sensor) {
		mraa_deinit();
		error("Error: failed to initialze AIO.");
  }
}

float get_temp() {
	int temperature = mraa_aio_read(temp_sensor);
	float R = 1023.0/((float) temperature) - 1.0;
	int B = 4275;
	R *= 100000.0;
	float T = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
	if (scale == 'C')
		return T;
	else
		return T * 1.8 + 32;
}

void get_stats() {
	gettimeofday(&clocker, 0);
	if (report && clocker.tv_sec >= next_t) {
		double temp_sensor = get_temp();
		int t = temp_sensor * 10;
		now = localtime(&clocker.tv_sec);
		char out[200];
		sprintf(out, "%02d:%02d:%02d %d.%1d", now->tm_hour, now->tm_min, now->tm_sec, t/10, t%10);
		outputer(out, 1);
		next_t = clocker.tv_sec + period; 
	}
}

void input_parser(char *buff) {
	while(*buff == ' ') {
		buff++;
	}
	char *log_pos = strstr(buff, "LOG");
	char *per_pos = strstr(buff, "PERIOD=");

	if(strcmp(buff, "SCALE=F") == 0) {
		outputer(buff, 0);
		scale = 'F'; 
	} else if(strcmp(buff, "SCALE=C") == 0) {
		outputer(buff, 0);
		scale = 'C'; 
	} else if(strcmp(buff, "STOP") == 0) {
		outputer(buff, 0);
		report = 0;
	} else if(strcmp(buff, "START") == 0) {
		outputer(buff, 0);
		report = 1;
	} else if(strcmp(buff, "OFF") == 0) {
		outputer(buff, 0);
		end();
	} else if(per_pos == buff) {
		char *n = buff+7;
		if(*n != 0) {
			int p = atoi(n);
			while(*n != 0) {
				if (!isdigit(*n)) {
					return;
				}
				n++;
			}
			period = p;
		}
		outputer(buff, 0);
	} else if (log_pos == buff) {
		outputer(buff, 0); 
	}
}

void server_input(char* buff) {
	int ret = read(sock, buff, 256);
	if (ret > 0) {
		buff[ret] = 0;
	}
	char *c = buff;
	while (c < &buff[ret]) {
		char* d = c;
		while (d < &buff[ret] && *d != '\n') {
			d++;
		}
		*d = 0;
		input_parser(c);
		c = &d[1];
	}
}

void poll_process(){
	struct pollfd poll_fd;
  poll_fd.fd = sock;
  poll_fd.events = POLLIN; 
  char* buff = (char *)malloc(1024 * sizeof(char));
  if(buff == NULL)
  	error("Error: failed to allocate memory for buff.");

  while(1) {
  	get_stats();
  	int res = poll(&poll_fd, 1, 0);
  	if(res < 0)
  		error("Error: failed to poll.");
   	if(res) {
   		server_input(buff);
 		}
	}
}

int main(int argc, char* argv[]) {

	struct option opts[] = {
		{"scale", 1, NULL, 's'},
		{"period", 1, NULL, 'p'},
		{"log", 1, NULL, 'l'},
		{"id", 1, NULL, 'i'},
		{"host", 1, NULL, 'h'},
	};

	int c; //parse options
	while ((c = getopt_long(argc, argv, "", opts, NULL)) != -1) {
		switch (c) {
			case 's':
				if (optarg[0] == 'F' || optarg[0] == 'C') {
					scale = optarg[0];
				}
				else
					error("Error: Wrong argument for --scale.");
				break;
			case 'p': 
				period = atoi(optarg);
				break;
			case 'l':
				file = fopen(optarg, "w+");
        if (file == NULL) {
	        error("Error: invalid logfile.");
				}
				break;
			case 'i':
				id = optarg;
				if (strlen(id) != 9)
					error("Error: ID must be a 9 digit number.");
				break;
			case 'h':
				host = optarg;
				if (strlen(host) == 0)
					error("Error: must have --host= argument.");
				break;
			default:
				fprintf(stderr, "Error. Invald option.\n");
				exit(1);
		}
	}
	if (optind < argc) {
		port = atoi(argv[optind]);
		if (port <= 0) {
			error("Error: invalid port number.");
		}
	}

	init_socket();

	char buff2[100];
	sprintf(buff2, "ID=%s", id);
	outputer(buff2, 1);

	init_mraa();

	poll_process();

	mraa_aio_close(temp_sensor);
	
	return 0;
}