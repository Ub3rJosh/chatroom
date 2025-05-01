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


#include <sys/socket.h>


int main(){
    // https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Sockets.html#:~:text=The%20socket%20interface%20in%20C,function%20call%20is%20the%20same.
    
    
    // Step 1: Socket(s)
    
    // Allow for up to 4 people to be chatting at a time
    int client_socket;
    client_socket = socket(AF_LOCAL, SOCK_STREAM, 0);
    
    
    // Step 2: Connect to server
    
    
    // Step 3: Send message to server (to be sent to other clients)
    
    
    // Step 4: Receive messages from server (taken from other clients)
    
    
    // Step 5: Terminate if requested
    
    return 0;
}