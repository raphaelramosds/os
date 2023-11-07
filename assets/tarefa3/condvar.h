#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

struct condvar {
    sem_t semaphore;
    atomic_uint waiting_threads;
};

void condvar_init(struct condvar *c) {
    sem_init(&c->semaphore, 0, 1);
    c->waiting_threads = 0;
}

void condvar_wait(struct condvar *c, sem_t *mutex) {
    c->waiting_threads++;
    sem_post(mutex);
    sem_wait(&c->semaphore);
    sem_wait(mutex);
    c->waiting_threads--;
}

void condvar_signal(struct condvar *c) {
    if (c->waiting_threads > 0) {
        sem_post(&c->semaphore);
    }
}

void condvar_broadcast(struct condvar *c) {
    while (c->waiting_threads > 0) {
        sem_post(&c->semaphore);
    }
}