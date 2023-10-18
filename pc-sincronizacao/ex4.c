#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>

volatile uint64_t counter = 0;
volatile atomic_bool lock;

void enter_region(int process)
{
	bool v;
	do {
		v = 0;
	} while (!atomic_compare_exchange_strong(&lock, &v, 1));
}

void leave_region(int process)
{
	atomic_store(&lock, 0);
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
