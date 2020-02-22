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
#include <limits.h>
#include <signal.h>

long thrnum = 1;
long elenum = 0;
long iter = 1;
char opt_sync = 0;
int* spin_locks;
long lists = 1;
long lock_time = 0;
// int opt_yield;
pthread_mutex_t* mutexes;

//initializes an empty list.
SortedList_t* head;
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

int hash_num(const char* key) {
  	return (key[0] % lists);
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

void init_locks(){
	//initialize the mutexes
	long i;
	if(opt_sync == 'm'){
		mutexes = malloc(lists*sizeof(pthread_mutex_t));
		if(mutexes==NULL)
			error("Error: failed to allocate memory for mutexes.");
		for(i=0; i<lists; i++){
			if (pthread_mutex_init(mutexes+i, NULL)!=0)
				error("Error: failed to create mutexes.");
		}
	}//initialize the spin locks
	else if(opt_sync == 's'){
		spin_locks = malloc(lists * sizeof(int));
		if(spin_locks==NULL)
			error("Error: failed to allocate memory for spin_locks.");
		for(i=0; i<lists; i++)
			spin_locks[i]=0;
	}
}

void clean_locks(){
	long i;
	if (opt_sync == 'm') {
  	for(i=0; i<lists; i++)
			pthread_mutex_destroy(mutexes+i);
  	free(mutexes);
	}
	else if(opt_sync == 's'){
		free(spin_locks);
	}
}

void lock(long i){
	if(opt_sync == 'm'){
		pthread_mutex_lock(mutexes+i);
	}
	else if(opt_sync == 's'){
		while(__sync_lock_test_and_set(spin_locks+i, 1));
	}
}

void unlock(long i){
	if (opt_sync == 'm') {
    pthread_mutex_unlock(mutexes + i);
	} else if (opt_sync == 's') {
 		__sync_lock_release(spin_locks + i);
	}
}

void* run_thrd(void *arg){
	SortedListElement_t* cur_list = arg;
	long i;
	struct timespec begin, end;
	for(i = 0; i<iter; i++){
		int hash = hash_num(cur_list[i].key);
		clock_gettime(CLOCK_MONOTONIC, &begin);
		//initiate the locks
		lock(hash);
		clock_gettime(CLOCK_MONOTONIC, &end);
		lock_time += 1000000000*(end.tv_sec-begin.tv_sec) + end.tv_nsec-begin.tv_nsec;
		//Insertion
		SortedList_insert(head + hash, (SortedListElement_t *) (cur_list+i));
		unlock(hash);

	}
	//get the total length of all lists
	long list_length = 0;
	for (i = 0; i < lists; i++) {
		clock_gettime(CLOCK_MONOTONIC, &begin);
 		lock(i);
		clock_gettime(CLOCK_MONOTONIC, &end);
    lock_time += 1000000000*(end.tv_sec-begin.tv_sec) + end.tv_nsec-begin.tv_nsec;
	}
	for (i = 0; i < lists; i++) {
    		list_length += SortedList_length(head+i);
	}
	if(list_length < iter){
		error2("Error: some elements not inserted into the list.");
	}
	for (i = 0; i < lists; i++) {
		unlock(i);
  }
	//look up the elements and delete them
	SortedListElement_t *res;
	for(i = 0; i < iter; i++){
		int hash = hash_num(cur_list[i].key);
		clock_gettime(CLOCK_MONOTONIC, &begin);
		//initiate the locks
		lock(hash);
		clock_gettime(CLOCK_MONOTONIC, &end);
		lock_time += 1000000000*(end.tv_sec-begin.tv_sec) + end.tv_nsec-begin.tv_nsec;
		res = SortedList_lookup(head+hash, cur_list[i].key);
		if(res == NULL)
			error2("Error: failed to find the element.");
		if( SortedList_delete(res) != 0)
			error2("Error: failed to delete the element.");
		unlock(hash);
	}
  return NULL;
}

void show_stat(long run_time){
	long oper = thrnum * iter * 3;
	long oper_time = run_time/oper;
	long wait_avertime = lock_time/oper;
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

	printf("%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", type, thrnum, iter, lists, oper, run_time, oper_time, wait_avertime);
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
		{"sync", 1, NULL, 's'},
		{"lists", 1, NULL, 'l'}};
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
			case 'l':
			 lists = atoi(optarg);
			 break;
			case '?':
				fprintf(stderr, "Error. Invald option.\n");
				exit(1);
		}
	}
	
	//initialize empty lists
	head = malloc(sizeof(SortedList_t) * lists);
	if(head == NULL)
		error("Error: failed to allocate memory for heads.");
	for(i=0; i<lists; i++){
		head[i].prev = NULL;
		head[i].next = NULL;
	}

	elenum = thrnum * iter;
	elements = malloc(elenum * sizeof(SortedListElement_t));
	if(elements==NULL)
		error("Error: failed to allocate memory for list elements.");
	init_keys();

	init_locks();

	pthread_t *thrd_ids = malloc(thrnum * sizeof(pthread_t));
  if (thrd_ids == NULL)
   	error("Error: failed to allocate memory for threads.");

	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	for(i = 0; i < thrnum; i++){
		if(pthread_create(&thrd_ids[i], NULL, &run_thrd, (void *) (elements+iter*i)) != 0){
			error("Error: failed to create the threads.");
		}
	}
	for(i = 0; i < thrnum; i++){
		if(pthread_join(thrd_ids[i], NULL)!=0)
			error("Error: failed to join the threads.");
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  //calculate the total length of lists
  long listlen = 0;
  for(i=0; i<lists; i++){
  	listlen += SortedList_length(head+i);
  }
  if(listlen != 0) {
  	error2("The total length for lists is not 0 at the end.");
  }

  clean_locks();

	long run_time = 1000000000 * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
	show_stat(run_time);

	free(elements);
	free(thrd_ids);

	return 0;
}

