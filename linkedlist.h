#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <math.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "helpers.h"

typedef struct node {
    struct client val;
    struct node *next;
} node_t;

extern void push(node_t **head, struct client val);
extern void print_list(node_t * head);
extern void delete_node(node_t **head_ref, int socket);
extern void send_client(node_t *head, char *topic, char *continut);


void push(node_t **head, struct client val) {
    node_t * new_node;
    new_node = (node_t *)malloc(sizeof(node_t));

    new_node->val = val;
    new_node->next = *head;
    *head = new_node;
}

void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%s\n", current->val.id);
        current = current->next;
    }
}

void delete_node(node_t **head_ref, int socket) { 
    node_t* temp = *head_ref, *prev; 
 
    if (temp != NULL && temp->val.socket == socket) {
        *head_ref = temp->next;
        free(temp);
        return; 
    }

    while (temp != NULL && temp->val.socket != socket) { 
        prev = temp; 
        temp = temp->next; 
    } 
    if (temp == NULL) return; 
    prev->next = temp->next; 
    free(temp); 
}


void send_client(node_t *head, char *topic, char *continut) {
    node_t *node = head;
    for (; node != NULL; node = node->next) {
        if (strcmp(node->val.topic, topic) == 0) {
            /* send the message to rigth client */
            send(node->val.socket, continut, strlen(continut), 0);
        }
    }
}