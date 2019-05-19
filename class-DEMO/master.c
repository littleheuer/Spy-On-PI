/*************************************************************************
	> File Name: master.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月09日 星期六 18时12分18秒
 ************************************************************************/

#include "master.h"
#include "DEBUG.h"
char config[] = "./test.conf";

int main () {
    int start[4];
    int finish[4];
    char start_ip[20] = {0};
    char finish_ip[20] = {0};
    char port_t[7] = {0};
    char ins_s[5] = {0};
    pthread_t print_t;


    get_conf_value(config, "from", start_ip);
    get_conf_value(config, "To", finish_ip);
    get_conf_value(config, "INS", ins_s);
    get_conf_value(config, "client_port", port_t);

    int ins = atoi(ins_s);
    int port = atoi(port_t);
    trans_ip(start_ip, start);
    trans_ip(finish_ip, finish);

    int *sum = (int *) malloc(ins * sizeof(int));

    DBG("start = %d.%d.%d.%d\n", start[0], start[1], start[2], start[3]);
    
    List *list = (List *)malloc(ins * sizeof(List));

    struct sockaddr_in initaddr;
    initaddr.sin_family = AF_INET;
    initaddr.sin_port = htons(port);
    initaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    for (int i = 0; i < ins; i++) {
        User *p = (User *)malloc(sizeof(User));
        p->client_addr = initaddr;
        p->next = NULL;
        list[i] = p;
    }
    
    char host[20] = {0};

    for (int i = start[3]; i <= finish[3]; i++) {
        sprintf(host, "%d.%d.%d.%d", start[0], start[1], start[2], start[3]);
        User *p = (User *)malloc(sizeof(User));
        p->client_addr = initaddr;
        p->next = NULL;
        int sub = find_min(sum, ins);
        insert(list[sub], p);
    }
    
    pthread_create(&print_t, NULL, print, (void *)list[0]);
    
    int temp_i[ins] = {0};
    pthread_t t[ins];
    for(int i = 0; i < ins; i++) {
        struct PRINT print_para;
        temp_i[i] = i;
        print_para.head = list[i];
        if(pthread_create(*t[i], NULL, print, (void *)print_para) == -1) {
            printf("pthread create error\n");
            return -1;
        }
    }
    

    pthread_join(print_t, NULL);

    return 0;
}
