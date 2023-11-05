#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t sem;

struct condvar {
    
};

void condvar_init(struct condvar *c) {
    
}

void condvar_wait(struct condvar *c, sem_t *m) {}

void condvar_signal(struct condvar *c) {}

void condvar_broadcast(struct condvar *c) {}

int main() {}