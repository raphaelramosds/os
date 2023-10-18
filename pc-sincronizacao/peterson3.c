#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>

volatile uint64_t counter = 0;

#define FALSE 0
#define TRUE  1
#define N     2                      /* number of processes */

volatile int turn;                            /* whose turn is it? */
volatile int interested[N];                   /* all values initially 0 (FALSE) */

void enter_region(int process)       /* process is 0 or 1 */
{
    int other;                      /* number of the other process */
    other = 1 - process;            /* the opposite of process */
    interested[process] = TRUE;     /* show that you are interested */
    turn = process;                 /* set flag */
    atomic_thread_fence(memory_order_seq_cst);
    while (turn == process && interested[other] == TRUE) /* null statement */ ;
}

void leave_region(int process)       /* process: who is leaving */
{
    interested[process] = FALSE;    /* indicate departure from critical region */
}


void *thread(void *arg)
{
	int i, proc = (size_t)arg;

	for (i = 0; i < 1000000; i++) {
		enter_region(proc);
		counter++;
		leave_region(proc);
	}

	return NULL;
}

int main()
{
	int err;
	pthread_t t1, t2;
	cpu_set_t cpuset;

	err = pthread_create(&t1, NULL, thread, (void*)0);
	if (err != 0) {
		fprintf(stderr, "Error creating thread 1: %d\n", err);
		exit(1);
	}

	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	pthread_setaffinity_np(t1, sizeof(cpuset), &cpuset);

	err = pthread_create(&t2, NULL, thread, (void*)1);
	if (err != 0) {
		fprintf(stderr, "Error creating thread 2: %d\n", err);
		exit(1);
	}

	CPU_ZERO(&cpuset);
	CPU_SET(2, &cpuset);
	pthread_setaffinity_np(t2, sizeof(cpuset), &cpuset);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("Counters: %lu\n", counter);
	
	return 0;
}
