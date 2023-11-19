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

Onde `sem_init` inicializa o semáforo com o valor `val`; `sem_inc` incrementa o valor do semáforo em uma unidade ($valor_{novo} = valor + 1$), também acordando alguma *thread* que tenha dormido esperando para decrementar o valor; e finalmente `sem_dec` decrementa o valor do semáforo em uma unidade ($valor_{novo} = valor - 1$) garantindo que o valor nunca fique negativo, para tanto ela pode ter que pôr a *thread* para dormir quando o valor do semáforo for zero até que haja um outro incremento.

O problema consistiu em utilizar essa API para implementar uma variável de condição a partir de um semáforo. Na minha implementação, utilizei os seguintes métodos da biblioteca `semaphore.h` da linguagem C para simular essa API de semáforo

- `sem_post(struct sem* s)` equivale a `sem_inc(struct sem* s)`
- `sem_wait(struct sem* s)` equivale a `sem_dec(struct sem* s)`

## Desenvolvimento

Uma variável de condição controla a concorrência entre tarefas que tentam acessar um determinado recurso a partir de um sinal de controle e um *mutex*

O sinal de controle é emitido por uma tarefa caso alguma condição for satisfeita. Enquanto essa condição não é satisfeita, as outras tarefas que esperam o cumprimento dessa condição *dormem* (param sua execução)

No momento em que tal condição é satisfeita, todas as tarefas continuam sua execução. Normalmente, é neste momento em que elas tentam acessar a região crítica, porém, apenas uma delas obterá êxito, graças ao mutex.

Nesse raciocínio, desenvolvi a implementação de uma API para variável de condição a partir de um semáforo tendo em mente que o sinal de controle poderia ser simulado com o incremento do contador de um semáforo principal e o mutex construído com um semáforo binário. 

Utilizei as seguintes *libraries* da linguagem C para desenvolver essa API

```c
#include <stdio.h>
#include <semaphore.h>
#include <stdatomic.h>
```

A estrutura básica da implementação dessa API é `struct condvar`, a qual possui as propriedades `sem_t semaphore`, que assume o papel do semáforo principal, e `atomic_uint waiting_threads`, que gerencia atomicamente a contagem de threads que estão esperando 

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

A função que emite o sinal de controle possui assinatura `condvar_signal(struct condvar* c)`. A *thread* que a invoca incrementa `c->semaphore`, caso haja alguma outra *thread* em espera. Esse comportamento sinaliza o cumprimento da variável de condição.

```c
void condvar_signal(struct condvar *c) {
    if (c->waiting_threads > 0) {
        sem_post(&c->semaphore);
    }
}
```

A função que é usada para fazer uma thread esperar até que uma condição específica seja satisfeita possui assinatura `condvar_wait(struct condvar* c, sem_t* mutex)`. O aumento do valor do semáforo mutex com `sem_post(mutex)` permite que outras threads possam adquirir o mutex, o que é importante para que outras threads tenham a oportunidade de entrar na região crítica. Após aumentar o semáforo mutex, a thread atual espera pelo sinal de controle que será emitido quando a condição for satisfeita. Isso é feito com `sem_wait(&c->semaphore)`. Quando o sinal de controle é emitido (geralmente, isso é feito por outra thread que percebeu que a condição foi satisfeita), a thread atual acorda e adquire o mutex novamente com `sem_wait(mutex)`. Isso é necessário para garantir a exclusão mútua na região crítica, uma vez que a thread pode ter sido interrompida logo antes de acordar e entrar na região crítica. A variável `c->waiting_threads` é incrementada depois de liberar o mutex e decrementada antes de adquiri-lo, portanto, evita-se condição de corrida nela.

```c
void condvar_wait(struct condvar *c, sem_t *mutex) {
    sem_post(mutex);
    c->waiting_threads++;
    sem_wait(&c->semaphore);
    c->waiting_threads--;
    sem_wait(mutex);
}
```

A função que acorda todas as *threads* em espera pela condição a ser satisfeita possui assinatura `condvar_broadcast(struct condvar* c)`. Ela verifica se há threads em espera (`c->waiting_threads > 0`) e, se houver, emite sinais para acordar todas elas em um loop com sem_post(`&c->semaphore`). 

```c
void condvar_broadcast(struct condvar *c) {
    while (c->waiting_threads > 0) {
        sem_post(&c->semaphore);
    }
}
```

Os arquivos que possuem a solução da avaliação estão hospedados de forma pública neste [repositório do Github](https://github.com/raphaelramosds/os/tree/main/assets/tarefa3)

## Teste

Testei a API que implementei no problema dos produtores e consumidores, neste [link](https://github.com/raphaelramosds/os/blob/main/assets/tarefa3/main.c). O produtor adiciona números aleatórios em um buffer circular de dez posições, e os consumidores removem elementos desse buffer. Utilizei um produtor e dois consumidores.

Os resultados das iterações desse algoritmo pode ser visualizado neste [arquivo de log](https://github.com/raphaelramosds/os/blob/main/assets/tarefa3/log.txt). Nesse log, vemos que obrigatoriamente apenas um dos consumidores remove o dado produzido pelo produtor, o que é um resultado esperado.

## Reflexões

Inicialmente, enfrentei desafios na implementação da função `condvar_wait`. Minha primeira versão não incluía a verificação do mutex, apenas o `sem_wait`, o que levou a um comportamento inesperado: o código ficava travado, como se as threads parassem sua execução para sempre.

Uma melhoria que poderia ser considerada é a implementação de uma fila de threads a serem acordadas pelo `condvar_signal`. Isso permitiria um controle mais preciso sobre quais threads são notificadas quando um sinal é emitido. Atualmente, a função `condvar_signal` acorda apenas uma das threads em espera, mas não há garantia de qual thread será acordada.

Verifiquei que o consumo de CPU durante o teste da API é baixo, o que é uma característica interessante do código. Isso indica que as threads estão realmente suspendendo sua execução quando necessário, evitando a utilização desnecessária de recursos do processador.
