// clear ; make clean ; make ; ./bin/server 127.0.0.1 3000
#include "constants.h"
#include <time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
/*******************************************************************************
*
*******************************************************************************/
struct Player {
   char id;
   char direction;
   char x;
   char y;
} Player;  


/*******************************************************************************
*
*******************************************************************************/
void deroute(int sig) {
    int status;
    switch(sig) {
        case SIGCHLD: { 
            wait(&status); 
            break;
        }
        default : {
            break;
        }
    }
}
/*******************************************************************************
*
*******************************************************************************/
int addPlayer(struct Player * players){
    int i ;
    for(i = 0 ; i<MAX_PLAYER_COUNT ; i++) {
        if(players[i].id == -1) {
            players[i].id = i;
            players[i].x = 5;
            players[i].y = 5;
            players[i].direction = 1;
            printf("New player : #%d\n", players[i].id);
            return i;
            break;
        }
    }
    return -1;
}
/*******************************************************************************
*
*******************************************************************************/
void run(char * baseMap, char * map, int * width, int * height, int * tick, struct Player * players){
    while(1) {
        int i = 0;
        memset(map, 0, MAX_BUFF_SIZE);
        strcpy(map, baseMap);
        for(i = 0 ; i < MAX_PLAYER_COUNT ; i++) {
            if(players[i].id == -1) {
                continue;
            }
            // Move
            switch(players[i].direction) {
                case 0 : 
                    players[i].y = players[i].y - 1;
                    break; 
                case 1 : 
                    players[i].x = players[i].x + 1;
                    break; 
                case 2 : 
                    players[i].y = players[i].y + 1;
                    break; 
                case 3 : 
                    players[i].x = players[i].x - 1;
                    break; 
            }
            players[i].x = (players[i].x + (*width))%(*width);
            players[i].y = (players[i].y + (*width))%(*width);
            
            int id = players[i].y * ((*width)) + players[i].x;
            map[id] = players[i].id+'0';
        }
        usleep(500*1000);
        *tick = (*tick)+1;
    }
}
/*******************************************************************************
*
*******************************************************************************/
void writing (int socket, char * map, int * tick, int playerId){
    char buff[MAX_BUFF_SIZE];
    char tickStr[15];
    while(1) {
        usleep(100*1000);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "width|10|");
        write(socket, buff, strlen(buff));
        //
        usleep(1);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "height|10|");
        write(socket, buff, strlen(buff));
        //
        usleep(1);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "tick|");
        sprintf(tickStr, "%d", *tick);
        strcat(buff, tickStr);
        strcat(buff, "|");
        write(socket, buff, strlen(buff));
        //
        usleep(1);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "map|");
        strcat(buff, map);
        strcat(buff, "|");
        write(socket, buff, strlen(buff));
    }
}
char * getTexture(char c) {
    switch(c) {
        case '*' : {
            return "██";
            break;
        }
        case ' ' : {
            return "  ";
            break;
        }
        case '1' : {
            return "11";
            break;
        }
        case '2' : {
            return "22";
            break;
        }
        case '3' : {
            return "33";
            break;
        }
        case '0' : {
            return "00";
            break;
        }
        default : {
            return "??";
            break;
        }
    }
}
void displaying (char * map, int * width, int * height, int * tick, struct Player * players) {
    char mapToDisplay[MAX_BUFF_SIZE];
    
    int i;
    while(1) {
        usleep(100*1000);
        //
        memset(mapToDisplay, 0, MAX_BUFF_SIZE);
        for(i=0 ; i < (*width)*(*height) ; i++) {
            if(i && !(i%(*width))) {
                strcat(mapToDisplay, "\n");
            }
            strcat(mapToDisplay, getTexture(map[i]));
        }
        
        
        
        
        //
        printf("\n-----\n\n");
        printf("tick    %d\n", *tick);
        printf("width   %d\n", *width);
        printf("height  %d\n", *height);
        printf("%s\n", mapToDisplay);
    }
}

/*******************************************************************************
*
*******************************************************************************/
int main(int c, char**v) {
    int i;
    //
    char * map;
    map = mmap(NULL, 
        sizeof(char) * MAX_BUFF_SIZE,
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    char * baseMap;
    baseMap = mmap(NULL, 
        sizeof(char) * MAX_BUFF_SIZE,
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    strcat(baseMap, "\
**********\
*        *\
          \
          \
          \
          \
          \
          \
*        *\
**********");
    int * tick;
    tick = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    tick[0] = 0;
    int * width;
    width = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    *width = 10;
    int * height;
    height = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    *height = 10;
    struct Player * players;
    players = mmap(NULL, 
        sizeof(Player) * MAX_PLAYER_COUNT,
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    players[0].id = -1;
    for(i = 0 ; i<MAX_PLAYER_COUNT ; i++) {
        players[i].id = -1;
        players[i].direction = -1;
        players[i].x = -1;
        players[i].y = -1;
    }
    
    // init
    int ARGUMENT_COUNT = 3;
    if(c < ARGUMENT_COUNT) {
        printf("usage : server ip port\n");
        exit(-1);
    }
    char * ip = v[1];
    int port = atoi(v[2]);
    int go = 1;
    int pid = 0;
    int masterSocket, otherSocket;
    struct sockaddr_in moi, clt;
    int clt_len;
    struct sigaction new;
    new.sa_handler=deroute;
    //new.sa_flags=0;
    new.sa_flags = SA_RESTART;
    //char sendBuff[1025];
    //
    CHECK(sigemptyset(&new.sa_mask),"464");
    CHECK(sigaction(SIGCHLD,&new, NULL),"456");
    //
    CHECK(masterSocket=socket(AF_INET, SOCK_STREAM,0), "1");
    moi.sin_family      = AF_INET;
    moi.sin_addr.s_addr = inet_addr(ip);
    moi.sin_port        = htons(port);
    memset(&moi.sin_zero, 0, 8);
    
    CHECK(bind(masterSocket, (struct sockaddr*)&moi, (socklen_t) sizeof(moi)),"2");
    CHECK(listen(masterSocket, 5),"3");
    
    pid=fork();
    if(pid == 0) { 
        run(baseMap, map, width, height, tick, players);
        exit(0);
    }
    pid=fork();
    if(pid == 0) { 
        displaying(map, width, height, tick, players);
        exit(0);
    }
    
    addPlayer(players);
    
    
    while (go) {
        clt_len=sizeof(clt);
        //on accepte une socket et on la met sur une autre pour garder le canal d'ecoute
        CHECK(otherSocket = accept(masterSocket, (struct sockaddr*)&clt, (socklen_t *)&clt_len), "4ere");          
        printf("New client : %s:%d\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));
        CHECK(pid=fork(), "5");
        // fils : Processus de service
        if(pid == 0) { 
            int playerId = addPlayer(players);
            close(masterSocket);
            writing(otherSocket, map, tick, playerId);
            close(otherSocket);
            exit(0);
        }
        close (otherSocket);   
    }
    close(masterSocket);
    exit(0);
    return 0;
}

