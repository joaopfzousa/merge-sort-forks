# Projeto de Sistemas Operativos

Deve ser implementado um programa que receba por *argv[] um caminho para um ficheiro onde com um grande volume de dados a ordenar. O programa deve implementar o algoritmo de ordenação
MergeSort.

O programa deve ler e armazenar em memória o input de valores lidos do ficheiro. 

Na fase de ordenação, o programa deve seguir uma filosofia de divisão e conquista, para isso, divide o conjunto de valores lidos em N blocos. Com recurso à chamada ao sistema fork(), o programa deve criar N filhos, cada um responsável por ordenar um dos blocos, em intervalos previamente definidos pelo pai. Na primeira fase, os processos filho devem devolver as subsequências ordenadas ao processo pai recorrendo a um protocolo de mensagens indicado no exemplo seguinte:

                                    #pid*index_start;index_end*sequência|

    ● pid: identificador do processo filho.
    ● index_start: limite inferior do intervalo a ordenar.
    ● index_end: limite superior do intervalo a ordenar.
    ● sequência: sequência numérica ordenados a retornar ao pai.

## Requisitos

1. Ler e armazenar o conjunto de valores a ordenar.

2. Criar N filhos, cada um deles deve ordenar a sua subsequência. Após a ordenação deve criar um ficheiro onde vai escrever a subsequência ordenada.No final o processo filho deve enviar um sinal ao pai SIGURS1 a notificar que a sua subsequência já se encontra ordenada. O pai deve esperar pela finalização dos seus filhos.

3. Esta etapa implica que o programa permita suportar a comunicação entre processos com recurso a pipes. Cada filho deve retornar a sua subsequência ordenada para o pai através do pipe. O programa deve suportar um protocolo de comunicação que permita ao pai saber que filho é que ordenou determinada subsequência e a que intervalo pertence. O pai recebe as subsequências ordenadas e guarda as subsequências no array original. Nesta etapa o programa deve fazer recurso da função readn e writen.

                                a. Protocolo: #pid*index_start;index_end*sequencia|

4. Esta etapa implica que o programa permita suportar a comunicação entre processos com recurso a Unix Domin Sokects. Cada filho deve estabelecer conexão com o server (pai). O pai deve atender as conexões e armazenar as subsequências ordenadas array original.


## Correr o projeto

###  Correr main.c

```bash
gcc main.c lib_util.c -o main.out -Wall 

./main.out
```

###  Correr main_exec3.c

```bash
gcc main_exec3.c lib_util.c -o all_exec3 -Wall

./all_exec3
```

###  Correr server.c e client.c

```bash
gcc client.c lib_util.c -o client.out -Wall  

gcc server.c -o sever.out

./sever.out
```

## License
[MIT](https://choosealicense.com/licenses/mit/)