#include <stdio.h>
#include <zlib.h>

int main(int argc,char **args)
{
    /*原始数据*/
    unsigned char strsrc[]="这些是测试数据。123456789 abcdefghigklmnopqrstuvwxyz\n\t\0abcdefghijklmnopqrstuvwxyz\n"; //包含\0字符
    unsigned char buf[1024]={0};
    unsigned char strdst[1024]={0};
    unsigned long srclen=sizeof(strsrc);
    unsigned long buflen=sizeof(buf);
    unsigned long dstlen=sizeof(strdst);
    int i;
    FILE * fp;

    printf("源串:");
    for(i=0;i<srclen;++i)
    {
        printf("%c",strsrc[i]);
    }
    printf("原串长度为:%ld\n",srclen);

    printf("字符串预计算长度为:%ld\n",compressBound(srclen));
    //压缩
    compress(buf,&buflen,strsrc,srclen);
    printf("压缩后实际长度为:%ld\n",buflen);
    //解压缩
    uncompress(strdst,&dstlen,buf,buflen);

    printf("目的串:");
    for(i=0;i<dstlen;++i)
    {
        printf("%c",strdst[i]);
    }

    return 0;
}
