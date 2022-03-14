#ifndef _HELPERS_H
#define _HELPERS_H 1

#define FALSE 0
#define BUFLEN 1024
#define IDLEN 15

typedef struct client {
    char id[15];
    int socket;
    int sf;
    char topic[50];
    char continut[1500];
} client;

#endif