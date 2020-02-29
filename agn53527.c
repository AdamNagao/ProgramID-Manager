/*
	Start code for Programming Project 4.
	Create 4 files with proper file extension.
*/
// File 1, pid.h
// This header file is included in pid.c and test.c

//test change
#include <pthread.h>

#define PID_MIN  	300
#define PID_MAX 	350

/* mutex lock for accessing pid_map */
pthread_mutex_t mutex;

int pid_map[PID_MAX+1];

int last;	// last pid in use

//File 2, pid.c

#include "pid.h"
#include <pthread.h>
#include <stdio.h>


int i;
/*
 Allocate the pid map which is a bitmap where 1 at index i represents PID i is taken,
 and where 0 at index i indicates that PID i is available
 */
int allocate_map(void) 
{
	for (i = PID_MIN; i <= PID_MAX; i++) 
		pid_map[i] = 0;
	
		last = PID_MIN;
		
		if (pthread_mutex_init(&mutex,NULL) != 0)
			fprintf(stderr,"Unable to initialize mutex\n");

	return last;
}

/*
  Find a pid for our thread. Return the available PID number or -1 if unavailable
 */
int allocate_pid(void)
{
	int counter;
	/* acquire the mutex lock */
	if (pthread_mutex_lock(&mutex) != 0)
		fprintf(stderr,"Unable to acquire lock\n");

	last = -1; //Last will remain -1 if no PID is found

	//Loop through the possible pids, and find one that is available
	for(counter = 300; counter <=350;counter++){
		if(pid_map[counter] == 1){
			
		} else {
			// we found a pid
			pid_map[counter] = 1;   //Mark this thread wants to take the pid

			last = counter;  //Prepare to return the pid

			counter=9999;   //A silly fix to break out of the for loop
		}
	}

	if (pthread_mutex_unlock(&mutex) != 0)
		fprintf(stderr,"Unable to release mutex\n");

	return last;
}
/*
  Releases a pid by setting the PID index to 0
 */
void release_pid(int pid)
{
	if (pthread_mutex_lock(&mutex) != 0)
		fprintf(stderr,"Unable to acquire mutex\n");

	pid_map[pid] = 0;

	if (pthread_mutex_unlock(&mutex) != 0)
		fprintf(stderr,"Unable to release mutex\n");
}

/*
A function to print the pid map for debugging
*/
void print_pid_map(){
	for(i = PID_MIN; i <= PID_MAX;i++){
		printf("PID %i Value: %i\n", i,pid_map[i]);
	}
}

//File 3, test.c 
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "pid.h"

#define NUM_THREADS 100
#define ITERATIONS	10
#define SLEEP		5

int in_use[PID_MAX + 1];

/**
 * mutex lock used when accessing data structure
 * to ensure there are no duplicate pid's in use.
 */
pthread_mutex_t test_mutex;

 /* 
This is the function called by each thread, if the thread acquires a PID
it will sleep for a random amount of time and then release the PID.
 */
void *allocator(void *param)       
{	
	int i = 0;
	int h;
	int wait;
	int r;

	while(i<3){

		r = rand() % 100;

		h = allocate_pid();

		if(h == -1){
			//No PID was found
		} else {
			//We found a PID, time to do stuff
			printf("PID %i allocated; Sleeping for %i\n",h,r);

			sleep(r);

			release_pid(h);

			printf("PID %i released\n",h);

			i++;
		}
		
	}
}

int main(void)
{

	int i;			    
	pthread_t tids[NUM_THREADS];
	int h;

	// allocate the pid map
	if (allocate_map() == -1)
		return -1;
	
	//Create a lot of threads to test
	for(i = 0; i <=100;i++){
		
		h = pthread_create(&(tids[i]), NULL, &allocator,NULL);

		if(h!= 0){

			printf("Can not create thread: [%i}\n", i);

		} else {

			printf("Thread number %i created\n",i );

		}
	}
	//Join the Threads together, so main waits until they finish
	for(i = 0; i<=100;i++){
		pthread_join(tids[NUM_THREADS], NULL);
	}


	printf("***DONE***\n");

	return 0;
}

# To run, enter 
# make all

all: testpid

testpid: test.o pid.o
	gcc  -lpthread -o testpid test.o pid.o

pid.o: pid.c pid.h
	gcc -c pid.c

test.o: test.c pid.h
	gcc -lpthread -c test.c


