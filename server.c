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
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()

#define MAX_SOCKETS 4  // max number of clients
#define PORT 8080  // port to connect to
#define SA struct sockaddr


void store_socket(int input_socket, int socket_number, int (*socket_list)[MAX_SOCKETS]){
    // update socket array on successful creation
    if (input_socket != -1){
        (*socket_list)[socket_number] = input_socket;
    }
    else{
        printf("Unable to create socket.");
    }
}


int main(){
    // https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Sockets.html#:~:text=The%20socket%20interface%20in%20C,function%20call%20is%20the%20same.
    
    // Step 1: Socket(s) (and use Setsockopt?)
    
    // Allow for up to MAX_SOCKETS number of people to be chatting at a time
    int sockets[MAX_SOCKETS] = {-1};  // initialize all values to -1
    
    int temp_socket;
    for (int i = 0; i < MAX_SOCKETS; i++){
        temp_socket = socket(AF_INET, SOCK_STREAM, 0);  // Use TCP (would be SOCK_DGRAM for UDP)
        store_socket(temp_socket, i, &sockets);
    }
    
    
    // Step 2: Bind
    struct sockaddr_in server_address; 
    bzero(&server_address, sizeof(server_address));
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);
    
    int bound_socket;
    for (int i = 0; i < MAX_SOCKETS; i++){
        // only bind successfully "socketed" sockets
        if (sockets[i] != -1){
            // int bind (int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
            // bound_socket = bind(sockets[i], *server_address);
            bound_socket = bind(sockets[i], (SA*)&server_address, sizeof(server_address));
        }
        else{
            printf("Socket not created. Quiting.");
            return -1;
        }
    }
    
    // sockaddr_in hint;
    // hint.sin_family = AF_INET;
    // hint.sin_port = htons(54000);
    // hint.sin_addr.s_addr = INADDR_ANY;
    // bind(temp_socket, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
    
    
    // Step 3: Listen
    
    
    // Step 4: Accept new clients (if possible)
    
    
    // Step 5: Take input from client
    
    
    // Step 6: Sent to other clients
    
    
    // Step 7: Terminate if requested
    
    
    printf("\n");
    return 0;
}