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
int lock = 0;
// int opt_yield;
pthread_mutex_t m_lock;
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
	fprintf(stderr, "Error: segmentation fault.\n");
	exit(2);
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

void deleteElement(SortedListElement_t* element){
	if (element == NULL){
		fprintf(stderr, "error on SortedList_lookup\n");
		exit(2);
	}

	if (SortedList_delete(element)){
		fprintf(stderr, "error on SortedList_delete\n");
		exit(2);
	}
}

void* run_thrd(void* tid){
	// SortedListElement_t* cur_list = arg;
	// //initiate the locks
	// if(opt_sync == 'm'){
	// 	pthread_mutex_lock(&m_lock);
	// }
	// else if(opt_sync == 's'){
	// 	while(__sync_lock_test_and_set(&lock, 1));
	// }
	// //Insertion
	// long i;
	// for(i = 0; i < iter; i++) {
	// 	SortedList_insert(&head, (SortedListElement_t *) (cur_list + i));
	// }
	// long inserted = SortedList_length(&head);
	// if(inserted < iter){
	// 	error2("Error: some elements not inserted into the list.");
	// }

	// SortedListElement_t *res;
	// char *cur_key = malloc(sizeof(char)*256);
	// for(i = 0; i < iter; i++){
	// 	strcpy(cur_key, (cur_list+i)->key);
	// 	res = SortedList_lookup(&head, cur_key);
	// 	if(res == NULL)
	// 		error2("Error: failed to find the element.");
	// 	if( SortedList_delete(res) != 0)
	// 		error2("Error: failed to delete the element.");
	// }

	// //release the locks
	// if(opt_sync == 'm') {
 //    pthread_mutex_unlock(&m_lock);
	// } 
	// else if(opt_sync == 's') {
 //    __sync_lock_release(&lock);
 //  }
 //  return NULL;


	int i = *(int*)tid;
	for(; i<elenum; i+= thrnum){
		if(opt_sync == 0)
			SortedList_insert(head, &elements[i]);
		else if(opt_sync == 'm'){
			pthread_mutex_lock(&m_lock);
			SortedList_insert(head, &elements[i]);
			pthread_mutex_unlock(&m_lock);
		}
		else if(opt_sync == 's'){
			while(__sync_lock_test_and_set(&lock, 1));
			SortedList_insert(head, &elements[i]);
			__sync_lock_release(&lock);
		}
	}

	int len;
	if(opt_sync == 0){
		len = SortedList_length(head);
		if(len==-1)
			error2("Error: failed to get list's length.");
	}
	else if(opt_sync == 'm'){
		pthread_mutex_lock(&m_lock);
		len = SortedList_length(head);
		if(len==-1)
			error2("Error: failed to get list's length.");
		pthread_mutex_unlock(&m_lock);
	}
	else if(opt_sync == 's'){
		while(__sync_lock_test_and_set(&lock, 1));
		len = SortedList_length(head);
		if(len==-1)
			error2("Error: failed to get list's length.");
		__sync_lock_release(&lock);
	}

	SortedListElement_t* element;
	for(i = *(int*)tid; i<elenum; i+= thrnum){
		if(opt_sync == 0){
			element = SortedList_lookup(head, elements[i].key);
			deleteElement(element);
		}
		else if(opt_sync == 'm'){
			pthread_mutex_lock(&m_lock);
			element = SortedList_lookup(head, elements[i].key);
			deleteElement(element);
			pthread_mutex_unlock(&m_lock);
		}
		else if(opt_sync == 's'){
			while(__sync_lock_test_and_set(&lock, 1));
			element = SortedList_lookup(head, elements[i].key);
			deleteElement(element);
			__sync_lock_release(&lock);
		}
	}
	return NULL;
}

int main(int argc, char **argv){
	//parse the options in the argument
	signal(SIGSEGV, segfault_handler);
	opt_yield = 0;
	char c;
	int i;
	unsigned long j;
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
	
	//initialize empty list
	head = malloc(sizeof(SortedList_t));
	head->next = head;
	head->prev = head;
	head->key = NULL;

	elenum = thrnum * iter;
	elements = malloc(elenum * sizeof(SortedListElement_t));
	if(elements==NULL)
		error("Error: failed to allocate memory for list elements.");
	init_keys();

	if(opt_sync == 'm'){
		if (pthread_mutex_init(&m_lock, NULL)!=0)
			error("Error: failed to create mutex.");
	}

	pthread_t *thrd_ids = malloc(thrnum * sizeof(pthread_t));
  if (thrd_ids == NULL)
   	error("Error: failed to allocate memory for threads.");
  int* tids = malloc(thrnum * sizeof(int));

	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);

	// for(i = 0; i < thrnum; i++){
	// 	if(pthread_create(&thrd_ids[i], NULL, &run_thrd, (void *) (elements + iter * i)) != 0){
	// 		error("Error: failed to create the threads.");
	// 	}
	// }
	for(i = 0; i < thrnum; i++){
		if(pthread_create(&thrd_ids[i], NULL, &run_thrd, &tids[i]) != 0)
			error("Error: failed to create the threads.");
	}

	for(i = 0; i < thrnum; i++){
  		if(pthread_join(thrd_ids[i], NULL)!=0)
  			error("Error: failed to join the threads.");
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  if(SortedList_length(head) != 0) {
  	error2("The list length is not 0 at the end.");
  }


  long oper = thrnum * iter * 3;
	long run_time = 1000000000 * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
	long oper_time = run_time/oper;

	char type[50] = "list";
	switch(opt_yield) {
    case 0:
			strcat(type, "-none");
			break;
    case 1:
			strcat(type, "-i");
			break;
 		case 2:
			strcat(type, "-d");
			break;
    case 3:
			strcat(type, "-id");
			break;
    case 4:
			strcat(type, "-l");
			break;
    case 5:
			strcat(type, "-il");
			break;
    case 6:
			strcat(type, "-dl");
			break;
    case 7:
			strcat(type, "-idl");
			break;
    default:
     	break;
	}

	switch(opt_sync) {
  	case 0:
      	strcat(type, "-none");
      	break;
  	case 's':
      	strcat(type, "-s");
      	break;
  	case 'm':
      	strcat(type, "-m");
      	break;
  	default:
      	break;
	}
	printf("%s,%ld,%ld,1,%ld,%ld,%ld\n", type, thrnum, iter, oper, run_time, oper_time);

	free(elements);
	free(thrd_ids);

	if (opt_sync == 'm') {
		pthread_mutex_destroy(&m_lock);
	}

	return 0;
}

