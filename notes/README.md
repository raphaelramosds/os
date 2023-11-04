# GNU Debug

## Entrar no GDB

Criar executável debugável

```
gcc -g main.c -o main
```

Abrir para degub

```
gdb fonte
```

Execute o comando abaixo duas vezes para colocar seu código abaixo da sua versão em Assembly

```c
lay next
```

> Na interface do GDB, para executar o mesmo comando que você executou antes, pressione ENTER

Colocar breakpoint na função F 

```
break F
```

Iniciar execução

```
run
```

## Comandos

#### Execução

Remover todos os breakpoints

```
clear
```

Executar próxima linha

```
next
```

Continuar execução até o próximo breakpoint

```
continue
```

Reiniciar o debug (zera os registradores e volta para o estado inicial do breakpoint que você definiu)

```
r
```

#### Visualização

Listar N primeiras linhas do codigo fonte

```
list N
```

Exibir valor da variável C

```
print C
```

Exibir valor de todas as variáveis locais

```
info locals
```

Exibir valores dos registradores

```
info registers
```