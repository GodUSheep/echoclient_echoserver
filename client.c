#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <errno.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include<string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFSIZE 1<<10
void usage(){
    printf("syntax : echoclient <host> <port>\n");
    printf("sample : echoclient 127.0.0.1 1234\n");
    exit(-1);
}

int main(int argc, char **argv){
    if(argc!=3)usage();
    const int port=atoi(argv[2]);

    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        printf("Socket open error\n");
        exit(-1);
    }
    printf("--------------------Connecting Success!--------------------\n");

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    struct hostent *host=gethostbyname(argv[1]);
    if(host==NULL){
        printf("ERROR : Can't find host\n");
        exit(-1);
    }
    memcpy(&server.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    if(connect(sock,(const struct sockaddr *)&server,sizeof(server))<0){
        printf("ERROR : Can't connect\n");
        exit(-1);
    }

    char buf[BUFSIZE];
    int x=fork();
    if(x==0){
        while(1){
            memset(buf,0,sizeof(buf));
            read(sock,buf,sizeof(buf));
            printf("Server : %s\n",buf);
        }
    }
    else if(x==1){
        while(1){
            fgets(buf,sizeof(buf),stdin);
            if(write(sock,buf,strlen(buf))<0){
                printf("ERROR : Can't write message\n");
                exit(-1);
            }
            printf("Send Message!\n");
        }
    }
    else{
        printf("Error : Can't fork\n");
        exit(-1);
    }

    close(sock);
    return 0;
}