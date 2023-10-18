#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>

volatile uint64_t counter = 0;

void *thread(void *arg)
{
	int i;
	for (i = 0; i < 1000000; i++) {
		counter++;
	}
	return NULL;
}

int main()
{
	int err;
	pthread_t t1, t2;

	err = pthread_create(&t1, NULL, thread, (void*)0);
	if (err != 0) {
		fprintf(stderr, "Error creating thread 1: %d\n", err);
		exit(1);
	}

	err = pthread_create(&t2, NULL, thread, (void*)1);
	if (err != 0) {
		fprintf(stderr, "Error creating thread 2: %d\n", err);
		exit(1);
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("Counters: %lu\n", counter);
	
	return 0;
}
