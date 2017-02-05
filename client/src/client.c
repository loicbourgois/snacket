////////////////////////////////////////////////////////////////////////////////
//  Pour lancer le client : 
//      clear ; make clean ; make ; ./bin/client 127.0.0.1 3000
////////////////////////////////////////////////////////////////////////////////
#include "constants.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <termios.h>
// Couleurs
#define red "\033[22;31m"
#define green "\033[22;32m"
#define yellow "\033[01;33m"
#define blue "\033[22;34m"
#define lightcyan "\033[01;36m"
#define lightred "\033[01;31m"
#define dflt "\033[01;37m"
// 
#define BODY_SYMBOL "00"
#define FOOD_SYMBOL "<3"
////////////////////////////////////////////////////////////////////////////////
// Définition de getch() pour linux.
////////////////////////////////////////////////////////////////////////////////
static struct termios old, new;
void initTermios(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disable buffered i/o */
    new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}
char getch_(int echo) {
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}
char getch(void) {
    return getch_(0);
}
////////////////////////////////////////////////////////////////////////////////
//  Boucle de lecture
////////////////////////////////////////////////////////////////////////////////
void reading (int * mySocket, char * map, int * width, int * height,
        int * tick) {
    char buff[MAX_BUFF_SIZE];
    char * substring;
    while(1) {
        //usleep(1);
        memset(buff,0,MAX_BUFF_SIZE);
        CHECK(read(*mySocket, buff, MAX_BUFF_SIZE), "4");
        substring = strtok(buff, "|");
        if(!strcmp(substring, "map")) {
            memset(map, 0, MAX_BUFF_SIZE);
            strcpy(map, strtok(NULL, "|"));
        } else if(!strcmp(substring, "width")) {
            *width = atoi(strtok(NULL, "|"));
        } else if(!strcmp(substring, "height")) {
            *height = atoi(strtok(NULL, "|"));
        } else if(!strcmp(substring, "tick")) {
            *tick = atoi(strtok(NULL, "|"));
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
//  Helper function pour l'affichage.
//  Permet de récuperer la texture correspondante au char passé en paramètre.
////////////////////////////////////////////////////////////////////////////////
char * getTexture(char c) {
    switch(c) {
        case '*' : {
            return red"██"dflt;
            break;
        }
        case ' ' : {
            return "  ";
            break;
        }
        case '0' : {
            return blue BODY_SYMBOL dflt;
            break;
        }
        case '1' : {
            return lightred BODY_SYMBOL dflt;
            break;
        }
        case '2' : {
            return yellow BODY_SYMBOL dflt;
            break;
        }
        case '3' : {
            return lightcyan BODY_SYMBOL dflt;
            break;
        }
        case 'f' : {
            return green FOOD_SYMBOL dflt;
            break;
        }
        default : {
            // printf("%d\n",c);
            //printf("%d\n",' ');
            return c+"0";
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
// Boucle d'affichage
////////////////////////////////////////////////////////////////////////////////
void displaying (char * map, int * width, int * height, int * tick) {
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
        printf("\n\n--------------------\n\n");
        printf("  tick    %d\n", *tick);
        printf("  width   %d\n", *width);
        printf("  height  %d\n\n", *height);
        printf("%s\n"dflt, mapToDisplay);
    }
}
////////////////////////////////////////////////////////////////////////////////
// Boucle d'écriture.
////////////////////////////////////////////////////////////////////////////////
void writing(int * mySocket) {
    char key;
    char buff[MAX_BUFF_SIZE];
    while(1) {
        usleep(1);
        key = getch();
        switch(key) {
            case 'z': {
                memset(buff, 0, MAX_BUFF_SIZE);
                strcpy(buff, "up");
                write(*mySocket, buff, strlen(buff));
                break;
            }
            case 'd': {
                memset(buff, 0, MAX_BUFF_SIZE);
                strcpy(buff, "right");
                write(*mySocket, buff, strlen(buff));
                break;
            }
            case 's': {
                memset(buff, 0, MAX_BUFF_SIZE);
                strcpy(buff, "down");
                write(*mySocket, buff, strlen(buff));
                break;
            }
            case 'q': {
                memset(buff, 0, MAX_BUFF_SIZE);
                strcpy(buff, "left");
                write(*mySocket, buff, strlen(buff));
                break;
            }
            default : {
                break;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
//  Début du client.
////////////////////////////////////////////////////////////////////////////////
int main (int c, char**v) {
    // Init
    int pid;
    struct sockaddr_in svc;
    // Utilisation de mmap pour partager des variables entre les process.
    char * map;
    map = mmap(NULL, 
        MAX_BUFF_SIZE*sizeof(char),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    int * width;
    width = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    int * height;
    height = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    int * tick;
    tick = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    int * mySocket;
    mySocket = mmap(NULL, 
        sizeof(int),
        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
        -1, 0);
    //
    int ARGUMENT_COUNT = 3;
    if(c != ARGUMENT_COUNT) {
        printf("usage: client ip port\n");
        exit(-1);
    }
    // On se connecte
    svc.sin_family      = AF_INET;
    svc.sin_addr.s_addr = inet_addr(v[1]);
    svc.sin_port        = htons(atoi(v[2]));
    CHECK(*mySocket=socket(AF_INET, SOCK_STREAM, 0),"clt1");
    CHECK(connect(*mySocket,(struct sockaddr*)&svc,sizeof(svc)),"PB Connect");
    // On lance la lecture du socket
    pid=fork();
    if(pid == 0) { 
        reading(mySocket, map, width, height, tick);
        exit(0);
    }
    // On lance l'affichage
    pid=fork();
    if(pid == 0) { 
        displaying(map, width, height, tick);
        exit(0);
    }
    // On lance l'écriture sur le socket
    writing(mySocket);
    pause();
    close (*mySocket);
    exit(0);   
}

