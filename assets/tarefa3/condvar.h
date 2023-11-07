// =================================================================
// No contexto de semáforos binários, uma thread tenta entrar na re
// gião crítica decrementando o contador: ela possui sucesso caso o
// valor no semáforo for maior que zero, e dorme caso seja zero
//
// Implementei uma variável de condição a partir de um semáforo com
// dois semáforos binários: mutex e semaphore
//
// mutex      :   protege a seção crítica, evitando que threads sina
//                lizadas entrem ao mesmo tempo nela
// semaphore  :   permite que as threads esperem e sejam sinalizadas
//
//
// Na função condvar_signal, a thread sinalize que a condição foi satisfeita. 
// Simulei esse compartamento incrementando o contador do semáforo
//
// Na função condvar_wait, a ideia é que as threads parem temporaria
// mente a sua execução enquanto não são sinalizadas. Simulei esse
// comportamento com o decremento do semáforo: caso a thread chame
// a função sem_wait para decrementar o contador de semaphore, e ele
// for igual a zero, então a condição ainda não foi satisfeita. Caso
// a thread consiga decrementar, ela não pode simplesmente continuar
// a execução e entrar na região crítica, pois mais de uma thread pode
// conseguir decrementar. Portanto, criei a variável mutex, que atua
// como um semáforo binário: a thread que foi despertada pelo signal
// agora vai tentar adquirir a trava com sem_wait
// =================================================================
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