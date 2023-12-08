#include <stdio.h>
#include <sys/types.h>    
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>   //define close
#include <stdlib.h>
#include <string.h>

#define PORT_MAX_LEN 20

int main(int argc, char **argv)
{
    char *port;
    char mcmd[40] = {0};
    port = (char *)malloc(PORT_MAX_LEN);
    memset(port,0,PORT_MAX_LEN);
    if(argc < 2)
        exit(0);
    sprintf(port,"%s",argv[1]);
    //use chmod 666 port_name to change the port limits of authority.
    sprintf(mcmd,"%s%s","chmod 666 ",argv[1]);
    printf("First,modify the port Limits of authority\n");
    system(mcmd);
    printf("test open scull device port\n");
    int fd = open(port,O_RDWR);
    if(fd < 0)
    {
        printf("open fialed \n");
        exit(0);
    }
    printf("open device success\n");
    close(fd);

    free(port);
    return 0;
}