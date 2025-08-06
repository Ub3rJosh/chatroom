/* Author: Joshua Maldonado
 * 
 * This file creates the server by which clients (created by client.c)
 * may connect and send messages through.
 * 
 * 
 * Goals:
 *     - allow for some number of clients to connect to one room
 *     - allow for all clients to send and receive chats
 *     - allow for "whispering" among users in the chat, to the tone of:
 *           "whisper client_2 "Hello" 
 *           >>> client_1 whispered to you: "Hello"
 * 
 * Potential extensions:
 *     - Multithreading (one thread per client?)
 *     - Filter messages for a list of words (i.e., don't allow message about tomatoes to be sent)
 *     - "Breakout room" functionality ("client_1 invited you to a breakout room. Accept? (y/n): ")
*/

#include <stdio.h>
#include <stdbool.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <pthread.h>

#include "chat_specs.h"

#define SA struct sockaddr
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


// connections and their management
struct connection{
    bool still_connected;      // keep track whether or not the client is still active
    int connection_number;     // keep track of what order people joined (probably unnecessary)
    int socket;                // the socket that will be used to send/receive on
    char username[NAME_SIZE];  // user-defined username
};

// keep track of connections and clients through `client_connection_array` and `connected_clients`
struct connection client_connection_array[MAX_CLIENTS];
int connected_clients = 0;
void store_connection(int input_socket, 
                      int socket_number, 
                      struct connection (*socket_list)[MAX_CLIENTS]){
    (*socket_list)[socket_number].still_connected = true;
    (*socket_list)[socket_number].connection_number = socket_number;
    (*socket_list)[socket_number].socket = input_socket;
}

void break_connection(int input_socket, 
                      int socket_number, 
                      struct connection (*socket_list)[MAX_CLIENTS]){
    // clean up struct
    (*socket_list)[socket_number].still_connected = false;
    (*socket_list)[socket_number].connection_number = -1;
    (*socket_list)[socket_number].socket = -1;
    bzero((*socket_list)[socket_number].username, NAME_SIZE);
}

// define struct for thread args
struct thread_args{
    int socket;
    int index;
};


// takes in the message that may have a command in it
// returns the int that corresponds to various commands
int parse_command(char *given_string){
    /* 
        0 <-> whisper
        1 <-> disconnect / exit
        2 <-> TBD
        ...
       -1 <-> fail
    */
   
    // if there is no command given then return fail case
    if (given_string[0] != '@'){
        return -1;
    }
    
    // parse argument
    int end_index = 0;
    for (int i = 0; i < strlen(given_string); i++){
        if (given_string[i] == ' '){
            end_index = i;
            break;
        }
    }
    char command[COMMAND_SIZE];
    strncpy(command, given_string, end_index);
    
    printf("input command: %s", command);
    printf("\n");
    
    // find the int associated with each commant
    if (strcmp(command, "@exit") == 0){
        return 0;
    }
    else if (strcmp(command, "@whisper") == 0){
        return 1;
    }
    // else if (strcmp(command, "@...") == 0){...}
    else{
        printf("command unknown: %s", command);
        return -1;  // fail case 
    }
}


// takes in the message that may have a command in it
// returns the socket number of the person being whispered to
int parse_whisper(char *given_string){
    // parse argument and grab name
    bool finding_first = true;
    int start_index = 0;
    int end_index = 0;
    
    for (int i = 0; i < strlen(given_string); i++){
        if (given_string[i] == ' '){
            // find first space (after @whisper)
            if (finding_first){
                start_index = i;
                finding_first = false;
            }
            // find second space (after username of @whisper)
            else{
                end_index = i;
                break;
            }
        }
    }
    
    // name is from (start_index + 1) to (end_index - 1)
    // now grab it
    char whisper_username[end_index - start_index];
    strncpy(whisper_username, given_string + start_index, end_index);
    printf("Username being whispered to: %s", whisper_username);
    
    // find socket of username in `client_connection_array`
    for (int i = 0; i < connected_clients; i++){
        if (client_connection_array[i].username == whisper_username){
            return client_connection_array[i].socket;  // return socket of whispered-to client
        }
    }
    return -1;  // fail case (username couldn't be found)
}


