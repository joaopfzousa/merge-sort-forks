//
//  main.c
//  SO_Project
//
//  Created by João  Sousa on 23/03/2020.
//  Copyright © 2020 João  Sousa. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>

#include "lib_util.h"

/** 
 * Nº filhos e nº divisoes do array int's 
 * */
#define N 5

/** 
 * Buffsize 
 * */
#define BUFFSIZE 4096


void handler(int signal_number)
{
    char msg[200];
    switch (signal_number)
    {
    case SIGUSR1:
        sprintf(msg, "SIGUSR1 = %d \n", signal_number);
        write(1, msg, strlen(msg));
        break;
    default:
        sprintf(msg, "Signal = %d \n", signal_number);
        write(1, msg, strlen(msg));
        break;
    }
}

int main(int argc, const char *argv[])
{
    pid_t pids[N];
    ssize_t n;

    /**
     * instalacao do sinal
     * */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);

    /**
     *  1 - Ler e armazenar o conjunto de valores a ordenar
     * */
    int fd;
    char *token;
    char buf[BUFFSIZE];
    int bufInts[BUFFSIZE];
    int length = 0;

    /**
     * abrir o ficheiro
     * */
    if ((fd = open("int.txt", O_CREAT | O_RDWR, 0644)) == -1)
    {
        perror("oops, not open file");
    }

    /**
     * ler o ficheiro até ao final
     * */
    while ((n = read(fd, buf, BUFFSIZE)) != 0)
    {
        token = strtok(buf, ",");
        while (token != NULL)
        {
            int valInt = atoi(token);
            bufInts[length] = valInt;
            length++;
            token = strtok(NULL, ",");
        }
    }
     
    /**
     * Criar N filhos
     * */
    for (int i = 0; i < N; i++)
    {
        /**
         * criar filhos
         * */
        if ((pids[i] = fork()) == -1)
        {
            perror("oops, my fork call failed");
            exit(1);
        }

        /**
         * filhos bem criados
         * */
        if (pids[i] == 0)
        {
            int newArray[length];
            int total = 0, init = 0, final = 0;

            if(i == 0)
            {
                init = 0;
                final = ((i + 1) * length / N) - 1 ;
                total = final;
                //printf("filho %d , init = %d, final = %d, total = %d\n", i, init, final, total);
                mergesort_recursivo(bufInts, newArray, init, final);
            }else if( i != 0 && i < N && i != N -1 )
            {
                init = (i * length / N);
                final = ((i + 1) * length / N) - 1;
                total = final - init;
                //printf("filho %d , init = %d, final = %d, total = %d\n", i, init, final, total);
                mergesort_recursivo(bufInts, newArray, init, final);
            }else if( i == N - 1)
            {
                init = (i  * length / N);
                final = length - 1 ;
                total = final - init;
                //printf("filho %d , init = %d, final = %d, total = %d\n", i, init, final, total);
                mergesort_recursivo(bufInts, newArray, init, final);
            }
            
            char file[10];
            char msg[200], *pos = msg;
            int fd_child;
            for(int j = init; j <= final; j++)
            {
                sprintf(file, "%d.txt", i);
                
                if ((fd_child = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0777)) == -1)
                {
                    perror("oops, not open file");
                }
                
                if(j > 0){
                    pos += sprintf(pos, ", ");  
                }
                pos += sprintf(pos, "%d", bufInts[j]);
            }
            write(fd_child, msg, strlen(msg));

            kill(getppid(), SIGUSR1);
            exit(0);
        }
    }
    
    for (int k = 0; k < N; k++)
    {
        //esperar pelos filhos
        waitpid(pids[k], NULL, 0);
    }

    return 0;
}
