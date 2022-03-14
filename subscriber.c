#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"

    /* Need arguments in order: <ID_Client>, <IP_Server>, <Port_Server>
    */
int main(int argc, char *argv[]) {
    int sockfd, n, ret, max_sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFLEN];
    struct client my_client;
    
    memcpy(my_client.id, argv[1], sizeof(argv[1]));

    fd_set read_fds;
    fd_set tmp_fds;
    if (argc != 4) {
        printf("Error at arguments. Need <ID_Client>, <IP_Server>, <Port_Server>\n");
        return -1;
    }

    if (strlen(argv[1]) > 10) {
        printf("ID should contains less or 10 chars\n");
        return -1;
    }
    FD_ZERO(&tmp_fds);
    FD_ZERO(&read_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error socket\n");
    }

    FD_SET(sockfd, &read_fds);
    max_sock = sockfd;
    FD_SET(0, &read_fds);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    ret = inet_aton(argv[2], &serv_addr.sin_addr);
    if (ret == 0) {
        printf("Error inet_aton\n");
    }
    ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        printf("Connection error\n");
    }
    
    /* Sending a packet to find out client's ID
    */
    send(sockfd, argv[1], strlen(argv[1]), 0);
    while (1) {
        tmp_fds = read_fds;

        ret = select(max_sock + 1, &tmp_fds, NULL, NULL, NULL);
        if (ret < 0) {
            printf("Selection error\n");
        }
        if (FD_ISSET(sockfd, &tmp_fds)){ /* Request a message from server */
            memset(buffer, 0, 2000);
            int req = recv(sockfd, buffer, 2000, 0);
            printf("%s\n", buffer);
        }
        if (FD_ISSET(0, &tmp_fds)) {  /* Read from key-b and send to server */
            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN - 1, stdin);
            if (strncmp(buffer, "exit", 4) == 0) {
                return -1;
            }
            if (strncmp(buffer, "subscribe", 9) == 0) {
                n = send(sockfd, buffer, 2000, 0);
                if (n < 0) {
                    printf("Send error\n");
                }
                continue;
            }
            if (strncmp(buffer, "unsubscribe", 11) == 0) {
                n = send(sockfd, buffer, 1024, 0);
                if (n < 0) {
                    printf("Send error\n");
                }
                continue;
            }
            printf("The client supports only: subscribe, unsubscribe and exit commands...\nTry again!\n");
        }
    }
    close(sockfd);
    return 0;
}