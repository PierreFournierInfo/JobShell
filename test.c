#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

char* tab_char(char* s){
    char *mots[];
    int i=0;
    int v=0;
    while (s[i] != '\0') {
        if(strcmp(s[i]," ")){
            v++;
        }else{
            strcat(mots[v],s[i]);
        }
        i++;
    }
}