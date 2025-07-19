#ifndef MSG_FORMAT_H
#define MSG_FORMAT_H
#include <stdint.h>
// uncomment if on WSL or linux
//#define LINUX
#ifdef LINUX
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif
const uint32_t REGULAR = 0;
const uint32_t SPECIAL = 1;
const uint32_t LOGOUT = 2;
const uint32_t LOGIN = 3;

struct first_header{
    uint32_t type;
};
struct special_header{
    uint32_t var1;
    uint8_t blahblah;
    uint16_t othervar;
};
struct regular_header{
    uint32_t msg_len;
};
#endif // Header guard end