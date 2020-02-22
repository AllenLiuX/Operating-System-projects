//NAME: Wenxuan Liu
//EMAIL: allenliux01@163.com
//ID: 805152602

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <string.h>

long thrnum = 1;
long iter = 1;
long long counter = 0;
int opt_yield = 0;
char sync = 0;
int lock = 0;
pthread_mutex_t m_lock; 

void error(char *msg){
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield) {
    sched_yield();
	}
  *pointer = sum;
}

void add_m(long long *pointer, long long value) {
 	pthread_mutex_lock(&m_lock);
  long long sum = *pointer + value;
  if (opt_yield) {
   	sched_yield();
	}
 	*pointer = sum;
  pthread_mutex_unlock(&m_lock);
}

void add_s(long long *pointer, long long value) {
	while (__sync_lock_test_and_set(&lock, 1));
	long long sum = *pointer + value;
	if (opt_yield) {
    sched_yield();
   }
  *pointer = sum;
 	__sync_lock_release(&lock);
}

void add_c(long long *pointer, long long value) {
	long long old;
	do {
		old = counter;
	  if (opt_yield) {
	    sched_yield();
	  }
  } 
  while (__sync_val_compare_and_swap(pointer, old, old+value)!=old);
}

void* add_parser(void* arg) {
	long i;	
	for (i = 0; i < iter; i++) {
		if (sync == 'm') {
			add_m(&counter, 1);
			add_m(&counter, -1);
		} else if (sync == 's') {
			add_s(&counter, 1);
			add_s(&counter, -1);
		} else if (sync == 'c') {
			add_c(&counter, 1);
			add_c(&counter, -1);
		} else {
			add(&counter, 1);
			add(&counter, -1);
		}
	}
	return arg;
}

int main(int argc, char **argv){
	//parse the options in the argument
	char c;
	struct option opts[] = {
		{"threads", 1, NULL, 't'},
		{"iterations", 1, NULL, 'i'},
		{"yield", 0, NULL, 'y'},
		{"sync", 1, NULL, 's'}};
	while((c=getopt_long(argc, argv, "", opts, NULL))!= -1){
		switch(c){
			case 't':
				thrnum = atoi(optarg);
				break;
			case 'i':
				iter = atoi(optarg);
				break;
			case 'y':
				opt_yield = 1;
				break;
			case 's':
				if(strlen(optarg) != 1)
					error("Error: --sync= must has input with exactly one character.");
				sync = optarg[0];
				break;
			case '?':
				fprintf(stderr, "Error. Invald option.\n");
				exit(1);
		}
	}

	counter = 0;
	//get the beginning time
	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	//allocate memory for an array of threads
	pthread_t *thrd_ids = malloc(thrnum * sizeof(pthread_t));
	if(thrd_ids == NULL)
		error("Error: failed to allocate thrnum memory.");

	if(sync == 'm'){
		if(pthread_mutex_init(&m_lock, NULL) != 0)
			error("Error: failed to initiate mutex.");
	}

	int i;
	for(i=0; i<thrnum; i++){
		if(pthread_create(&thrd_ids[i], NULL, &add_parser, NULL)!=0)
			error("Error: failed to create or execute thread.");
	}

	//end the threads
	for (i = 0; i < thrnum; i++) {
		if(pthread_join(thrd_ids[i], NULL) != 0 )
			error("Error: failed to join the thread.");
	}

	if(sync == 'm'){
		if(pthread_mutex_destroy(&m_lock)!=0)
			error("Error: failed to destroy mutex.");
	}

	//get the ending time
	clock_gettime(CLOCK_MONOTONIC, &end);

	free(thrd_ids);

	long oper = thrnum * iter * 2;
	long run_time = 1000000000 * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
	long oper_time = run_time/oper;
//print the data
	char type[30] = "add-";
	if(opt_yield)
		strcat(type, "yield-");
	if(sync==0)
		strcat(type, "none");
	if(sync=='m')
		strcat(type, "m");
	if(sync=='s')
		strcat(type, "s");
	if(sync=='c')
		strcat(type, "c");
	fprintf(stdout, "%s,%ld,%ld,%ld,%ld,%ld,%lld\n", type, thrnum, iter, oper, run_time, oper_time, counter);

 //  if (opt_yield == 0 && sync == 0)
 //    fprintf(stdout, "add-none,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
	// else if (opt_yield == 0 && sync == 'm')
 //    fprintf(stdout, "add-m,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
 //  else if (opt_yield == 0 && sync == 's')
 //    fprintf(stdout, "add-s,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
 //  else if (opt_yield == 0 && sync == 'c')
 //    fprintf(stdout, "add-c,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
 //  else if (opt_yield == 1 && sync == 0)
 //    fprintf(stdout, "add-yield-none,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
 //  else if (opt_yield == 1 && sync == 'm')
	//   fprintf(stdout, "add-yield-m,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
 //  else if (opt_yield == 1 && sync == 's')
 //    fprintf(stdout, "add-yield-s,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);
 //  else if (opt_yield == 1 && sync == 'c')
 //    fprintf(stdout, "add-yield-c,%ld,%ld,%ld,%ld,%ld,%lld\n", thrnum, iter, oper, run_time, oper_time, counter);

	exit(0);
}







