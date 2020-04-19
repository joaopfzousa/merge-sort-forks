#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "lib_util.h"

char *socket_path = "/tmp/socket";

#define BUF_SIZE 4096			/* block transfer size */

int main(int argc, char **argv)
{
    int uds ;
    struct sockaddr_un channel;		/* Unix Domain socket */
     
    if (argc != 4) {
        printf("Usage: client file-name\n");
        exit(1);
    }
    
    if ((uds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
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

    /**
     * ler o ficheiro até ao final
     * */

    char *ints;
    char bufRead[BUF_SIZE];
    int bufInts[BUF_SIZE];
    int length = 0;
    ssize_t n;

    while ((n = read(fd, bufRead, BUF_SIZE)) != 0)
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

    int total = 0, init = 0, final = 0;

    init = atoi(argv[2]);
    final = atoi(argv[3]);
    total = final - init;

    mergesort_run(bufInts, total, init, final);

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
    
    write(uds, msg, strlen(msg));
    
    close(fd);
    close(uds);
}