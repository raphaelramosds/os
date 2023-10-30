#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Race condition
int mails = 0;

// Mutual exclusion
pthread_mutex_t mutex;

void* routine() {
	for (int i = 0; i < 1000000; i++) {
		// Lock critical section
		pthread_mutex_lock(&mutex);

		mails++; // Non atomic operation: read, increment and write mails

		// Unlock critical section
		pthread_mutex_unlock(&mutex);	
	}
}

int main(int argc, char* argv[]) {
	pthread_t p1, p2;
	pthread_mutex_init(&mutex, NULL);

	if (pthread_create(&p1, NULL, &routine, NULL) != 0) return -1;
	if (pthread_create(&p2, NULL, &routine, NULL) != 0) return -2;

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	pthread_mutex_destroy(&mutex);
	printf("Number of mails: %d\n", mails);
}
