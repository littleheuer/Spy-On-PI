/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月07日 星期四 13时10分08秒
 ************************************************************************/

#include<stdio.h>
#include "common.h"
#include "spy.h"

int server_listen;
char fn[] = "./spy.log";
Linkedlist list;

void *addnode(void *argv) {
    Node *p = (Node *) argv;
    list = insert(list, p);
    char buffer[1024] = {0};
    while(recv(server_listen, buffer, sizeof(buffer), 0) > 0) {
        log_write(fn, "%s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    list = dele_ip(list, p);
}




int main() {
    pthread_t addnode_t;
    int server_port = 8731;
    int socket_fd;
    
    char file_log[] = "./spy.log";

    list = (Node *) malloc(sizeof(Node));
    list->next = NULL;

    if(server_listen = socket_create(server_port) < 0) {
        printf("error create socket\n");
        return 1;
    }
    struct sockaddr_in c_addr;
    int addr_len = sizeof(c_addr);
    while(1) {
        if((socket_fd = accept(server_listen, (struct sockaddr*)&c_addr, (socklen_t*) &addr_len)) < 0) {
            printf("%d\n", socket_fd);
            perror("accept");
            return 1;
        }
        if(!search_ip(list, &c_addr)) {
            Node *node = NULL;
            node = create_node(node, &c_addr);
            char buffer[1024] = {0};
            pthread_create(&addnode_t, NULL, addnode, (void *) node);
        }
    }
    clear(list);
    return 0;
}
