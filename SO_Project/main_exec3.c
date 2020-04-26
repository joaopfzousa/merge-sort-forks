//
//  main_exec3.c
//  SO_Project
//
//  Created by João  Sousa on 28/03/2020.
//  Copyright © 2020 João  Sousa. All rights reserved.
//

//run: gcc main_exec3.c lib_util.c -o all_exec3 -Wall

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
#define MAX_C 1200

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

    printf("length = %d\n", length);
    
    int h = N;
    int flag = 0;

    while(h > 0)
    {
        if(flag == 1)
        {
            h = h/2;
        }
        printf("h = %d\n", h);

        /*
        * Create the pipe
        */
        if(pipe(fds) < 0)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        
        int i = 0;
        for (i = 0; i < h; i++) 
        {
            if((pids[i] = fork()) < 0)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pids[i] == 0) 
            {
                //fecho do seu descritor do lado de leitura do pipe.
                close(fds[0]);
                
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
                
                //tudo ok até aqui
                char msg[BUF_SIZE], *pos = msg;
                int w = 0;
                //printf("init = %d, final = %d total = %d\n",  init, final, total);
                if(total <= MAX_C)
                {
                    for(int j = init; j < final; j++)
                    {
                        if(j == init)
                        {
                            pos += sprintf(pos, "#%d*%d;%d*", getpid(), init, final); 
                        }else if(j > init){
                            pos += sprintf(pos, ",");
                        }
                        pos += sprintf(pos, "%d", bufInts[j]);

                        if(j == final - 1)
                        {
                            pos += sprintf(pos, "|\n");
                        }        
                    }
                    writen(fds[1], msg, BUF_SIZE);
                }else{
                    //printf("entrei aqui\n");
                    while((total /w) > MAX_C)
                    {
                        w++;
                    }

                    printf("w = %d\n", w);

                    char msg_nova[BUF_SIZE], *pos_nova = msg_nova;

                    int l = 0;
                    for(l = 0; l < w; l++)
                    {
                        //printf("l =  %d ", l);
                        int start = 0;
                        int fim = 0;

                        if(l == 0)
                        {
                            start = init;
                            fim = ((l + 1) * final / w);
                            //printf(" filho i = 0 filho %d , init = %d, final = %d\n", l, start, fim);
                        }else if( l == w - 1)
                        {
                            start = (l  * final / w) + 1 ;
                            fim = final;
                            //printf("filho final %d , init = %d, final = %d\n", l, start, fim);
                        }else if( l != 0 && l < h && l != w - 1 )
                        {
                            start = (l * final / w) + 1;
                            fim = ((l + 1) * final / w);
                            //printf("filhos do meio  %d , init = %d, final = %d,\n", l, start, fim);
                            
                        }

                        printf("init = %d, final = %d\n", start, fim);
                        for(int j = start; j <= fim; j++)
                        {
                            if(j == start)
                            {
                                pos_nova += sprintf(pos_nova, "#%d*%d;%d*", getpid(), start, fim); 
                            }
                            
                            if(j > start){
                                pos_nova += sprintf(pos_nova, ",");  
                            }

                            pos_nova += sprintf(pos_nova, "%d", bufInts[j]);

                            if(j == fim )
                            {
                                pos_nova += sprintf(pos_nova, "|\n");
                            }
                        }
                        writen(fds[1], msg_nova, BUF_SIZE);
                    }
                }
                //fecho do seu descritor do lado de escrita do pipe.
                close(fds[1]);
                exit(EXIT_SUCCESS);
            }
        }

        /**
         * Até aqui tudo ok
         * */
          
        
        //fecho do seu descritor do lado de escrita do pipe.
        close(fds[1]); 

        char buf_readn[BUF_SIZE];
        char * arrayMessage[BUF_SIZE];
        int tamanho = 0;
        ssize_t bytes;
        
        //le os dados enviados pelos filhos via pipe
        while((bytes = readn(fds[0], buf_readn, BUF_SIZE)) != 0)
        {
            printf("buf = %s\n\n", buf_readn);
            char * message = strtok(buf_readn, "|");
            while(message != NULL)
            {
                arrayMessage[tamanho] = message;
                tamanho ++;
                //printf("message = %s", message);
                message = strtok(NULL, "|");
            }
        }

        int k = 0;
        for(k = 0; k < tamanho; k++)
        {
            //printf("arrayMessage[%d] = %s\n", k, arrayMessage[k]);
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

        
            //printf("init = %d, fim = %d, total  = %d\n", init, fim, (fim - init));
            //printf("tamnhoArrayInts = %d", tamnhoArrayInts - 1 );
            int t = init, q = 0;

            for (t = init; t <= fim; t++)
            {
                bufInts[t] = newArrayInts[q];
                if(q <= tamnhoArrayInts)
                {
                    q++; 
                }
                //printf ("t = %d, bufInts[%d] = %d, q = %d, newArrayInts[%d] = %d \n", t, t, bufInts[t], q, q, newArrayInts[q]);
            }
            
            tamanhoAsterisco = 0;
            tamanhoPontoVirgula = 0;
            tamnhoArrayInts = 0;
        }

        if(h == 1)
        {
            h = 0;
        }

        flag = 1;

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
    }

    
    printf("\n\nArray { %d, ", bufInts[0]);
    for(int t = 1; t < length - 1 ; t++)
    {
        printf(" %d,", bufInts[t]);
    }
    printf("%d }\n", bufInts[length]);

    exit(EXIT_SUCCESS);
}