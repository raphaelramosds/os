#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Qual assinatura dessas funções?
// Estou no caminho certo?

struct condvar {
    sem_t sem;
};

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

int main() {}