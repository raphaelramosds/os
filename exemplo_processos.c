#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>

int main()
{
	pid_t pid;

	// Cria um processo filho
	pid = fork();

	// Houve erro
	if (pid < 0) {
		printf("\nErro da criacao de processo filho!\n");
		return 1;
	}

	// Processo filho
	else if (pid == 0) {
		// Codigo exclusivo para o processo filho executar
		printf("\nOla! Sou o processo filho!");
		printf("\nMeu identificador (filho) eh o numero %d", getpid());	
		printf("\nEncerrando!\n");
	}

	// Processo pai
	else {
		// Pai esperarra ate que o processo filho termine
		wait(NULL);
		printf("\nOla! Sou o processo pai!");
		printf("\nCriei um processo filho de numero %d. Ele jah encerrou!", pid);
		printf("\nMeu identificador (pai) eh o numero %d", getpid());	
		printf("\nEncerrando!\n");
	}
	return 0;
}

