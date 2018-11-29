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
#include<vector>
#include <arpa/inet.h> 

#define BUFSIZE 1<<10

using namespace std;

void usage(){
    printf("syntax : echoserver <port> [-b]\n");
    printf("sample : echoserver 1234 -b\n");
    exit(-1);
}

int main(int argc, char **argv){
    if(argc!=2&&argc!=3)usage();
    if(argc==3&&memcmp(argv[2],"-b",2)!=0)usage();

    int port=atoi(argv[1]);
    bool _b=false;
    if(argc==3)_b=true;

    int sock=socket(AF_INET, SOCK_STREAM, 0);
    if(sock<0){
        printf("Socket open error\n");
        exit(-1);
    }
    int opt=1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt, sizeof(int));

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    server.sin_addr.s_addr=htonl(INADDR_ANY);

    if (bind(sock, (const sockaddr *)&server, sizeof(server)) < 0){
        printf("ERROR : Can't bind\n");
        exit(-1);
    }

    if (listen(sock, 5) < 0){
        printf("ERROR : Can't listen\n");
        exit(-1);
    }
    
    struct sockaddr_in client;
    socklen_t clientlen = sizeof(client);
    vector<int> socketid;
    vector<bool> ok;
    while(1){
        int which=accept(sock,(sockaddr *)&client,&clientlen);
        if(which<0){
            printf("ERROR : Can't accept\n");
            exit(-1);
        }
        socketid.push_back(which);
        ok.push_back(true);
        int x=fork();
        if(x==0){ /*Do nothing */}
        else if(x==1){   
            struct hostent *host=gethostbyaddr((const char *)&client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr), AF_INET);
            if(host==NULL){
                printf("ERROR : Can't find host\n");
                exit(-1);
            }
            printf("--------------------Connected with %s(%d)--------------------\n",host->h_name,which);
            char buf[BUFSIZE];
            while(1){
                memset(buf,0,sizeof(buf));
                int y=read(which,buf,BUFSIZE);
                printf("%s : %s\n",host->h_name,buf);
                if(_b){
                    for(int i=0;i<socketid.size();i++){
                        if(!ok[i])continue;
                        if(write(socketid[i],buf,strlen(buf))<0){
                            printf("Error : Can't write\n");
                            exit(-1);
                        }
                        printf("Echo to %d\n",socketid[i]);
                    }
                }
                else{
                        if(write(which,buf,strlen(buf))<0){
                            printf("Error : Can't write\n");
                            exit(-1);
                        }
                        printf("Echo to %d\n",which);
                }
            }
            
            for (int i=0;i<socketid.size();i++){
                if (socketid[i]==which){
                    ok[i]=false;
                }
            }
        }
        else{
            printf("Error : Can't fork\n");
            exit(-1);
        }
    }
    close(sock);
    return 0;
}