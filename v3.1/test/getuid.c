/*************************************************************************
	> File Name: getuid.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月02日 星期二 00时27分21秒
 ************************************************************************/

#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
int main(void)
{
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    printf("当前登陆的用户名为：%s\n", pwd->pw_name);
    return 0;

}
