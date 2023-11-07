# Avaliação - Sistemas Operacionais: Sincronização

- Aluno: Raphael Ramos da Silva
- Matricula: 20200081884

## Problema

Considere um ambiente onde a seguinte API para semáforos está disponível

```c
struct sem {
    /* ... */
};

void sem_init(struct sem *s, unsigned val);
void sem_inc(struct sem *s);
void sem_dec(struct sem *s);
```

Onde `sem_init` inicializa o semáforo com o valor `val`; `sem_inc` incrementa o valor do semáforo em uma unidade (valor_novo = valor + 1), também acordando alguma *thread* que tenha dormido esperando para decrementar o valor; e finalmente `sem_dec` decrementa o valor do semáforo em uma unidade (valor_novo = valor - 1) garantindo que o valor nunca fique negativo, para tanto ela pode ter que pôr a *thread* para dormir quando o valor do semáforo for zero até que haja um outro incremento.

Implemente, para esse ambiente, a API para variáveis de condição descrita no item *Variável de condição a partir de um mutex*. A diferença entre a questão aparesentada nesse item e esta é o ponto de partida: na primeira, a API de *mutex* está disponível, enquanto que aqui a API de semáforo está disponível

## Desenvolvimento

Uma variável de condição controla a concorrência entre tarefas que tentam acessar um determinado recurso a partir de um sinal de controle e um *mutex*

O sinal de controle é emitido por uma tarefa caso alguma condição for satisfeita. Enquanto essa condição não é satisfeita, as outras tarefas que esperam o cumprimento dessa condição *dormem* (param sua execução)

No momento em que tal condição é satisfeita, todas as tarefas continuam sua execução. Normalmente, é neste momento em que elas tentam acessar a região crítica, porém, apenas uma delas obterá êxito, graças ao mutex.

Nesse raciocínio, desenvolvi a implementação de uma API para variável de condição a partir de um semáforo tendo em mente que o sinal de controle e o mutex poderiam ser construídos com um semáforo binário. 

Utilizei as seguintes *libraries* da linguagem C para desenvolver essa API

```c
#include <stdio.h>
#include <semaphore.h>
#include <stdatomic.h>
```

A estrutura básica dessa implementação é o `struct condvar`, a qual possui as seguintes propriedades

- `sem_t semaphore` realiza o papel de um semáforo binário
- `atomic_uint waiting_threads` gerencia atomicamente a contagem de threads que estão esperando 

```c
struct condvar {
    sem_t semaphore;
    atomic_uint waiting_threads;
};
```

A função de inicialização da variável de condição possui assinatura `condvar_init(struct condvar *c)`. Ela inicializa em zero o contador do semáforo da estrutura `condvar` e faz com que esse semáforo não seja compartilhado com outros processos. Além disso, essa função zera o número de *threads* em espera

```c
void condvar_init(struct condvar *c) {
    sem_init(&c->semaphore, 0, 0);
    c->waiting_threads = 0;
}
```

A função que emite o sinal de controle possui assinatura `condvar_signal(struct condvar* c)`. A *thread* que a invoca incrementa o semáforo binário `c->semaphore`, caso haja alguma outra *thread* em espera. Esse comportamento sinaliza o cumprimento da variável de condição.

```c
void condvar_signal(struct condvar *c) {
    if (c->waiting_threads > 0) {
        sem_post(&c->semaphore);
    }
}
```

A função de espera da variável de condição possui assinatura `condvar_wait(struct condvar* c, sem_t* mutex)`. Seu funcionamento consiste em 

- Incrementar o semáforo `mutex` para dar oportunidade para outras threads esperarem pela condição ser satisfeita
- Fazer a thread esperar pelo sinal de controle decrementando `c->semaphore`

Uma vez que esse sinal é emitido, essa mesma thread acorda e adquire a trava para garantir a exclusão mútua no acesso à região crítica. 

```c
void condvar_wait(struct condvar *c, sem_t *mutex) {
    c->waiting_threads++;
    sem_post(mutex);
    sem_wait(&c->semaphore);
    sem_wait(mutex);
    c->waiting_threads--;
}
```

## Teste

## Conclusões
