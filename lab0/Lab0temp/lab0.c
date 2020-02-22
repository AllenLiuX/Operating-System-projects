#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

void sighandler(int);
void printHelp();

int main(int argc, char **argv) {
  const char *optstring = "iosc";
  int c;
  // char* in_file = 0;
  char* out_file = 0;
  struct option opts[] = {
    {"input", 1, NULL, 'i'},
    {"output", 1, NULL, 'o'},
    {"segfault", 0, NULL, 's'},
    {"catch", 0, NULL, 'c'}
  };
  int in=0, out=1, seg=0, cat=0;
  while((c=getopt_long(argc, argv, optstring, opts, NULL))!= -1){
    switch(c){
    case 'i':
      // in_file = optarg;
      in = open(optarg, O_RDONLY);
      if (in == -1 ){
        fprintf(stderr, "error: %s. --input file failed: %s\n", strerror(errno),optarg);
        exit(2);
      }
      break;
  	case 'o':
      out_file = optarg;
  	  break; 
    case 's':
      seg = 1;
      break;
    case 'c':
      cat = 1;
      break;
    case '?':
      fprintf(stderr, "invald option.\n");
      printHelp();
      exit(1);
      break;
    }
  }
  //process file redirections
  // if(in_file!=0){
  //   in = open(in_file, O_RDONLY);
  //   if(in==-1){
  //     fprintf(stderr, "error: %s. --input file failed: %s\n", strerror(errno),in_file);
  //     exit(2);
  //   }
  // }
  if(in!=0){
      close(0);
      dup(in);
      close(in);
  } 
  //redirect stdout to output file
  if(out_file!=0){
    out = creat(out_file, 0666);
    if(out==-1){
      fprintf(stderr, "error: %s. --output file failed: %s\n", strerror(errno), out_file);
      exit(3);
    }
  }
  if(out != 1){
    close(1);
    dup(out);
    close(out);
    } 
  //register signal handler
  if(cat == 1)
  	signal(SIGSEGV, sighandler);
  //process segfault
  if(seg == 1){
    char *ptr = NULL;
    *ptr = 'a';
  }
  //redirect stdin to stdout  
  int flag;
  char buffer[2];                                 
  while ((flag = read(0, buffer, 1)) > 0)
  {
    write(1, buffer, flag);
  }
  close(0);
  close(1);
  return 0;
}

void printHelp(){
  printf("Usage:\n--input=filename		use the specified file as standard input\n\
--output=filename		create the specified file and use it as standard output\n\
-s, --segfault			force a segmentation fault\n\
-c, --catch			catches the segmentation fault, logs an error message\n");
}

void sighandler(int signum)
{
  signum = signum+0;
  fprintf(stderr, "SIGSEGV\n");
  exit(4);
}

