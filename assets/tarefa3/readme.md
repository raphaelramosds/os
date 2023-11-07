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

A função que é usada para fazer uma thread esperar até que uma condição específica seja satisfeita possui assinatura `condvar_signal(struct condvar* c, sem_t* mutex)`. 

O aumento do valor do semáforo mutex com `sem_post(mutex)` permite que outras threads possam adquirir o mutex, o que é importante para que outras threads tenham a oportunidade de entrar na região crítica.

Após aumentar o semáforo mutex, a thread atual espera pelo sinal de controle que será emitido quando a condição for satisfeita. Isso é feito com `sem_wait(&c->semaphore)`.

Quando o sinal de controle é emitido (geralmente, isso é feito por outra thread que percebeu que a condição foi satisfeita), a thread atual acorda e adquire o mutex novamente com `sem_wait(mutex)`. Isso é necessário para garantir a exclusão mútua na região crítica, uma vez que a thread pode ter sido interrompida logo antes de entrar na região crítica e liberado o mutex com `sem_post(mutex)`.

A variável `c->waiting_threads` é incrementada antes de liberar o mutex e decrementada após adquirir o mutex novamente.

```c
void condvar_wait(struct condvar *c, sem_t *mutex) {
    c->waiting_threads++;
    sem_post(mutex);
    sem_wait(&c->semaphore);
    sem_wait(mutex);
    c->waiting_threads--;
}
```

A funçã que acorda todas as *threads* em espera pela condição ser satisfeita possui assinatura `condvar_broadcast(struct condvar* c)`. Ela verifica se há threads em espera (c->waiting_threads > 0) e, se houver, emite sinais para acordar todas elas em um loop com sem_post(&c->semaphore). Isso está em conformidade com o comportamento de broadcast em uma variável de condição.

```c
void condvar_broadcast(struct condvar *c) {
    while (c->waiting_threads > 0) {
        sem_post(&c->semaphore);
    }
}
```

## Teste

## Reflexões

