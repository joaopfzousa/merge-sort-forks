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
#define LISTENQ 5                                                          // Size of the listen Queue
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

    int n = N;
    while(n > 0)
    {

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
            char *args[3] = {"client.out", "int.txt", NULL};
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

        printf("connfd = %d\n", connfd);

        while((bytes = read(connfd, buf, BUF_SIZE)) != 0)
        {
            write(1, buf ,(size_t) bytes);
        }
    }
}
    
    