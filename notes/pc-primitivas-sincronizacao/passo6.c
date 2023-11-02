#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define TAMANHO 10

volatile size_t inserir = 0;
volatile size_t remover = 0;

volatile int dados[TAMANHO];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem;

void *produtor(void *arg)
{
	for (;;) {
		int v = rand();
		if (v % 10 == 0 && v != 0) {
			pthread_mutex_lock(&mutex);
			while (((inserir + 1) % TAMANHO) == remover) {
				pthread_mutex_unlock(&mutex);
				pthread_mutex_lock(&mutex);
			}
			printf("Produzindo %d\n", v);
			dados[inserir] = v;
			inserir = (inserir + 1) % TAMANHO;
			pthread_mutex_unlock(&mutex);
			sem_post(&sem);
		}
		usleep(50000);
	}

	return NULL;
}

void *consumidor(void *arg)
{
	for (;;) {
		pthread_mutex_lock(&mutex);
		while (inserir == remover) {
			pthread_mutex_unlock(&mutex);
			sem_wait(&sem);
			pthread_mutex_lock(&mutex);
		}
		printf("%zu: Consumindo %d\n", (size_t)arg, dados[remover]);
		remover = (remover + 1) % TAMANHO;
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

int main()
{
	int err;
	pthread_t t1, t2;
	cpu_set_t cpuset;

	sem_init(&sem, 0, 0);

	err = pthread_create(&t1, NULL, produtor, (void*)0);
	if (err != 0) {
		fprintf(stderr, "Error creating thread 1: %d\n", err);
		exit(1);
	}

	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	pthread_setaffinity_np(t1, sizeof(cpuset), &cpuset);

	err = pthread_create(&t2, NULL, consumidor, (void*)1);
	if (err != 0) {
		fprintf(stderr, "Error creating thread 2: %d\n", err);
		exit(1);
	}

	CPU_ZERO(&cpuset);
	CPU_SET(2, &cpuset);
	pthread_setaffinity_np(t2, sizeof(cpuset), &cpuset);

	{
		pthread_t t3;

		err = pthread_create(&t3, NULL, consumidor, (void*)2);
		if (err != 0) {
			fprintf(stderr, "Error creating thread 3: %d\n", err);
			exit(1);
		}

		CPU_ZERO(&cpuset);
		CPU_SET(3, &cpuset);
		pthread_setaffinity_np(t3, sizeof(cpuset), &cpuset);

		pthread_join(t3, NULL);
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
