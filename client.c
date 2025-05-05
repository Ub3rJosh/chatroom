/* Author: Joshua Maldonado
 * 
 * This file creates clients by for the server (created by server.c)
 * connect to and send messages through.
 * 
 * 
 * Goals:
 *     - allow for some number of clients to connect to one room
 *     - allow for all clients to send and receive chats
 *     - allow for "whispering" among users in the chat, to the tone of:
 *           "whisper client_2 "Hello" 
 *           >>> client_1 whispered to you: "Hello"
*/

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <sys/socket.h>

#define PORT 8080  // port to connect to
#define MAX 80  // max chars in a message
#define SA struct sockaddr

void client_to_server(int socket){
        char buff[MAX];
        int n;
        
        for (;;){
            bzero(buff, sizeof(buff));
            printf("Enter the string : ");
            n = 0;
            while ((buff[n++] = getchar()) != '\n');
            write(socket, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(socket, buff, sizeof(buff));
            printf("From Server : %s", buff);
            if ((strncmp(buff, "exit", 4)) == 0) {
                printf("Client Exit...\n");
                break;
            }
        }
}


int main(){
    // https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Sockets.html#:~:text=The%20socket%20interface%20in%20C,function%20call%20is%20the%20same.
    
    
    // Step 1: Socket(s)
    
    int client_socket;
    client_socket = socket(AF_LOCAL, SOCK_STREAM, 0);  // Use TCP (would be SOCK_DGRAM for UDP)
    
    // check success of sucket creation
    if (client_socket == -1){
        printf("Socket not created. Quiting.");
        return -1;
    }
    
    
    // Step 2: Connect to server
    struct sockaddr_in server_address, client_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);
    
    if (connect(client_socket, (SA*)&server_address, sizeof(server_address)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    
    // Step 3: Send message to server (to be sent to other clients)
    // Step 4: Receive messages from server (taken from other clients)
    client_to_server(client_socket);
    
    
    
    // Step 5: Terminate if requested
    close(client_socket);
    
    return 0;
}