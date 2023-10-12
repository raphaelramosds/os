/**
 * Criação de Processos no Linux
 * 
 * Aluno: Raphael Ramos da Silva
 * Matricula: 20200081884
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    // Ponteiro para armazenar os termos da série de Fibonacci
    int* fib;
    // Variáveis de loop e para armazenar a quantidade de termos desejada
    int i, num;      

    // Variável para armazenar o ID do processo
    pid_t pid;       

    // Solicita ao usuário a quantidade de termos desejados na série de Fibonacci
    printf("\nDigite quantidade de termos para exibir na serie de Fibonacci: ");
    scanf("%d", &num);

    // Verifica se a quantidade de termos é válida (deve ser positiva e não nula)
    if (num <= 0) {
        printf("\nQuantidade de termos deve ser um numero positivo e nao-nulo!\n");
        return 1;
    }

    // Aloca dinamicamente memória para armazenar os termos da série de Fibonacci
    fib = (int*)calloc(num, sizeof(int));

    // Verifica se a alocação de memória foi bem-sucedida
    if (!fib) {
        printf("\nSem espaco suficiente na memoria para armazenar os termos da Serie de Fibonacci!\n");
        return 1;
    }

    // Cria um processo filho
    pid = fork();

    // Verifica se houve um erro na criação do processo filho
    if (pid < 0) {
        printf("\nErro na criacao de processo filho!\n");
        return 1;
    } 

    // Processo filho
    else if (pid == 0) {
        // Inicializa os dois primeiros termos da série de Fibonacci
        fib[0] = 0;
        fib[1] = 1;

        // Calcula os termos restantes da série de Fibonacci
        for (i = 2; i < num; i++) {
            fib[i] = fib[i - 1] + fib[i - 2];
        }

        // Exibe os termos da série de Fibonacci
        printf("\n\t\t");
        for (i = 0; i < num; i++) {
            printf(" %d ", fib[i]);
        }
        printf("\n\n");

        // Libera a memória alocada
        free(fib);
    } 
    
    // Processo pai
    else { 
        // Pai espera até que o processo filho termine
        wait(NULL);
        printf("\nProcesso filho encerrou a execucao");
    }

    return 0;
}
