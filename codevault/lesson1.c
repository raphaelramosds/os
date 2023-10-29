#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* routine() {
	printf("Test from threads \n");
	sleep(3);
	printf("Ending thread \n");
}

int main(int argc, char* argv[]) {
	pthread_t t1,t2;
	
	// Create two threads
	if (pthread_create(&t1, NULL, &routine, NULL) != 0)return -1;

	if (pthread_create(&t2, NULL, &routine, NULL) != 0) return -2;

	// Wait thread to finish it execution
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;		  
}
