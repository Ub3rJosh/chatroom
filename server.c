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
*/


#include <sys/socket.h>


int main(){
    // https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Sockets.html#:~:text=The%20socket%20interface%20in%20C,function%20call%20is%20the%20same.
    
    
    // Step 1: Socket(s)
    
    // Allow for up to 4 people to be chatting at a time
    int socket_1;
    int socket_2;
    int socket_3;
    int socket_4;
    
    socket_1 = socket(AF_LOCAL, SOCK_STREAM, 0);
    
    
    // Step 2: Bind
    
    
    // Step 3: Listen
    
    
    // Step 4: Accept new clients (if possible)
    
    
    // Step 5: Take input from client
    
    
    // Step 6: Sent to other clients
    
    
    // Step 7: Terminate if requested
    
    return 0;
}