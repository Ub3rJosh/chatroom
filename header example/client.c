#include "msg_format.h"
#include "string.h" // strlen
#include <stdio.h> // File I/O
#define MSG_SEND_LEN 1024
int main(int argc, char**argv){
    // Buffer for full message + headers
    char buff_to_send[MSG_SEND_LEN];
    // Message to send
    char* msg = "Hello World\n\0";
    // Load type into first header
    struct first_header h1;
    h1.type = htonl(REGULAR);
    // Load message length into second header
    struct regular_header h2;
    // strlen might not be the best way to do this
    // Doesnt include null terminator so we add 1
    uint32_t msg_len = strlen(msg) + 1; 
    h2.msg_len = htonl(msg_len);
    // load headers + msg into the buffer
    /*
    Packet format
    | First header | Second header | msg |
    */
    memcpy(buff_to_send, (char*) &h1, sizeof(struct first_header));
    memcpy(buff_to_send + sizeof(struct first_header), (char*) &h2, sizeof(struct regular_header));
    memcpy(buff_to_send + sizeof(struct first_header) + sizeof(struct regular_header), msg, msg_len);
    // Buffer is loaded so we "send it" by writing to a file
    // Pretend we did, this is up to the OS + what server/client code you use
    FILE *fptr;
    fptr = fopen("to_server.txt", "w");
    fwrite(buff_to_send, 1, sizeof(struct first_header) + sizeof(struct regular_header) + msg_len, fptr);
    fclose(fptr); 
    return 0;
}