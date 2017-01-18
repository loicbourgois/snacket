#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <arpa/inet.h>

#define CHECK(sts,msg) if ((sts)==-1) {perror(msg);exit(-1);}
  
#define ADDR_IP "0.0.0.0"
// 0 : 
// 1 -> 123
// reserver port 3000 dans /etc/services
#define SVC_PORT 3000 // cf /etc/services

#define MAX_BUF 100

struct requete {
  int a;
};
struct reponse {
  int a;
};

/*traiter() {
  
}*/

// req2str;
// str2req;
// rep2str;
// str2rep;
