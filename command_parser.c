
#include <errno.h>

int V_return;

void pwd(){
    char *buf;
    getcwd(buf);
    if(errno==ERANGE){V_return=1;return 1;}
    printf(buf);
    V_return=0;
    return 0;
}

void point_interrogation(){
    return V_return;
}