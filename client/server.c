#include "soc_cst.h"
#define MAX_BUFF 8 


void deroute(int sig)
{
    int status;
    switch(sig)
    {
        case SIGCHLD: wait(&status); break;
    }
}

void dialogue_clt(int sd){
    
    pause();
    int nb;
    char buff[MAX_BUFF];
    /*while(1)
    {
        memset(buff,0,MAX_BUFF);
        CHECK(read(sd,buff,MAX_BUFF),"4");
        //str2req(buff,&req);
        //traiter(req,&rep);
        
        memset(buff,0,MAX_BUFF);
        //rep2str(rep,&buff);
        CHECK(write(sd,buff,strlen(buff)+1),"45");
        //if(FIN) break;  //Requete de fin: arret
    }*/
    
    
    
    
}
int main()
{
    /**********VARIABLES***********/
    int se,sd;
    struct sockaddr_in moi, clt;
    int clt_len,pid;
    /******************************/
    
    struct sigaction new;
    new.sa_handler=deroute;
    new.sa_flags=0;
    
    CHECK(sigemptyset(&new.sa_mask),"464");
    CHECK(sigaction(SIGCHLD,&new, NULL),"456");
    
    
    
    CHECK(se=socket(AF_INET,SOCK_STREAM,0),"1");
    moi.sin_family      = AF_INET;
    moi.sin_addr.s_addr = inet_addr(ADDR_IP);
    moi.sin_port        = htons(SVC_PORT);
    memset(&moi.sin_zero, 0, 8);
    
    CHECK(bind(se,(struct sockaddr*)&moi,(socklen_t) sizeof(moi)),"2");
    CHECK(listen(se, 5),"3");
    
    while (1)
    {
        clt_len=sizeof(clt);
        CHECK(sd=accept(se,(struct sockaddr*)&clt, &clt_len), "4ere");          //on accepte une socket et on la met sur une autre pour garder le canal d'ecoute
        printf("New client : %s:%d\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));
        CHECK(pid=fork(), "5");
        if(pid==0) //FILS: Processus de service
        {
            close(se);
            dialogue_clt(sd);
            close(sd);
            exit(0);
        }
        close (sd);
        
             
    }
    close(se);
    exit(0);
    
    return 0;
}







/*
void traiter(... requete
    
    switch(requete...*/
