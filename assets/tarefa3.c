#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define TAMANHO 10

volatile size_t inserir = 0;
volatile size_t remover = 0;
volatile int dados[TAMANHO];

struct condvar {
    sem_t sem;
};

struct condvar cond;

void condvar_init(struct condvar *c) {
    // Criar semáforo binário
    sem_init(&c->sem, 0, 0);
}

void condvar_wait(struct condvar *c, sem_t *s) {
    // Decremente
    // Como o semaforo é binário, trava a seção crítica
    sem_wait(&c->sem);
}

void condvar_signal(struct condvar *c) {
    // Permitir entrada na região crítica
    // Incremente quando o semáforo não for zero
    sem_post(&c->sem);
}

void condvar_broadcast(struct condvar *c) {
    // Acorde todas as threads esperando pela condição
    while (sem_trywait(&c->sem) == 0);
}

void *produtor(void *arg)
{
	for (;;) {
		int v = rand();
		if (v % 10 == 0 && v != 0) {
			while (((inserir + 1) % TAMANHO) == remover);
			printf("Produzindo %d\n", v);
			dados[inserir] = v;
			inserir = (inserir + 1) % TAMANHO;
		}
		usleep(50000);
	}

	return NULL;
}

void *consumidor(void *arg)
{
	for (;;) {
		while (inserir == remover);
		printf("%zu: Consumindo %d\n", (size_t)arg, dados[remover]);
		remover = (remover + 1) % TAMANHO;
	}

	return NULL;
}

int main()
{
	int err;
	pthread_t t1, t2;
	cpu_set_t cpuset;

    condvar_init(&cond);

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
