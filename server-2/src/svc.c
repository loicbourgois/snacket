#include "soc_cst.h"

void str2Req(char*buff, struct requete * req) {
  
}

void traiter(struct requete req, struct reponse * rep) {
  
}

void rep2str(struct reponse rep, char*buff) {
  
}


void dialogue_clt() {
  //pause();
  int nb;
  int sd;
  char buff[MAX_BUF];
  int go = 1;
  struct requete req;
  struct reponse rep;
  while(go) {
    memset(buff, 0, MAX_BUF);
    CHECK(nb=read(sd, buff, MAX_BUF), "Error");
    str2Req(buff, &req);
    traiter(req, &rep);
    rep2str(rep, buff);
    CHECK(write(sd, buff, strlen(buff)+1), "Error");
  }
}

void deroute (int sig) {
    int status;
  switch(sig) {
    case SIGCHLD: wait(&status);
    break;
  }
} 

int main() {
  // socket d'écoute
  int se;
  struct sockaddr_in moi;
  struct sockaddr_in clt; // client
  int go = 1;
  int sd; //socketDialog
  int clt_len;
  //int pid;
  
  struct sigaction new;
  new.sa_handler = deroute;
  new.sa_flags = 0;
  
  CHECK(sigemptyset(&new.sa_mask), "Error");
  CHECK(sigaction(SIGCHLD, &new, NULL), "Error") ;
  
  // Creer socket d'écoute
  // SOCK_STREAM = tcp
  CHECK(se=socket(AF_INET, SOCK_STREAM, 0), "Error socket creation");
  // Préparation adressage
  moi.sin_family        = AF_INET;
  moi.sin_addr.s_addr   = inet_addr(ADDR_IP);
  moi.sin_port          = htons(SVC_PORT); // host to network short
  memset(&moi.sin_zero, 0, 8);
  // Association socket + addresse
  CHECK(bind(se, (struct sockaddr*)&moi, (socklen_t) sizeof(moi)), "Error bind");
  CHECK(listen(se,5), "Error listen");
  printf("Server launched\n");
  // 
  while(go) {
    clt_len = sizeof(clt);
    CHECK(sd=accept(se, (struct sockaddr*)&clt, (socklen_t *) &clt_len), "Error accept");
    printf("New client : %s:%d\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));
    
   int pid;
    CHECK(pid=fork(), "Error");
    
    if(pid==0) {
      close(se); 
      dialogue_clt();
      close(sd);
      exit(0);
    }
  }
  return 0 ;
}






