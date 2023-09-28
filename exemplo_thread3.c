#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define TAMANHO 10

int buffer[N][TAMANHO];

/**
 * @details Thread que recebe uma linha da matriz buffer com dimensao
 * (N,TAMANHO), e coloca na ultima coluna o resultado da soma das
 * entradas de 0 ate TAMANHOA-1
*/
void *soma_linha(void *ptr) {
    int indice = 0;
    int soma = 0;
    int *b = (int *)ptr;

    while (indice < TAMANHO - 1) {
		// Soma linha
        soma += b[indice++];
    }
	// Guardar soma na ultima coluna
    b[indice] = soma;
	// Saida normal da thread
    return NULL;
}

int main(int argc, char *argv[]) {
    int i, j;
    pthread_t tid[N];

	// Iniciar matriz
    for (i = 0; i < N; i++) {
        for (j = 0; j < TAMANHO - 1; j++) buffer[i][j] = rand() % 10;
    }

	// Criar threads
    for (i = 0; i < N; i++) 
    {
        // Cada thread vai ficar responsavel por uma linha da matriz 
        // (void *)&(buffer[i]) eh o endereco de memoria para i-esima linha
        if (pthread_create(&tid[i], 0, soma_linha, (void *)&(buffer[i])) != 0) 
        {
            printf("Erro na criacao da thread\n");
            exit(1);
        } else
            printf("Criada a thread %d\n", i);
    }

	// Sincronizar threads
    for (i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("Terminaram todas as threads!\n");

	// Mostrar saida do programa
    for (i = 0; i < N; i++) {
        for (j = 0; j < TAMANHO; j++) printf(" %3d ", buffer[i][j]);
        printf("linha %d \n", i);
    }
    exit(0);
}
