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
#include <sys/socket.h>

#define MAX_SOCKETS 4


void store_socket(int input_socket, int socket_number, int (*socket_list)[MAX_SOCKETS]){
    // update socket array on successful creation
    if (input_socket != -1){
        (*socket_list)[socket_number - 1] = input_socket;
    }
}


int main(){
    // https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Sockets.html#:~:text=The%20socket%20interface%20in%20C,function%20call%20is%20the%20same.
    
    // Step 1: Socket(s)
    
    // Allow for up to 4 people to be chatting at a time
    int socket_1;
    int socket_2;
    int socket_3;
    int socket_4;
    int sockets[MAX_SOCKETS] = {-1};  // initialize all values to -1
    printf("%i", sockets[0]);
    printf("\n");
    printf("%i", socket_1);
    
    printf("\n\n");
    
    socket_1 = socket(AF_INET, SOCK_STREAM, 0);  // Use TCP (would be SOCK_DGRAM for UDP)
    store_socket(socket_1, 1, &sockets);
    socket_2 = socket(AF_INET, SOCK_STREAM, 0);
    store_socket(socket_2, 2, &sockets);
    socket_3 = socket(AF_INET, SOCK_STREAM, 0);
    store_socket(socket_3, 3, &sockets);
    socket_4 = socket(AF_INET, SOCK_STREAM, 0);
    store_socket(socket_4, 4, &sockets);
    
    printf("%i", socket_1);
    printf("\n");
    printf("%i", sockets[0]);
    
    
    // Step 2: Bind
    // for (int i = 0; i < )
    
    
    // Step 3: Listen
    
    
    // Step 4: Accept new clients (if possible)
    
    
    // Step 5: Take input from client
    
    
    // Step 6: Sent to other clients
    
    
    // Step 7: Terminate if requested
    
    
    printf("\n");
    return 0;
}