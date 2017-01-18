#include "soc_cst.h"

void  dialogue_svc() {
  pause();
}
/**

*/
int main (int c, char**v) {
    int s; 
    struct sockaddr_in svc;
    //svc.st
    if(c<3) {
        printf("usage: % ADR_IP Port \n",v[0]);
        exit(-1);
    }
   
    svc.sin_family      =AF_INET;
    svc.sin_addr.s_addr =inet_addr(v[1]);
    svc.sin_port        =htons(atoi(v[2]));
   
    CHECK(s=socket(AF_INET,SOCK_STREAM,0),"clt1");
    CHECK(connect(s,(struct sockaddr*)&svc,sizeof(svc)),"PB Connect");
   
    dialogue_svc();
   
    close (s);
    exit(0);
    //return 0;    
}

