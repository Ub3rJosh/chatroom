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

#define MAX_SOCKETS 1  // max number of clients
#define PORT 8080  // port to connect to
#define MAX 80  // max message size
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
    
    // Allow for up to MAX_SOCKETS number of people to be chatting at a time
    int sockets[MAX_SOCKETS] = {-1};  // initialize all values to -1
    
    int temp_socket;
    for (int i = 0; i < MAX_SOCKETS; i++){
        temp_socket = socket(AF_INET, SOCK_STREAM, 0);  // Use TCP (would be SOCK_DGRAM for UDP)
        
        // check success of sucket creation
        if (temp_socket == -1){
            printf("Socket not created. Quiting.");
            return -1;
        }
        
        store_socket(temp_socket, i, &sockets);  // store socket in socket array
    }
    
    
    // Step 2: Bind
    struct sockaddr_in server_address; 
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_address.sin_port = htons(PORT); 
    
    int bound_socket;
    for (int i = 0; i < MAX_SOCKETS; i++){
        // only bind successfully "socketed" sockets
        if (sockets[i] != -1){
            bound_socket = bind(sockets[i], (SA*)&server_address, sizeof(server_address));
            
            // check success of binding
            if (bound_socket != 0){
                printf("Binding unsuccessful. Quitting.");
                return -1;
            }
        }
    }
    
    
    // Step 3: Listen
    int listen_to_socket;
    for (int i = 0; i < MAX_SOCKETS; i++){
        listen_to_socket = listen(sockets[i], 5);  // TODO what does `5` do here?
        if (listen_to_socket != 0){
            printf("Listening failed");
            return -1;
        }
    }
    printf("Server is listening for new clients.");
    
    
    // Step 4: Accept new clients (if possible)
    struct sockaddr_in client; 
    int len = sizeof(client);
    for (int i = 0; i <= MAX_SOCKETS; i++){
        int connection = accept(sockets[i], (SA*)&client, &len); 
        if (connection < 0){ 
            printf("server accept failed...\n"); 
            return -1;
        }
        else{
            printf("server accept the client...\n"); 
        }
    }
    
    
    // Step 5: Take input from client
    // Step 6: Sent to other clients
    for (int i = 0; i <= MAX_SOCKETS; i++){
        server_to_client(sockets[i]);
    }
    
    
    
    // Step 7: Terminate if requested
    for (int i = 0; i <= MAX_SOCKETS; i++){
        close(sockets[i]);
    }
    
    printf("\n");
    return 0;
}