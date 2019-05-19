/*************************************************************************
	> File Name: access.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月30日 星期六 11时23分55秒
 ************************************************************************/

#include<stdio.h>
//#include<sys/io.h>
#include<unistd.h>
int main () {
    char fn1[] = "../common/common.l";
    if(access(fn1, F_OK) == 0) {
        printf("存在\n");
    } else {
        printf("不存在\n");
    }

    return 0;
}
