#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5

// Um processo precisa de pelo menos uma thread para poder executar

/**
 * @details A thread eh uma funcao que tem sua propria stack e registradores (PC e IR)
 * Sua estrutura eh uma funcao que retorna um ponteiro para void e recebe um ponteiro 
 * para void. Note que na linha 19 fazemos um casting para (int) desse ponteiro, pois
 * ele representa o tipo de dado que vamos trabalhar. No exemplo_thread2.c, veja que
 * esse tipo de dado é até qual valor inteiro precisamos somar seus valores antecessores
*/
void *PrintHello(void *threadid) {
    int tid;
    tid = (int)threadid;
    printf("Olah, mundo! Sou eu, thread #%d!\n", tid);
    // Encerrar a thread
    pthread_exit(NULL);
}

// gcc exemplo_thread1.c -pthread

int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    int t;
    for (t = 0; t < NUM_THREADS; t++) {
        printf("Thread mae (main): criando thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
        if (rc) {
            printf("ERROR! Codigo retornado de pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Última ação da função main()
    pthread_exit(NULL);
}
