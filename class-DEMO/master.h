/*************************************************************************
	> File Name: master.h
	> Author: 
	> Mail: 
	> Created Time: 2019年03月09日 星期六 18时16分57秒
 ************************************************************************/

#include "common.h"

typedef struct User {
    struct sockaddr_in client_addr;
    struct User *next;
} User, *List;

struct PRINT{
    List head;
    int index;
    
};

struct HEART {
    List head;
    int ins;
};

int trans_ip(char *sip, int *ip) {
    if(sip == NULL) return -1;
    char temp[4];
    int count = 0;
    while(1) {
        int index = 0;
        while(*sip != '\0' && *sip != '.' && count < 4) {
            temp[index++] = *ip;
            sip++;
        }
        if(index == 4) return -1;
        temp[index] = '\0';
        ip[count] = atoi(temp);
        count++;
        if(*sip == '\0') {
            if(count == 4) return -1;
        } else {
            sip++;
        }
    }
    return 0;
}

int insert(List head, User *user) {
    User *p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = user;
    return 1;
}

int find_min (int *sum, int ins) {
    int ans = -1;
    for(int i = 0; i < ins; i++) {
        if(*(sum + i) < *(sum + ins)) ans = i;
    }
    return ans;
}

void *print(void *arg) {
    struct PRINT *print_para = (struct PRINT *)arg;
    int index = print_para->index;
    char fn[20] = {0};
    sprintf(fn, "./%d.log", index);
    int temp = 0;mZ    while (1) {
        FILE *file = fopen(fn, "w");
        User *p = print_para->head;
        fprintf(file, "%d\n", tepm++);
        while (p->next != NULL) {
            //printf("%s:%d\n", inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
            fprintf("%s:%d\n", inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
            p = p->next;
        }
        fclose(file);
        sleep(1);
    }
    return NULL;
}

int connect_sock(struct sockaddr_in addr) {
    int sock_fd;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DBG("socket f\n");
        return 1;
    }
    if(connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        DBG("connect f\n");
        return -1;
    }

    close(sock_fd);
    return 0;
}


void *heart_beat(void *arg) {
    struct HEART *heart;
    heart = (struct HEART *)arg;
    while (1) {
        for (int i = 0; i < heart->ins; i++) {
            Node *p;
            p = heart->head + i;
            while (p != NULL && p->next != NULL) {
                char ip[20] = {0};
                strcpy(ip, inet_ntoa(p->next->client_addr.sin_addr));
                if(connect_sock(p->next->client_addr) < 0) {
                    Node *temp = NULL;
                    temp = p->next;
                    p->next = p->next->next;
                    free(temp);
                } else {
                    DBG("%s : %d online \n", inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
                    p = p->next;
                }
            }
        }
    }
}

