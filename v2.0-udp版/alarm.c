/*************************************************************************
	> File Name: alarm.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月19日 星期二 21时07分32秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<signal.h>
void sigal(int sig) {
    if(sig == SIGALRM) {
        printf("hello\n");
    }
    return ;
}

int main () {
    alarm(2);
    signal(SIGALRM, sigal);
    char buf[1024];
    while (1) {
        scanf("%s", buf);
        printf("%s\n", buf);
    }
    return 0;
}
