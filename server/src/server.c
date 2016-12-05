/*
    Le serveur / Seance 2
*/
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
int main(int argc , char *argv[]) {
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
	int PORT = 8888;
    
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Socket non creee \n");
    }else{
        printf("Socket creee \n");
    }     
    //Preparation de sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );// port d ecoute
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //Afficher un message d erreur
        printf("bind echoué. Erreur \n");
        return 1;
    }else{
        printf("bind reussi \n");
    }
    
    //Ecoute sur le port 8888
    listen(socket_desc , 3);
    
    //Accept and incoming connection
    printf("Attente d arrivee des connexions... \n");
    c = sizeof(struct sockaddr_in);
    
    //Acception de la connexion arrivant d un client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        printf("acception echouee \n");
        return 1;
    }else{
        printf("acceptation reussite \n");
    }
    //Reception du message d un client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        printf("Message arrive au serveur, le message est [%s] \n", client_message );

		int x = strlen(client_message); // You want to get the length of the whole string.
		int i = 0;
		for (i=0 ; i<x ; i++){
			client_message[i]= toupper(client_message[i]);
		}		

		printf("Le message à renvoyer : [%s] \n", client_message );

		if( send(client_sock , client_message , strlen(client_message) , 0) < 0)
        {
            printf("ko - Message non envoye \n");
            return -1;
        }else{
            printf("ok - Message envoye");
		}
    }
    printf("\n");
    return 0;
}
*/
