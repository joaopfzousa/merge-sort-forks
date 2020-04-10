#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

char *socket_path = "/tmp/socket";

#define BUF_SIZE 4096			/* block transfer size */

int main(int argc, char **argv)
{
    int c, uds, bytes;
    char buf[BUF_SIZE];			/* buffer for incoming file */
    struct sockaddr_un channel;		/* Unix Domain socket */
    
    if (argc != 2) {
        printf("Usage: client file-name\n");
        exit(1);
    }
    
    if ( (uds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    
    memset(&channel, 0, sizeof(channel));
    channel.sun_family= AF_UNIX;
    strncpy(channel.sun_path, socket_path, sizeof(channel.sun_path)-1);
    
    if (connect(uds, (struct sockaddr*)&channel, sizeof(channel)) == -1) {
        perror("connect error");
        exit(1);
    }

    int fd; 
    fd = open(argv[1], O_RDONLY);

    if(fd < 0)
    {
        perror("open");
        exit(1);
    }


    char * token;
    while((bytes = read(fd, buf, BUF_SIZE)) != 0)
    {
        token = strtok(buf, "\n");
        
        while(token != NULL)
        {
            char string[100];
            sprintf(string, "|%d*%s#\n", getpid(), token);
            printf("token = %s\n", token);
            write(uds, string, strlen(string));
            token = strtok(NULL, "\n");
        }
    }

    close(fd);
    close(uds);
}