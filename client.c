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
#include <stdbool.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080  // port to connect to
#define MAX 80  // max chars in a message
#define NAME_SIZE 12  // max size for usernames
#define SA struct sockaddr


// get name (and any other info?) from client on connection
struct definition{
    char username[NAME_SIZE];
    // char role[ROLE_SIZE] = admin;  // or something idk
    // char bio[BIO_SIZE];  // or something idk
};
struct client_args{
    int socket;
    struct definition who_is_this;
};


void* send_to_server(void* args){
    struct client_args* given_args = (struct client_args*) args;
    int socket = (int) given_args -> socket;
    char username = *(char*) given_args -> who_is_this.username;
    
    char buff[MAX];
    
    
    while (1){
        bzero(buff, MAX);
        // printf("You: ");
        
        fgets(buff, MAX, stdin);  // take in message
        // write message unless it's am empty message
        if (buff[0] != '\0'){
            write(socket, buff, strlen(buff));
        }
        else{
            printf("/r");  // go back to the beginning of the line.
        }
        
        // exit if that's what we want
        if (strncmp(buff, "exit", 4) == 0){
            break;
        }
    }
    return NULL;
}

void* receive_from_server(void* arg){
    int socket = *(int*)arg;
    char buff[MAX];
    
    while (1){
        int bytes = read(socket, buff, MAX);
        
        // do nothing if the message is empty
        if (bytes == -1){
            break;
        }
        else{
            printf("%s", buff);
            printf("\n");
        }
        
        fflush(stdout);
        bzero(buff, MAX);
    }
    return NULL;
}

struct definition ask_to_define_self(){
    char name[NAME_SIZE];
    
    printf("Enter username: ");
    scanf("%s", name); 
    
    struct definition who_am_i;
    strcpy(who_am_i.username, name);
    
    return who_am_i;
}



int main(){
    
    // Step 1: Socket(s)
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);  // Use TCP (would be SOCK_DGRAM for UDP)
    
    // check success of socket creation
    if (client_socket == -1){
        printf("Socket not created. Quitting.");
        return -1;
    }
    
    
    // Step 2: Connect to server
    struct sockaddr_in server_address, client_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);
    
    // breaks at this if statement
    if (connect(client_socket, (SA*)&server_address, sizeof(server_address)) != 0){
        printf("connection with the server failed...\n");
        return -1;
    }
    else{
        printf("connected to the server..\n");
        // printf("%d", client_socket);
    }
    
    // get username
    struct definition self_definition = ask_to_define_self();
    // and send it
    printf("username = %s", self_definition.username);
    write(client_socket, self_definition.username, strlen(self_definition.username));
    
    
    // Step 3: Send message to server (to be sent to other clients)
    // Step 4: Receive messages from server (taken from other clients)
    
    // make args
    struct client_args args;
    args.socket = client_socket;
    
    // make 2 threads, one for sending and one for receiving
    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_to_server, &client_socket);
    pthread_create(&recv_thread, NULL, receive_from_server, &client_socket);
    pthread_join(send_thread, NULL);
    pthread_cancel(recv_thread);
    
    // Step 5: Terminate if requested
    close(client_socket);
    
    return EXIT_SUCCESS;
}