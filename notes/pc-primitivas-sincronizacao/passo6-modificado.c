#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <assert.h>

struct mutex {
	atomic_uint v;
};

void mutex_init(struct mutex *m)
{
	m->v = 0;
}

void mutex_travar(struct mutex *m)
{
	for (;;) {
		uint32_t v = 0;
		if (atomic_compare_exchange_strong(&m->v, &v, 1)) {
			break;
		}
	}
}

void mutex_destravar(struct mutex *m)
{
	atomic_store(&m->v, 0);
}

struct dormindo {
	pthread_mutex_t mutex;
	struct dormindo *proximo;
};

struct semaforo {
	pthread_mutex_t mutex;
	unsigned valor;
	struct dormindo *primeiro;
	struct dormindo *ultimo;
};

void semaforo_init(struct semaforo *s, unsigned valor)
{
	s->valor = valor;
	pthread_mutex_init(&s->mutex, NULL);
	s->primeiro = NULL;
	s->ultimo = NULL;
}

void semaforo_inc(struct semaforo *s)
{
	pthread_mutex_lock(&s->mutex);
	s->valor++;
	if (s->primeiro != NULL) {
		struct dormindo *d = s->primeiro;
		assert(s->valor == 1);
		s->primeiro = d->proximo;
		if (s->primeiro == NULL) {
			s->ultimo = NULL;
		}
		pthread_mutex_unlock(&d->mutex);
	}
	pthread_mutex_unlock(&s->mutex);
}

void semaforo_dec(struct semaforo *s)
{
	pthread_mutex_lock(&s->mutex);
	while (s->valor == 0) {
		struct dormindo d;
		pthread_mutex_init(&d.mutex, NULL);
		pthread_mutex_lock(&d.mutex);

		d.proximo = NULL;
		if (s->ultimo) {
			s->ultimo->proximo = &d;
		} else {
			s->primeiro = &d;
		}
		s->ultimo = &d;

		pthread_mutex_unlock(&s->mutex);
		pthread_mutex_lock(&d.mutex);
		pthread_mutex_lock(&s->mutex);
	}
	s->valor--;
	pthread_mutex_unlock(&s->mutex);
}

#define TAMANHO 10

volatile size_t inserir = 0;
volatile size_t remover = 0;

volatile int dados[TAMANHO];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct semaforo sem;

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
			semaforo_inc(&sem);
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
			semaforo_dec(&sem);
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

	semaforo_init(&sem, 0);

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
