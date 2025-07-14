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
#include <unistd.h> // read(), write(), close()
#include <pthread.h>

#define MAX_CLIENTS 4  // max number of connections to the server at a time
#define PORT 8080  // port to connect to
#define MAX 80  // max message size
#define NAME_SIZE 12  // max size for usernames
#define SA struct sockaddr
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


// connecitions and their managment
struct connection{
    bool still_connected;      // keep track whether or not the client is still active
    int connection_number;     // keep track of what order people joined (probably unnecessary)
    int socket;                // the socket that will be used to send/recieve on
    char username[NAME_SIZE];  // user-defined username
};

// keep track of connections and clients through `client_connection_array` and `connected_clients`
struct connection client_connection_array[MAX_CLIENTS];
int connected_clients = 0;
void store_connection(int input_socket, int socket_number, struct connection (*socket_list)[MAX_CLIENTS]){
    (*socket_list)[socket_number].still_connected = true;
    (*socket_list)[socket_number].connection_number = socket_number;
    (*socket_list)[socket_number].socket = input_socket;
}
void break_connection(int input_socket, int socket_number, struct connection (*socket_list)[MAX_CLIENTS]){
    (*socket_list)[socket_number].still_connected = false;
    (*socket_list)[socket_number].connection_number = -1;
    (*socket_list)[socket_number].socket = -1;
}

struct thread_args{
    int socket;
    int index;
};



// the "chat" part of the chatroom
void* server_to_client(void* args){
    struct thread_args* given_args = (struct thread_args*) args;
    int socket = given_args -> socket;
    int socket_number = given_args -> index;
    
    char buff[MAX]; 
    int n;
    
    
    for (;;){ 
        // bzero(buff, MAX);
        
        // read the message from client and copy it in buffer 
        int bytes_read = read(socket, buff, sizeof(buff));
        if (bytes_read <= 0) {
            printf("Client disconnected or error occurred. Closing connection.\n");
            break_connection(socket, socket_number, &client_connection_array);
            close(socket);
            break;
        }
        
        // print buffer which contains the client contents 
        printf("From client: %d: %s\t", socket_number, buff); 
        // fflush(stdout);
        
        // and send that buffer to client 
        // loop through clients and send message to all connected clients (that didn't send the message)
        for (int i = 0; i < connected_clients; i++){
            pthread_mutex_lock(&lock);  // keep the threads behaving
            
            // send the message to all threads that did not send message
            int socket_i = client_connection_array[i].socket;
            if (client_connection_array[i].still_connected && (socket != socket_i)){
                char message[14 + MAX];
                snprintf(message, sizeof(message), "Client %d says: %s", socket_number, buff);
                write(socket_i,  message, sizeof(message));
            }
            
            pthread_mutex_unlock(&lock);  // free thread
        }
        
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        
        bzero(buff, MAX); // clear buff after it's used.
        }
    }
    free(given_args);
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
            perror("Server accept failed...\n"); 
        }
        // see if the server is at it's max number of clients already
        else if (connected_clients >= MAX_CLIENTS){
            perror("Maximum number of clients already met.");
            close(incoming_connection);
        }
        // if there's no reason not to, accept the client
        else{
            struct thread_args* args = malloc(sizeof(struct thread_args));
            args -> socket = incoming_connection;
            args -> index = connected_clients;
            store_connection(incoming_connection, connected_clients, &client_connection_array);
            connected_clients++;
            printf("Server accept the client...\n");
            pthread_mutex_unlock(&lock);
            
            pthread_t tid;
            pthread_create(&tid, NULL, (void*)server_to_client, (void*)args);
        }
        
        printf("\n");
    }
    return EXIT_SUCCESS;
}
