#include "msg_format.h"
#include "string.h" // strlen
#include <stdio.h> // File I/O
#define MSG_RECV_LEN 1024
int main(int argc, char**argv){
    // Buffer for full message + headers
    char buff_to_recv[MSG_RECV_LEN];
    FILE *fptr;
    // Pretend we recieved a message from the client
    fptr = fopen("to_server.txt", "r"); 
    // Read in first header and interpret buffer as the first header
    fread(buff_to_recv, 1, sizeof(struct first_header), fptr);
    struct first_header* temph1 = (struct first_header*) buff_to_recv;
    // If the message type is a regular message, proceed
    if(ntohl(temph1->type) == REGULAR){
        // Read in regular header and interpret buffer as the regular header
        fread(buff_to_recv, 1, sizeof(struct regular_header), fptr);       
        struct regular_header* temph2 = (struct regular_header*) buff_to_recv;
        uint32_t msg_len = ntohl(temph2->msg_len);
        // Finally get the msg_len number of bytes out of the file and display it
        fread(buff_to_recv, 1, msg_len, fptr);  
        printf("Size of message: %d\n", msg_len);
        printf("Message: %s\n", buff_to_recv);
    }else{
        return -1;
    }
    return 0;
}