#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <math.h> 
#include <sys/time.h>
#include "helpers.h"
#include "linkedlist.h"

int max(int x, int y) {
    if (x < y) return y; 
    else return x; 
} 

int main(int argc , char *argv[]) {
    if (argc != 2) {
        printf("Error at arguments. Need ./server <PORT>\n");
        return -1;
    }
    int socketfd, ret, addr_len, my_socket,
    client_socket[100] = {0}, max_cl = 100, socket_desc; 
    int max_socket_desc, udpfd;                          /* udp socket and socket descriptor for max socket*/ 
    struct sockaddr_in address;                 /* structure of address to set */
    struct sockaddr_in client;              
    char buffer[BUFLEN];                        /* BUFFER for string managment */
    char id[IDLEN];                             /* Client ID */
    node_t *list = NULL;                        /* Pointer to a list for all clients */
    fd_set readfds;                             /* FD SET for multiple connections*/
 
    ret = socketfd = socket(AF_INET , SOCK_STREAM , 0);         /* Create a socket for server */
    if(ret == 0) {
        printf("Socket error\n"); 
        return -1; 
    }

    udpfd = socket(AF_INET, SOCK_DGRAM, 0);                     /* Create a socket to listen UDP Clients*/
    if (udpfd == 0) {
        printf("Socket error\n");
        return -1;
    }
    
    /* Fill the address structure with port, IPv4 and any address */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));
        
    /* Bind the socket to local address */ 
    ret = bind(socketfd, (struct sockaddr *)&address, sizeof(address));
    if (ret < 0) {
        printf("Bind error\n"); 
        return -1; 
    }
    /* Bind the UDP socket to local address */
    ret = bind(udpfd, (struct sockaddr*)&address, sizeof(address));
    if (ret < 0) {
        printf("Bind error\n");
        return -1;
    }
    ret = listen(socketfd, 10);
    if (ret < 0) {
        printf("Listen error\n"); 
        return -1; 
    }
    
    max_socket_desc = max(udpfd, socketfd);                          /* Find the maximum of the sockets */
    
    addr_len = sizeof(address); 
    int clientlen = sizeof(client);
    
    while(1) {  
        FD_ZERO(&readfds);                                  /* Clear the socket set */ 
    
        FD_SET(socketfd, &readfds);                         /* Add the UDP and TCP socket to the set */
        FD_SET(udpfd, &readfds);

        for (int i = 0; i < max_cl; i++) {             /* Add other sockets to the FD set and compute the maxim on them */ 
            if(client_socket[i] > 0) 
                FD_SET(client_socket[i], &readfds);  
            if(client_socket[i] > max_socket_desc) 
                max_socket_desc = client_socket[i]; 
        }
        /* Select an active socket from the FD Set */
        int activity = select(max_socket_desc + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error\n");
            return -1; 
        } 
        if (FD_ISSET(socketfd, &readfds)) { 
            if ((my_socket = accept(socketfd, (struct sockaddr*)&client,
            (socklen_t*)&clientlen)) < 0) {
                printf("Accept error\n"); 
                return -1; 
            } 
            
            memset(id, 0, IDLEN);                              /* Print the information about connected user */
            recv(my_socket, id, 10, 0);
            printf("New client with (%s) connected from  %s:%d\n", id,
            inet_ntoa(client.sin_addr), ntohs(client.sin_port)); 
        
            for (int i = 0; i < max_cl; i++) {             /* Add new user to socket array */
                if(client_socket[i] == 0) {
                    client_socket[i] = my_socket; 
                    break; 
                }
            }
        }
        if (FD_ISSET(udpfd, &readfds)) {                        /* Get an UDP message from UDP clients */
            printf("Message from UDP client: \n");
            int len = sizeof(client);
            char new_type;                                      /* Parsing UDP message */
            char new_continut[1500];                            /* Main message of client */
            char new_topic[50];                                 /* Topic of the message */
            char output[2000];                                  /* Buffer to store final string */
            memset(output, 0, 2000);
            memset(buffer, 0, 2000);
            int n = recvfrom(udpfd, buffer, 2000, 0,            /* Recieve the UDP message */
            (struct sockaddr*)&client, &len);                   /* Assigne UDP information to client structure */
            strncpy(new_topic, buffer, 50);                     /* Topic, e.g. {ana_string_announce}*/
            strncpy(&new_type, buffer+50, 1);                   /* Its type, e.g. {STRING} */
            printf("Data type: %d\n", new_type);

            char port[5];                                       /* Convert client PORT to string */
            sprintf(port, "%d", client.sin_port);
                                                                /* Concat the full message */
            strcat(output, inet_ntoa(client.sin_addr));         /* Convert IP to string */
            strcat(output, ":");
            strcat(output, port);
            strcat(output, " - ");
            strcat(output, new_topic);
            strcat(output, " - ");

            if (new_type == 0) {                                /* If type is INT */
                uint8_t sign;
                uint32_t new_int;
                memcpy(&sign, buffer + 51, 1);                  /* Read from buffer fisrt char, sign */
                memcpy(&new_int, buffer + 52, 4);               /* Read from buffer unsigned int */
                if (!sign) {                                    /* If positive sign */
                    char string_n[25];                          /* Convert the number to string */
                    sprintf(string_n, "{%d}", ntohl(new_int));
                    strcat(output, string_n);                   /* Concat the final number */
                } else {
                    char string_n[25];
                    sprintf(string_n, "{-%d}", ntohl(new_int)); /* If negative sign */
                    strcat(output, string_n);                   
                }
            }
            if (new_type == 1) {                                /* If the type is SHORT_REAL*/
                uint16_t my_type;                               /* Store to 2 bytes from buffer */
                memcpy(&my_type, buffer + 51, 2);
                char string_n[25];                              /* COnvert to string */
                sprintf(string_n, "{%.2f}", ntohs(my_type)/100.0);
                strcat(output, string_n);
            }
            if (new_type == 2) {                               /* If type is FLOAT */
                uint32_t number;
                uint8_t my_power, sign;                        /* 1 Byte for sign char and power */
                memcpy(&sign, buffer + 51, 1);                 /* Store the sign of FLOAT */
                memcpy(&number, buffer + 52, 4);               /* Strore part of the number of FLOAT */
                memcpy(&my_power, buffer + 56, 1);             /* Store the power of FLOAT */
                number = ntohl(number);                        /* Conver from network to host order */ 
                float x = pow(10, my_power);                   /* Compute the power */
                if (!sign) {
                    char string_n[25];
                    sprintf(string_n, "{%.4f}", number / x);   /* Convert the number to string */
                    strcat(output, string_n);
                } else {
                    char string_n[26];
                    sprintf(string_n, "{-%.4f}", number / x); /* Conver the number to string with sign */
                    strcat(output, string_n);
                }
            }
            if (new_type == 3) {                                        /* If type is STRING */
                printf("Continut: {%s}\n", (char*)memcpy(new_continut,  /* Output to server side */
                buffer + 51, 1500));
                char this_continut[1502];                               /* Store the main */
                sprintf(this_continut, "{%s}", (char*)memcpy(new_continut,
                buffer + 51, 1500));
                strcat(output, this_continut);
            }
            printf("%s\n", output);                                     /* Send the information to the clients */
            send_client(list, new_topic, output);                       /* that are subscribed from this topic */
        }
        for (int i = 0; i < max_cl; i++) { 
            socket_desc = client_socket[i]; 
            
            if (FD_ISSET(socket_desc, &readfds)) {                               /* If got information from other sockets */
                memset(buffer, 0, 1024);
                int req = recv(socket_desc, buffer, 1024, 0);
                if (req == 0) { 
                    getpeername(socket_desc, (struct sockaddr*)&address,         /* Somebody disconnected, get his details and print */
                    (socklen_t*)&addr_len);
                    printf("Client disconnected (%s). %s:%d \n", id,    /* Print the IP and PORT of disconnected client */
                    inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(socket_desc);                                          /* Close the disconnected socket */
                    client_socket[i] = 0;                               /* Set to zero for future using */
                }
                if (strncmp(buffer, "subscribe", 9) == 0) {             /* Server got a subscribe message from a client */
                    struct client new_client;                           /* Make a client structure to add in the list */
                    sscanf(id, "%s", new_client.id);                    /* Add user's ID */
                    sscanf(buffer + 9, "%s", new_client.topic);         /* User's topic he wants to subscribe */
                    sscanf(buffer, "%d", &new_client.sf);               /* S&F */
                    new_client.socket = socket_desc;                             /* User's socket */
                    printf("Got new subscriber => Topic: %s, Socket number: %d, ID: %s\n",
                    new_client.topic, new_client.socket, new_client.id);
                    push(&list, new_client);
                    
                    memset(buffer, 0, 1024);                            /* Sending ACK to subscriber */
                    strcpy(buffer, "You were subscribed\n");
                    send(socket_desc, buffer, 1024, 0);
                }
                if (strncmp(buffer, "unsubscribe", 11) == 0) {          /* Server got a unsubscribe message from client */
                    struct client new_client;
                    sscanf(id, "%s", new_client.id);
                    sscanf(buffer+11, "%s", new_client.topic);
                    sscanf(buffer, "%d", &new_client.sf);
                    new_client.socket = socket_desc;
                    printf("Remove a subscriber => Topic: %s, Socket number: %d, ID: %s\n",
                    new_client.topic, new_client.socket, new_client.id);
                    delete_node(&list, socket_desc);                            /* Delete the user with socket from list */

                    memset(buffer, 0, 1024);                            /* Sending ACK to subscriber */
                    strcpy(buffer, "You were unsubscribed...\n");
                    send(socket_desc, buffer, 1024, 0);
                }
            }
        }
        if (FD_ISSET(0, &readfds)) {                                    /* Read from key-B and try to exit */
            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN - 1, stdin);
            if (strncmp(buffer, "exit", 4) == 0) {
                return -1;
            }
        }
    }
    return 0; 
}