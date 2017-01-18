/*******************************************************************************
* Pour lancer le programme
* clear ; make clean ; make ; ./bin/server 127.0.0.1 3000
*******************************************************************************/
#include "constants.h"
#include <time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
/*******************************************************************************
* Structures
*******************************************************************************/
struct Point {
   char x;
   char y;
} Point;
struct Player {
   char id;
   char direction;
   char x;
   char y;
   char size;
   char alive;
   struct Point bodyParts[MAX_BUFF_SIZE]; 
} Player;  
struct Food {
   char id;
   char x;
   char y;
} Food;
/*******************************************************************************
* Heu.. ça sert à quoi ? 
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
* Logique pour les joueurs.
*******************************************************************************/
void resetPlayer(struct Player * players, int id) {
    int i = 0;
    players[id].id = -1;
    players[id].direction = -1;
    players[id].x = -1;
    players[id].y = -1;
    players[id].size = 0;
    players[id].alive = 0;
    for(i = 0 ; i < MAX_BUFF_SIZE ; i++) {
        players[id].bodyParts[i].x = -1;
        players[id].bodyParts[i].y = -1;
    }
}
int addPlayer(struct Player * players){
    int i ;
    for(i = 0 ; i<MAX_PLAYER_COUNT ; i++) {
        if(players[i].id == -1) {
            players[i].id = i;
            players[i].x = 5;
            players[i].y = 5;
            players[i].direction = 1;
            players[i].alive = 1;
            players[i].size = MIN_PLAYER_SIZE;
            return i;
            break;
        }
    }
    return -1;
}
/*******************************************************************************
* Logique du jeux
*******************************************************************************/
void repopFood(struct Food * food, char * baseMap, struct Player * players,
        int width, int height) {
    int cellId = 0;
    while(1) {
        food[0].x = rand() % width;
        food[0].y = rand() % height;
        cellId = food[0].y * width + food[0].x;
        if(baseMap[cellId] == ' ') {
            break;
        }
    }
    
}
void movePlayer(struct Player * players, int id, int * width, int * height) {
    if(!players[id].alive) {
        return;
    }

    int i = 0;
    for(i = players[id].size ; i > 0 ; i--) {
        players[id].bodyParts[i].x = players[id].bodyParts[i-1].x;
        players[id].bodyParts[i].y = players[id].bodyParts[i-1].y;
    }
    switch(players[id].direction) {
        case 0 : 
            players[id].y = players[id].y - 1;
            break; 
        case 1 : 
            players[id].x = players[id].x + 1;
            break; 
        case 2 : 
            players[id].y = players[id].y + 1;
            break; 
        case 3 :
            players[id].x = players[id].x - 1;
            break;
    }
    players[id].x = (players[id].x + (*width))%(*width);
    players[id].y = (players[id].y + (*width))%(*width);
    players[id].bodyParts[0].x = players[id].x;
    players[id].bodyParts[0].y = players[id].y;
}
void addPlayerToMap(struct Player * players, int playerId, int * width, int * height, char * map) {
    int i = 0;
    int cellId = 0;
    int x = 0;
    int y = 0;
    for(i = 0 ; i < players[playerId].size ; i++) {
        x = players[playerId].bodyParts[i].x;
        y = players[playerId].bodyParts[i].y;
        cellId = y * ((*width)) + x;
        map[cellId] = players[playerId].id+'0';
    }
}
void checkColisions(char * baseMap, struct Player * players, int playerId, 
        int * width, int * height, struct Food * food) {
    int i = 0;
    int j = 0;
    int cellId = 0;
    if(players[playerId].size > MIN_PLAYER_SIZE) {
        // Walls
        cellId = players[playerId].y * ((*width)) + players[playerId].x;
        if(baseMap[cellId] != ' ') {
            players[playerId].alive = 0;
            return;
        }
        // Snakes
        for(i=0 ; i < MAX_PLAYER_COUNT ; i++) {
            if(i==playerId) {
                continue;
            }
            for(j=0 ; j < players[i].size ; j++) {
                if(players[i].bodyParts[j].x == players[playerId].x
                        && players[i].bodyParts[j].y == players[playerId].y ) {
                    players[playerId].alive = 0;
                    return;
                }
            }
        }
    }
    // Food
    if(players[playerId].x == food[0].x
            && players[playerId].y == food[0].y ) {
        players[playerId].size += 1;
        repopFood(food, baseMap, players, *width, *height);
    }
}
/*******************************************************************************
* Boucle qui gère la logique du jeux.
* Il n'y en a qu'une.
*******************************************************************************/
void run(char * baseMap, char * map, int * width, int * height, int * tick, struct Player * players, struct Food * food){
    int i = 0;
    while(1) {
        usleep(SLEEP_RUN*1000);
        // Move
        for(i = 0 ; i < MAX_PLAYER_COUNT ; i++) {
            if(players[i].id == -1) {continue;}
            movePlayer(players, i, width, height);
        }
        // Check colisions
        for(i = 0 ; i < MAX_PLAYER_COUNT ; i++) {
            if(players[i].id == -1) {continue;}
            checkColisions(baseMap, players, i, width, height, food);
        }
        // Update map
        memset(map, 0, MAX_BUFF_SIZE);
        strcpy(map, baseMap);
        for(i = 0 ; i < MAX_PLAYER_COUNT ; i++) {
            if(players[i].id == -1) {continue;}
            addPlayerToMap(players, i, width, height, map);
        }
        int cellId = food[0].y * ((*width)) + food[0].x;
        map[cellId] = 'f';
        //
        *tick = (*tick)+1;
    }
}
/*******************************************************************************
* 
*******************************************************************************/
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
        case '0' : {
            return "00";
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
        case 'f' : {
            return "<3";
            break;
        }
        default : {
            return "??";
            break;
        }
    }
}
/*******************************************************************************
* Boucle d'affichage
* Il y a seulement une boucle d'affichage qui tourne.
*******************************************************************************/
void displaying (char * map, int * width, int * height, int * tick, struct Player * players) {
    char mapToDisplay[MAX_BUFF_SIZE];
    int i;
    while(1) {
        usleep(SLEEP_RUN/4*1000);
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
* Boucle d'écriture, pour envoyer les données régulièrement aux clients
* Au total on a N boucles d'écriture. 
* N = nombre de clients connectés
*******************************************************************************/
void writing (int socket, char * map, int * tick, int playerId){
    char buff[MAX_BUFF_SIZE];
    char tickStr[15];
    while(1) {
        usleep(SLEEP_RUN/8*1000);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "width|10|");
        write(socket, buff, strlen(buff));
        //
        usleep(SLEEP_RUN/8*1000);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "height|10|");
        write(socket, buff, strlen(buff));
        //
        usleep(SLEEP_RUN/8*1000);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "tick|");
        sprintf(tickStr, "%d", *tick);
        strcat(buff, tickStr);
        strcat(buff, "|");
        write(socket, buff, strlen(buff));
        //
        usleep(SLEEP_RUN/8*1000);
        memset(buff, 0, MAX_BUFF_SIZE);
        strcpy(buff, "map|");
        strcat(buff, map);
        strcat(buff, "|");
        write(socket, buff, strlen(buff));
    }
}
/*******************************************************************************
* Boucle de lecture pour les sockets
* Au total on a N boucles de lectures. 
* N = nombre de clients connectés
*******************************************************************************/
void reading(int otherSocket, int playerId, struct Player * players) {
    char buff[MAX_BUFF_SIZE];
    while(1) {
        usleep(1);
        memset(buff,0,MAX_BUFF_SIZE);
        CHECK(read(otherSocket, buff, MAX_BUFF_SIZE), "4");
        if(!strcmp("",buff)) {
            printf("Player #%d disconnected\n", playerId);
            resetPlayer(players, playerId);
            break;
        } else if (!strcmp("up",buff)) {
            players[playerId].direction = 0;
        } else if (!strcmp("right",buff)) {
            players[playerId].direction = 1;
        } else if (!strcmp("down",buff)) {
            players[playerId].direction = 2;
        } else if (!strcmp("left",buff)) {
            players[playerId].direction = 3;
        }
    }
}
/*******************************************************************************
* Début du programme
*******************************************************************************/
int main(int c, char**v) {
    // Inits
    srand(time(NULL));
    int ARGUMENT_COUNT = 3;
    if(c < ARGUMENT_COUNT) {
        printf("usage : server ip port\n");
        exit(-1);
    }
    char * ip = v[1];                                   
    int port = atoi(v[2]);                              
    int pid = 0;                                    
    int masterSocket; // Permet d'écouter et créer un nouveau socket si on a une nouvelle connection
    int otherSocket; // Le nouveau socket, pour libérer le socket d'écoute
    struct sockaddr_in moi;
    struct sockaddr_in clt; 
    int clt_len; // ??
    struct sigaction new; // ??
    int i; 
    int go = 1;
    // Init des variables partagées entre process, grâce à mmap.
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
        resetPlayer(players, i);
    }
    struct Food * food;
    food = mmap(NULL, 
        sizeof(Food),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    food[0].x = 9;
    food[0].y = 5;
    repopFood(food, baseMap, players, *width, *height);
    
    
    // Des trucs
    new.sa_handler=deroute;
    new.sa_flags = SA_RESTART;
    CHECK(sigemptyset(&new.sa_mask),
        "erreur ");
    CHECK(sigaction(SIGCHLD,&new, NULL),
        "erreur ");
    // Des trucs 
    CHECK(masterSocket = socket(AF_INET, SOCK_STREAM,0),
        "problem");
    moi.sin_family      = AF_INET;
    moi.sin_addr.s_addr = inet_addr(ip);
    moi.sin_port        = htons(port);
    memset(&moi.sin_zero, 0, 8);
    CHECK(bind(masterSocket, (struct sockaddr*)&moi, (socklen_t) sizeof(moi)),
        "problem bind");
    CHECK(listen(masterSocket, 5),
        "problem listen");
    // On lance la logique du serveur
    pid=fork();
    if(pid == 0) { 
        run(baseMap, map, width, height, tick, players, food);
        exit(0);
    }
    // On lance l'affichage
    pid=fork();
    if(pid == 0) { 
        displaying(map, width, height, tick, players);
        exit(0);
    }
    // On boucle pour attendre les nouvelles connections
    while (go) {
        //on accepte une socket et on la met sur une autre pour garder le canal d'ecoute
        clt_len=sizeof(clt);
        CHECK(otherSocket = accept(masterSocket, (struct sockaddr*)&clt, (socklen_t *)&clt_len),
            "problem");      
        int playerId = addPlayer(players);    
        printf("Player #%d joined : %s:%d\n", playerId, inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));
        // On lance l'écriture sur le nouveau socket
        CHECK(pid=fork(), 
            "fork error");
        if(pid == 0) { 
            close(masterSocket);
            writing(otherSocket, map, tick, playerId);
            close(otherSocket);
            exit(0);
        }
        // On lance la lecture sur le nouveau socket
        CHECK(pid=fork(), 
            "fork error");
        if(pid == 0) { 
            close(masterSocket);
            reading(otherSocket, playerId, players);
            close(otherSocket);
            exit(0);
        }
        //
        close (otherSocket);   
    }
    close(masterSocket);
    exit(0);
    return 0;
}

