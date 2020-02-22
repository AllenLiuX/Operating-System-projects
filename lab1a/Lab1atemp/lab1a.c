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

struct termios saved;
static int shell_flag = 0;
int     fd2p[2], fd2c[2], nbytes;
pid_t   childpid;



void restore_input_mode (void) {
	tcsetattr (STDIN_FILENO, TCSANOW, &saved);
}

void set_input_mode (void){
	struct termios tattr;
	char *name;

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
		fprintf(stderr, "Error. Setting terminal mode failed.");
		exit(1);
	}
}

void create_pipe(int p[2]){
	if (pipe(p) == -1){ //create pipe
		fprintf(stderr, "Error. Creating pipe failed.");
		exit(1);
	}
}

int main (int argc, char **argv){
	char c;
	const char *optstring = "s";
	struct option opts[] = {{"shell", 0, NULL, 's'}};
	while((c=getopt_long(argc, argv, optstring, opts, NULL))!= -1){
		switch(c){
			case 's':
				shell_flag = 1;
				break;
			case '?':
				fprintf(stderr, "invald option.\n");
				exit(1);
		}
	}


	set_input_mode ();

	while (1) {
		read (STDIN_FILENO, &c, 1);
		if (c == '\004')          
			break;
		else
			putchar (c);
	}

	return EXIT_SUCCESS;
}
