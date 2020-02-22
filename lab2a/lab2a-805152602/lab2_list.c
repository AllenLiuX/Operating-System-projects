//NAME: Wenxuan Liu
//EMAIL: allenliux01@163.com
//ID: 805152602

#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <signal.h>

long thrnum = 1;
long elenum = 0;
long iter = 1;
char opt_sync = 0;
int spin_lock = 0;
// int opt_yield;
pthread_mutex_t mutex;

//initializes an empty list.
SortedList_t head;
SortedListElement_t* elements; 

void error(char *msg){
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void error2(char *msg){
	fprintf(stderr, "%s\n", msg);
	exit(2);
}

void segfault_handler() {
	error2("Error: segmentation fault.");
}

void init_keys(){
	int i, j;
	for(i=0; i<elenum; i++){
		char* temp_key = malloc(6*sizeof(char));
		for(j=0; j<6; j++){
			temp_key[j] = rand()%10 + '0';
		}
		temp_key[6] = '\0';
		(elements+i)->key = temp_key;
	}
}

void* run_thrd(void *arg){
	SortedListElement_t* cur_list = arg;
	long i;
	//initiate the locks
	if(opt_sync == 'm'){
		pthread_mutex_lock(&mutex);
	}
	else if(opt_sync == 's'){
		while(__sync_lock_test_and_set(&spin_lock, 1));
	}
	//Insertion
	for(i = 0; i < iter; i++) {
		SortedList_insert(&head, (SortedListElement_t *) (cur_list + i));
	}
	if(SortedList_length(&head) < iter){
		error2("Error: some elements not inserted into the list.");
	}
	//look up the elements and delete them
	SortedListElement_t *res;
	for(i = 0; i < iter; i++){
		res = SortedList_lookup(&head, cur_list[i].key);
		if(res == NULL)
			error2("Error: failed to find the element.");
		if( SortedList_delete(res) != 0)
			error2("Error: failed to delete the element.");
	}

	//release the locks
	if(opt_sync == 'm') {
    pthread_mutex_unlock(&mutex);
	} 
	else if(opt_sync == 's') {
    __sync_lock_release(&spin_lock);
  }
  return NULL;
}

int main(int argc, char **argv){
	//parse the options in the argument
	opt_yield = 0;
	char c;
	int i;
	unsigned long j;
	signal(SIGSEGV, segfault_handler);
	struct option opts[] = {
		{"threads", 1, NULL, 't'},
		{"iterations", 1, NULL, 'i'},
		{"yield", 1, NULL, 'y'},
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
				for(j=0; j<strlen(optarg); j++){
					if(optarg[j] == 'i')
						opt_yield |= INSERT_YIELD;
					else if(optarg[j] == 'd')
						opt_yield |= DELETE_YIELD;
					else if(optarg[j] == 'l')
						opt_yield |= LOOKUP_YIELD;
				}
				break;
			case 's':
				if(strlen(optarg) != 1 || (optarg[0]!='m' && optarg[0]!='s'))
					error("Error: --sync= must has input with exactly one character 'm' or 's'.");
				opt_sync = optarg[0];
				break;
			case '?':
				fprintf(stderr, "Error. Invald option.\n");
				exit(1);
		}
	}
	
	// //initialize empty list
	// head = malloc(sizeof(SortedList_t));
	// head->next = head;
	// head->prev = head;
	// head->key = NULL;

	elenum = thrnum * iter;
	elements = malloc(elenum * sizeof(SortedListElement_t));
	if(elements==NULL)
		error("Error: failed to allocate memory for list elements.");
	init_keys();

	if(opt_sync == 'm'){
		if (pthread_mutex_init(&mutex, NULL)!=0)
			error("Error: failed to create mutex.");
	}

	pthread_t *thrd_ids = malloc(thrnum * sizeof(pthread_t));
  if (thrd_ids == NULL)
   	error("Error: failed to allocate memory for threads.");

	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);

	for(i = 0; i < thrnum; i++){
		if(pthread_create(&thrd_ids[i], NULL, &run_thrd, (void *) (elements + iter * i)) != 0){
			error("Error: failed to create the threads.");
		}
	}

	for(i = 0; i < thrnum; i++){
  		if(pthread_join(thrd_ids[i], NULL)!=0)
  			error("Error: failed to join the threads.");
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  if(SortedList_length(&head) != 0) {
  	error2("The list length is not 0 at the end.");
  }

  if (opt_sync == 'm') {
		pthread_mutex_destroy(&mutex);
	}

  long oper = thrnum * iter * 3;
	long run_time = 1000000000 * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
	long oper_time = run_time/oper;
//clasify the option type:
	char type[100] = "list";
	if(opt_yield == 0)
		strcat(type, "-none");
	else if(opt_yield == 1)
		strcat(type, "-i");
	else if(opt_yield == 2)
		strcat(type, "-d");
	else if(opt_yield == 3)
		strcat(type, "-id");
	else if(opt_yield == 4)
		strcat(type, "-l");
	else if(opt_yield == 5)
		strcat(type, "-il");
	else if(opt_yield == 6)
		strcat(type, "-dl");
	else if(opt_yield == 7)
		strcat(type, "-idl");

	if(opt_sync == 0)
		strcat(type, "-none");
	else if(opt_sync == 's')
		strcat(type, "-s");
	else if(opt_sync == 'm')
		strcat(type, "-m");

	printf("%s,%ld,%ld,1,%ld,%ld,%ld\n", type, thrnum, iter, oper, run_time, oper_time);

	free(elements);
	free(thrd_ids);

	return 0;
}

