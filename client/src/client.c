#include "constants.h"

int main (int c, char**v) {
    // init
    int s; struct sockaddr_in svc;
    int n = 0;
    char recvBuff[1024];
    int ARGUMENT_COUNT = 3;
    if(c < ARGUMENT_COUNT) {
        printf("usage:%s ADR_IP:%s Port:%s \n",v[0],v[1],v[2]);
        exit(-1);
    }
    //
    svc.sin_family      =AF_INET;
    svc.sin_addr.s_addr =inet_addr(v[1]);
    svc.sin_port        =htons(atoi(v[2]));
    CHECK(s=socket(AF_INET,SOCK_STREAM,0),"clt1");
    CHECK(connect(s,(struct sockaddr*)&svc,sizeof(svc)),"PB Connect");
    //
    int i = 0;
    while ( (n = read(s, recvBuff, sizeof(recvBuff)-1)) > 0) {
        printf("%d\n", i++);
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error\n");
        }
    }
    close (s);
    exit(0);   
}

