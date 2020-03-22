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
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <mraa.h>
#include <mraa/aio.h>

#define A0 1
#define GPIO_50 60

int period = 1;
char scale = 'F';
int show_stat = 1;
FILE* file;

struct tm* now;
struct timeval clocker;
time_t next_t = 0;

mraa_aio_context temp_sensor;
mraa_gpio_context button;

void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void outputer(char *str, int std) {
	if (std == 1)
		fprintf(stdout, "%s\n", str);
	if (file != 0) {
		fprintf(file, "%s\n", str);
		fflush(file);
	}
}

void end() {
	now = localtime(&clocker.tv_sec);
	char out[100];
	sprintf(out, "%02d:%02d:%02d SHUTDOWN", now->tm_hour, now->tm_min, now->tm_sec);
	outputer(out,1);
	exit(0);
}

void init_mraa(){
	temp_sensor = mraa_aio_init(A0);
	button = mraa_gpio_init(GPIO_50);

	mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &end, NULL);
}

void close_mraa(){
	mraa_aio_close(temp_sensor);
  mraa_gpio_close(button);
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
	if (show_stat && clocker.tv_sec >= next_t) {
		now = localtime(&clocker.tv_sec);
		float t = get_temp();
		char stats[100];
  	sprintf(stats, "%02d:%02d:%02d %.1f", now->tm_hour, now->tm_min, now->tm_sec, t);
  	outputer(stats, 1);
   	next_t = clocker.tv_sec + period; 
	}
}

void stdin_parser(char *buff) {
	buff[strlen(buff)-1] = '\0';
	while(*buff == ' ')
		buff++;
	char* log_pos = strstr(buff, "LOG");
	char* per_pos = strstr(buff, "PERIOD=");

	if(strcmp(buff, "STOP") == 0) {
		// fprintf(stdout, "%s\n", buff);
		show_stat = 0;
		outputer(buff, 0);
	} else if(strcmp(buff, "START") == 0) {
		// fprintf(stdout, "%s\n", buff);
		show_stat = 1;
		outputer(buff, 0);
	} else if(strcmp(buff, "OFF") == 0) {
		// fprintf(stdout, "%s\n", buff);
		outputer(buff, 0);
		end();
	} else if(strcmp(buff, "SCALE=C") == 0) {
		// fprintf(stdout, "%s\n", buff);
		scale = 'C'; 
		outputer(buff, 0);
	} else if(strcmp(buff, "SCALE=F") == 0) {
		// fprintf(stdout, "%s\n", buff);
		scale = 'F'; 
		outputer(buff, 0);
	} else if(per_pos == buff) {
		char* d = buff + 7;
		if(*d != 0) {
			int p = atoi(d);
			while(*d != 0) {
				if (!isdigit(*d)) {
					return;
					// error("Error: invalid argument for period=.");
				}
				d++;
			}
			period = p;
		}
		outputer(buff, 0);
		// fprintf(stdout, "%s\n", buff);
	} else if (log_pos == buff) {
		outputer(buff, 0);
		// fprintf(stdout, "%s\n", buff);
	}
}

void poll_process(){
	struct pollfd poll_fd;
  poll_fd.fd = STDIN_FILENO; 
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
   		fgets(buff, 1024, stdin);
   		stdin_parser(buff); 
 		}
	}
}

int main(int argc, char **argv){
	int c;
	struct option opts[] = {
		{"scale", 1, NULL, 's'},
		{"period", 1, NULL, 'p'},
		{"log", 1, NULL, 'l'}};
	while((c=getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 's':
				if(optarg[0]=='C' || optarg[0]=='F')
					scale = optarg[0];
				else
					error("Error: Wrong argument for --scale.");
				break;
			case 'p':
				period = atoi(optarg);
				break;
			case 'l':
				file = fopen(optarg, "w+");
				if(file == NULL)
					error("Error: Failed to open the file.");
				break;
			case '?':
				fprintf(stderr, "Error. Invald option.\n");
				exit(1);
		}
	}

	init_mraa();

	poll_process();

	close_mraa();

	return 0;
}
