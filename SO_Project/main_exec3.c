//
//  main_exec3.c
//  SO_Project
//
//  Created by João  Sousa on 28/03/2020.
//  Copyright © 2020 João  Sousa. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib_util.h"

#define N 7
#define BUF_SIZE 4096

ssize_t /* Read "n" bytes from a descriptor */
readn(int fd, void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    nleft = n;

    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break; /* error, return amount read so far */
        } else if (nread == 0) {
                break; /* EOF */
        }
        nleft -= nread;
        ptr += nread;
    }
    return(n - nleft); /* return >= 0 */
}
ssize_t /* Write "n" bytes to a descriptor */
writen(int fd, const void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    nleft = n;

    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break; /* error, return amount written so far */
        } else if (nwritten == 0) {
            break;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return(n - nleft); /* return >= 0 */
}

int main(int argc, char *argv[])
{ 

    int pids[N], fds[2];
    ssize_t n;

    int fd;
    char *token;
    char bufRead[BUF_SIZE];
    int bufInts[BUF_SIZE];
    int length = 0;


    /**
     * abrir o ficheiro
     * */
    if ((fd = open("int.txt", O_CREAT | O_RDWR, 0644)) == -1)
    {
        perror("oops, not open file");
        exit(EXIT_FAILURE);
    }

    /**
     * ler o ficheiro até ao final
     * */
    while ((n = read(fd, bufRead, BUF_SIZE)) != 0)
    {
        token = strtok(bufRead, ",");
        while (token != NULL)
        {
            int valInt = atoi(token);
            bufInts[length] = valInt;
            length++;
            token = strtok(NULL, ",");
        }
    }

    //printf("length = %d\n", length);
    
    int h = N;
    int flag = 0;

    while(h > 0)
    {
        if(flag == 1)
        {
            h = h/2;
        }
        /*
        * Create the pipe
        */
        if(pipe(fds) < 0)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        
        //while(N < 0 )
        //N = N/2;
        int i = 0;
        for (i = 0; i < h; i++) {

    
            if((pids[i] = fork()) < 0)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pids[i] == 0) {

                //fecho do seu descritor do lado de leitura do pipe.
                close(fds[0]);
                
                int *newArray = (int *) malloc(sizeof(int) * length);
                int total = 0, init = 0, final = 0;

                if(i == 0)
                {
                    init = 0;
                    final = ((i + 1) * length / h);
                    total = final;
                    //printf(" filho i = 0 filho %d , init = %d, final = %d, total = %d\n", i, init, final, total);
                    mergesort_run(bufInts, total, init, final);
                }else if( i != 0 && i < h && i != h - 1 )
                {
                    init = (i * length / h) + 1;
                    final = ((i + 1) * length / h);
                    total = final - init;
                    //printf("filhos do meio  %d , init = %d, final = %d, total = %d\n", i, init, final, total);
                    mergesort_run(bufInts, total, init, final);
                }else if( i == h - 1)
                {
                    init = (i  * length / h) + 1 ;
                    final = length;
                    total = final - init;
                    //printf("filho final %d , init = %d, final = %d, total = %d\n", i, init, final, total);
                    mergesort_run(bufInts, total, init, final);
                }

                free(newArray);
                
                char msg[BUF_SIZE], *pos = msg;
                for(int j = init; j <= final; j++)
                {
                    if(j == init)
                    {
                        pos += sprintf(pos, "#%d*%d;%d*", getpid(), init, final); 
                    }
                    
                    if(j > init){
                        pos += sprintf(pos, ",");  
                    }

                    pos += sprintf(pos, "%d", bufInts[j]);

                    if(j == final)
                    {
                        pos += sprintf(pos, "|\n");
                    }
                }
                printf("msg = %s", msg);
                writen(fds[1], msg, strlen(msg));

                //fecho do seu descritor do lado de escrita do pipe.
                close(fds[1]);
                exit(EXIT_SUCCESS);
            }
        }

        //fecho do seu descritor do lado de escrita do pipe.
        close(fds[1]); 

        char buf[BUF_SIZE];
        char * arrayMessage[BUF_SIZE];
        int tamanho = 0;
        ssize_t bytes;
        
        //le os dados enviados pelos filhos via pipe
        while((bytes = readn(fds[0], buf, BUF_SIZE)) != 0)
        {
            printf("buf = %s", buf);
            char * message = strtok(buf, "|");
            while(message != NULL)
            {
                arrayMessage[tamanho] = message;
                tamanho ++;
                //printf("message = %s", message);
                message = strtok(NULL, "|");
            }
        }

        printf("\n\n\n");
        int k = 0;
        for(k = 0; k < tamanho; k++)
        {
            //printf("arrayMessage[%d] = %s\n", k, arrayMessage[k]);
        }

        //printf("\n\n");

        for(k = 0; k < tamanho; k++)
        {
            char * arrayAsterisco[BUF_SIZE];
            int tamanhoAsterisco = 0;
            int * arrayPontoVirgula = (int *) malloc(sizeof(int) * length);
            int * newArrayInts = (int *) malloc(sizeof(int) * length);
            int tamanhoPontoVirgula = 0;
            int tamnhoArrayInts = 0;

            char * asterisco = strtok(arrayMessage[k], "*");
            while(asterisco != NULL)
            {
                arrayAsterisco[tamanhoAsterisco] = asterisco;
                tamanhoAsterisco ++;
                asterisco = strtok(NULL, "*");
            }

            for(int r = 1; r < tamanhoAsterisco; r++)
            {
                if(r == 1)
                {
                    char * pontovirgula = strtok(arrayAsterisco[r], ";");
                    while(pontovirgula != NULL)
                    {
                        int valInt = atoi(pontovirgula);
                        arrayPontoVirgula[tamanhoPontoVirgula] = valInt;
                        tamanhoPontoVirgula ++;
                        pontovirgula = strtok(NULL, ";");
                    }
                }else if(r == 2){
                    char * virgula = strtok(arrayAsterisco[r], ",");
                    while(virgula != NULL)
                    {
                        int valInt = atoi(virgula);
                        newArrayInts[tamnhoArrayInts] = valInt;
                        tamnhoArrayInts ++;
                        virgula = strtok(NULL, ",");
                    }
                }
            }
            int init = 0, fim = 0;
            init = arrayPontoVirgula[0];
            fim = arrayPontoVirgula[1];

            printf("\n\n\n");
            for(int h = 0; h < tamnhoArrayInts; h++)
            {
                //printf("newArrayInts[%d] = %d\n", h, newArrayInts[h]);
            }

            //printf("init = %d, fim = %d, total  = %d\n", init, fim, (fim - init));
            //printf("tamnhoArrayInts = %d", tamnhoArrayInts - 1 );
            int t = init, q = 0;

            for (t = init; t <= fim; t++)
            {
                //printf ("t = %d, bufInts[%d] = %d, q = %d, newArrayInts[%d] = %d \n", t, t, bufInts[t], q, q, newArrayInts[q]);
                bufInts[t] = newArrayInts[q];
                if(q <= tamnhoArrayInts)
                {
                    q++; 
                }
            }

            for(int t = 0; t < length; t++)
            {
                //printf("bufInts[%d] = %d\n", t, bufInts[t]);
            }

            tamanhoAsterisco = 0;
            tamanhoPontoVirgula = 0;
            tamnhoArrayInts = 0;
        }
        flag = 1;
    }
   
    int pid = -1;
    int status, j = 0;
    for(j=0; j < N; j++){
        
        pid = waitpid(pids[j], &status, 0);

        if (WIFEXITED(status)) {
            /*
            WEXITSTATUS(status) - returns the exit status of the child.
            */
            //printf("PID %d Status: %d\n", pid, WEXITSTATUS(status));
        }
    }

    exit(EXIT_SUCCESS);
}