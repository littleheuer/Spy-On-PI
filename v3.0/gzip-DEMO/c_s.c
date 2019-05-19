/*************************************************************************
	> File Name: c_s.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月20日 星期三 20时11分27秒
 ************************************************************************/

#include<stdio.h>
#include "../common.h"

struct sm_msg {
    pthread_mutex_t sm_mutex;
    pthread_cond_t sm_ready;
};

pthread_mutexattr_t m_attr;
pthread_condattr_t c_attr;

char *share_memory;
struct sm_msg *msg;

int main () {
    int shmid;
    shmid = shmget(IPC_PRIVATE, sizeof(struct sm_msg), 0666|IPC_CREAT);
    share_memory = (char *) shmat(shmid, 0, 0);
    msg = (struct sm_msg *) share_memory;
    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);
    
    int pid = fork();
    if(pid == 0) {
        pthread_mutex_lock(&msg->sm_mutex);
        printf("1st lock sent signal 5s later\n");
        sleep(5);
        pthread_cond_signal(&msg->sm_ready);
        pthread_mutex_unlock(&msg->sm_mutex);
        printf("1 unlock\n");
    } else {
        int pid2 = fork();
        if(pid2 == 0) {
            pthread_mutex_lock(&msg->sm_mutex);
            printf("I'm in\n");
            pthread_cond_wait(&msg->sm_ready, &msg->sm_mutex);
            printf("I'm ready!\n");
            sleep(5);
            pthread_mutex_unlock(&msg->sm_mutex);
            printf("2 unlock\n");
        } else {
            while (1) {
                printf("主程序打酱油\n");
                sleep(3);
            }
        }
    }


    return 0;
}
