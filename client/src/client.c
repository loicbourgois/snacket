// clear ; make clean ; make ; ./bin/client 127.0.0.1 3000

#include "constants.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

void reading (int socket, char * map, int * width, int * height,
        int * tick) {
    char buff[MAX_BUFF_SIZE];
    char * substring;
    
    while(1) {
        //usleep(1*1000);
        memset(buff,0,MAX_BUFF_SIZE);
        CHECK(read(socket, buff, MAX_BUFF_SIZE), "4");
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
        default : {
            printf("%d\n",c);
            printf("%d\n",' ');
            return "??";
            break;
        }
    }
}

void displaying (char * map, int * width, int * height, int * tick) {
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


int main (int c, char**v) {
    // init
    int socketPulling; 
    char character;
    int pid;
    struct sockaddr_in svc;
    // mmap c'est cool : on peut partager la variable entre les process
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
    //
    int ARGUMENT_COUNT = 4;
    if(c < ARGUMENT_COUNT) {
        printf("usage: client ip port character");
        exit(-1);
    }
    //
    character = v[3][0];
    printf("Yo, vous êtes %c\n", character);
    // Connection
    svc.sin_family      = AF_INET;
    svc.sin_addr.s_addr = inet_addr(v[1]);
    svc.sin_port        = htons(atoi(v[2]));
    CHECK(socketPulling=socket(AF_INET, SOCK_STREAM, 0),"clt1");
    CHECK(connect(socketPulling,(struct sockaddr*)&svc,sizeof(svc)),"PB Connect");
    // launch reading
    pid=fork();
    if(pid == 0) { 
        reading(socketPulling, map, width, height, tick);
        exit(0);
    }
    // launch displaying
    pid=fork();
    if(pid == 0) { 
        displaying(map, width, height, tick);
        exit(0);
    }
    //
    pause();
    close (socketPulling);
    exit(0);   
}

