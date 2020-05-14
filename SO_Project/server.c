#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 4096                                                       // block transfer size
#define LISTENQ 8                                                          // Size of the listen Queue
#define N 5                                                               // Nº de filhos                                                 
 
char *socket_path = "/tmp/socket";                                          // Unix domain socket name

int prepare_socket(struct sockaddr_un channel_srv, char *  socket_path){

    int listenfd;

    if ( (listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {              // Creating the server socket
        perror("socket error");
        exit(-1);
    }
    
    unlink(socket_path);
    
    memset(&channel_srv, 0, sizeof(channel_srv));
    channel_srv.sun_family = AF_UNIX;
    strncpy(channel_srv.sun_path, socket_path, sizeof(channel_srv.sun_path)-1);
    
    if (bind(listenfd, (struct sockaddr*)&channel_srv, sizeof(channel_srv)) == -1) {      // Binding the server socket to its name
        perror("bind error");
        exit(-1);
    }
    if (listen(listenfd, LISTENQ) == -1) {                                  // Configuring the listen queue
        perror("listen error");
        exit(-1);
    }
    //ate aqui a instalação
    return listenfd;
}

int main(int argc, char *argv[])
{
    int listenfd,connfd,fd,bytes, i = 0;
    char buf[BUF_SIZE];                                                     // buffer for outgoing file instalação do socket
    struct sockaddr_un channel_srv;
    int pids[2];

    listenfd = prepare_socket(channel_srv, socket_path);

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

    char *ints;
    char bufRead[BUF_SIZE];
    int bufInts[BUF_SIZE];
    int length = 0;
    ssize_t h;

    while ((h = read(fd, bufRead, BUF_SIZE)) != 0)
    {
        ints = strtok(bufRead, ",");
        while (ints != NULL)
        {
            int valInt = atoi(ints);
            bufInts[length] = valInt;
            length++;
            ints = strtok(NULL, ",");
        }
    }

    int n = N;
    int flag = 0;
    while(n > 0)
    {
        if(flag == 1)
        {
            n = n/2;
        }

        for(i = 0; i < n; i++)
        {
            pids[i] = fork();
            if(pids[i] < 0)
            {
                perror("Erro....");
                exit(1);
            }

            if(pids[i] == 0)
            {
                int init = 0, final = 0;
                if(i == 0)
                {
                    init = 0;
                    final = ((i + 1) * length / n);
                }else if( i != 0 && i < n && i != n - 1 )
                {
                    init = (i * length / n) + 1;
                    final = ((i + 1) * length / n);
                }else if( i == n - 1)
                {
                    init = (i  * length / n) + 1 ;
                    final = length;
                }

                char inicial[1000], fim[1000];
                sprintf(inicial, "%d", init);
                sprintf(fim, "%d", final);
                char *args[5] = {"client.out", "int.txt", inicial, fim, NULL};
                execv("client.out", args);
                perror("Execv:");
                exit(1);
            }
        }

        for(i = 0; i < n; i++)
        {
            if((connfd = accept(listenfd, NULL, NULL)) == -1)
            {
                perror("accepts error;");
                continue;
            }

            char buf[BUF_SIZE];
            char * arrayMessage[BUF_SIZE];
            int tamanho = 0;

            while((bytes = read(connfd, buf, BUF_SIZE)) != 0)
            {
                char * message = strtok(buf, "|");
                while(message != NULL)
                {
                    arrayMessage[tamanho] = message;
                    tamanho ++;
                    printf("message = %s\n", message);
                    message = strtok(NULL, "|");
                }
                int k = 0;
                for(k = 0; k < 1; k++)
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
                        printf("bufInts[%d] = %d\n", t, bufInts[t]);
                    }

                    tamanhoAsterisco = 0;
                    tamanhoPontoVirgula = 0;
                    tamnhoArrayInts = 0;
                }
                flag = 1;
            }
        } 
    }
}