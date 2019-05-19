/*************************************************************************
	> File Name: spy.c
	> Author: 
	> Mail: 
	> Created Time: 2019年03月05日 星期二 18时52分11秒
 ************************************************************************/

#include<stdio.h>
#include"spy.h"

int log_write(char *fn,const char *fmt, ...) {
    FILE *f = NULL;
    f = fopen (fn, "a+");
    va_list ap;
    va_start(ap, fmt);
    time_t tt = time(NULL);
    struct tm *p = localtime(&tt);
    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d ", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    vfprintf(f, fmt, ap);
    fflush(f);
    fclose(f);
    va_end(ap);
    return 0;
}

//int main() {
//    char fn[] = "hehe.log";
//    char fmt[] = "%s 你好%d\n";
//    int d = 1;
//    char name[] = "呵呵";
//    log_write(fn, fmt, name, d);
//    return 0;
//}
