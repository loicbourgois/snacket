#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define CHECK(sts,msg) if((sts)==-1) {perror(msg); exit(-1);} //Eteint le logiciel si erreur, et affiche le message msg
#define ADDR_IP "127.0.0.1"           //Adresse IP du serveur
#define SVC_PORT 3000               //Port du serveur

#define MAX_BUFF_SIZE 1024
#define MAX_PLAYER_COUNT 4