// the "chat" part of the chatroom
void* server_to_client(void* args){
    // handle args
    struct thread_args* given_args = (struct thread_args*) args;
    int socket = given_args -> socket;
    int socket_number = given_args -> index;
    
    int command_int;  // for keeping track of commands
    char buff[MAX];   // define message variable
    
    // read in username
    char username[NAME_SIZE];
    read(socket, &username, sizeof(username));
    strcpy(client_connection_array[socket_number].username, username);
    printf("Client %d's username is %s\n", socket_number, username);
    
    
    // Loop for actually running the chat
    for (;;){ 
        bzero(buff, MAX);
        
        // read the message from client and copy it in buffer 
        int bytes_read = read(socket, buff, sizeof(buff));
        if (bytes_read <= 0) {
            printf("Client disconnected or error occurred. Closing connection.");
            break_connection(socket, socket_number, &client_connection_array);
            close(socket);
            break;
        }
        
        
        int command_int = parse_command(buff);  // check for command being given
        
        // start if chain
        if (command_int == 0){  // exit command hit
            printf("Server Exit..."); 
            
            // send message stating disconnect
            for (int i = 0; i < connected_clients; i++){
                pthread_mutex_lock(&lock);  // keep the threads behaving
                
                int socket_i = client_connection_array[i].socket;
                char exit_message[NAME_SIZE + 6];
                snprintf(exit_message, sizeof(exit_message), "%s left.\n", client_connection_array[socket_number].username);
                printf("%s", exit_message);
                write(socket_i, exit_message, sizeof(exit_message));
                
                pthread_mutex_unlock(&lock);  // free thread
            }
            break; 
        }
        else if (command_int == 1){  // whisper command hit
            int whisper_socket = parse_whisper(buff);
            printf("From client %d (whispered to %d) (%s): %s", socket_number, whisper_socket, client_connection_array[whisper_socket].username, buff);
            
            pthread_mutex_lock(&lock);  // keep the threads behaving
            
            // send the message to all threads that did not send message
            char message[25 + MAX + NAME_SIZE];
            snprintf(message, sizeof(message), "%s (whispered): %s", client_connection_array[whisper_socket].username, buff);
            write(whisper_socket,  message, sizeof(message));
            
            pthread_mutex_unlock(&lock);  // free thread
        }
        // else if (command_int == ...){...}
        else{  // no command found 
            // if not whispering (or otherwise using a command)
            // print buffer which contains the client contents 
            printf("From client %d (%s): %s", socket_number, client_connection_array[socket_number].username, buff); 
            
            // and send that buffer to client 
            // loop through clients and send message to all connected clients (that didn't send the message)
            for (int i = 0; i < connected_clients; i++){
                pthread_mutex_lock(&lock);  // keep the threads behaving
                
                // send the message to all threads that did not send message
                char message[14 + MAX + NAME_SIZE];
                int socket_i = client_connection_array[i].socket;
                if (client_connection_array[i].still_connected && (socket != socket_i)){
                    snprintf(message, sizeof(message), "%s: %s", 
                            client_connection_array[socket_number].username, buff);
                    write(socket_i,  message, sizeof(message));
                }
                
                pthread_mutex_unlock(&lock);  // free thread
            }
        }
    }  // end for
        
        
    free(given_args);  // malloc'd in else statement of server_running while loop in main
    return NULL;
}



int main(){
    // Step 1: Socket(s) (and use Setsockopt?)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    
    // Step 2: Bind
    struct sockaddr_in server_address; 
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_address.sin_port = htons(PORT); 
    
    int bound_socket = bind(sockfd, (SA*)&server_address, sizeof(server_address));;
    // check success of binding
    if (bound_socket != 0){
        perror("Binding unsuccessful.");
    }
    
    
    // Step 3: Listen
    // printf("Server is listening for new clients.");
    int listen_to_socket = listen(sockfd, 5);
    
    
    bool server_running = true;  // keep the server looping to keep accepting new clients
    while (server_running){
        
        // Step 4: Accept new clients (if possible)
        struct sockaddr_in client; 
        socklen_t len = sizeof(client);
        int incoming_connection = accept(sockfd, (SA*)&client, &len);
        pthread_mutex_lock(&lock);
        
        // see if there's something bad happening with the sockets
        if (incoming_connection < 0){
            perror("Server accept failed..."); 
        }
        // see if the server is at it's max number of clients already
        else if (connected_clients >= MAX_CLIENTS){
            perror("Maximum number of clients already met.");
            close(incoming_connection);
        }
        // if there's no reason not to, accept the client
        else{
            struct thread_args* args = malloc(sizeof(struct thread_args));  // to be freed in server_to_client()
            args -> socket = incoming_connection;
            args -> index = connected_clients;
            store_connection(incoming_connection, connected_clients, &client_connection_array);
            
            connected_clients++;
            printf("Server accepted client.\n");
            pthread_mutex_unlock(&lock);
            
            pthread_t tid;
            pthread_create(&tid, NULL, (void*)server_to_client, (void*)args);
        }
    }
    
    return EXIT_SUCCESS;
}
