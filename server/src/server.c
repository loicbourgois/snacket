#include "constants.h"
#define MAX_BUFF_SIZE 1024
/*******************************************************************************
*
*******************************************************************************/
void deroute(int sig)
{
    int status;
    switch(sig)
    {
        case SIGCHLD: wait(&status); break;
    }
}
/*******************************************************************************
*
*******************************************************************************/
void dialogue_clt(int socketDialog){
    int nb;
    char buff[MAX_BUFF_SIZE];
    while(1) {
        printf("dial start\n");
    
        //memset(buff,0,MAX_BUFF_SIZE);
        //CHECK(read(socketDialog, buff, MAX_BUFF_SIZE), "4");
        //str2req(buff,&req);
        //traiter(req,&rep);
        
        memset(buff,0,MAX_BUFF);
        //rep2str(rep,&buff);
        //CHECK(write(sd,buff,strlen(buff)+1),"45");
        snprintf(buff, sizeof(buff), "yoloswag");
        write(sd, buff, strlen(buff));
        //write(socketDialog, "erer\n", strlen("erer"));
        //if(FIN) break;  //Requete de fin: arret
        printf("dial end\n");
        pause();
    }
}

/*******************************************************************************
*
*******************************************************************************/
int main(int c, char**v) {
    // init
    int ARGUMENT_COUNT = 3;
    if(c < ARGUMENT_COUNT) {
        printf("usage : server ip port\n");
        exit(-1);
    }
    char * ip = v[1];
    int port = atoi(v[2]);
    int go = 1;
    int se,sd;
    struct sockaddr_in moi, clt;
    int clt_len,pid;
    struct sigaction new;
    new.sa_handler=deroute;
    //new.sa_flags=0;
    new.sa_flags = SA_RESTART;
    char sendBuff[1025];
    //
    CHECK(sigemptyset(&new.sa_mask),"464");
    CHECK(sigaction(SIGCHLD,&new, NULL),"456");
    //
    CHECK(se=socket(AF_INET, SOCK_STREAM,0), "1");
    moi.sin_family      = AF_INET;
    moi.sin_addr.s_addr = inet_addr(ip);
    moi.sin_port        = htons(port);
    memset(&moi.sin_zero, 0, 8);
    
    CHECK(bind(se, (struct sockaddr*)&moi, (socklen_t) sizeof(moi)),"2");
    CHECK(listen(se, 5),"3");
    
    while (go) {
        clt_len=sizeof(clt);
        //on accepte une socket et on la met sur une autre pour garder le canal d'ecoute
        CHECK(sd = accept(se, (struct sockaddr*)&clt, &clt_len), "4ere");          
        printf("New client : %s:%d\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));
        CHECK(pid=fork(), "5");
        
        
         
        
        
        // fils : Processus de service
        if(pid == 0) { 
            printf("child\n");
            close(se);
            dialogue_clt(sd);
            close(sd);
            exit(0);
        }
        // parent 
        else { 
            printf("parent\n");
        }
        close (sd);   
    }
    close(se);
    exit(0);
    return 0;
}

