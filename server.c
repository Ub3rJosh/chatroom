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

#define MAX_CLIENTS 4  // max number of connections to the server at a time
#define PORT 8080  // port to connect to
#define MAX 80  // max message size
#define SA struct sockaddr


int client_connections[MAX_CLIENTS];
int connected_clients = 0;
void store_connection(int input_socket, int socket_number, int (*socket_list)[MAX_CLIENTS]){
    (*socket_list)[socket_number] = input_socket;
}

struct connections{
    int connection_number;
    bool still_connected;
    int socket;
};

void server_to_client(int socket){
    char buff[MAX]; 
    int n; 
    
    for (;;){ 
        bzero(buff, MAX); 
        
        // read the message from client and copy it in buffer 
        read(socket, buff, sizeof(buff)); 
        
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n'); 
        
        // and send that buffer to client 
        write(socket, buff, sizeof(buff)); 
        
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
}


int main(){
    // https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Sockets.html#:~:text=The%20socket%20interface%20in%20C,function%20call%20is%20the%20same.
    
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
        perror("Binding unsuccessful. Quitting.");
        return -1;
    }
    
    
    // Step 3: Listen
    printf("Server is listening for new clients.");
    int listen_to_socket = listen(sockfd, 5);
    
    
    // Step 4: Accept new clients (if possible)
    struct sockaddr_in client; 
    socklen_t len = sizeof(client);
    int connection = accept(sockfd, (SA*)&client, &len); 
    if (connection < 0){ 
        perror("server accept failed...\n"); 
        return -1;
    }
    else{
        store_connection(connected_clients, connection, connected_clients);
        connected_clients++;
        printf("server accept the client...\n");
        
    }
    
    
    // Step 5: Take input from client
    // Step 6: Sent to other clients
    server_to_client(connection);
    
    
    // Step 7: Terminate if requested
    close(connection);
    
    
    printf("\n");
    return 0;
}