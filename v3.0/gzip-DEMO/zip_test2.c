#include <stdio.h>
#include <zlib.h>
#include <string.h>
int main(int argc,char **args)
{
    gzFile file, rfile;
    char buffer[1024] = {0};
    file=gzopen("fuc.gz","wb");
    if(NULL==file)
        perror("Can't open file");
    gzsetparams(file,2,0);
    FILE *fp = NULL;
    fp = fopen("te.txt", "r");
    while(fgets(buffer, 1024, fp)) {
        int len = strlen(buffer);
        gzwrite(file, buffer, len);
    }
    gzclose(file);
    rfile = gzopen("fuc.gz","rb");
    while(gzread(rfile, buffer, sizeof(buffer)) > 0) {
        printf("%s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    gzclose(rfile);
    fclose(fp);
    return 0;
}
