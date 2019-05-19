/*************************************************************************
	> File Name: ../test/login_name.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月02日 星期二 00时21分44秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main() {
    printf("%s\n", getlogin());
    return 0;
}
