#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int V_return;

int pwd(){
    char *buf;
    size_t size;
    getcwd(buf,size);
    if(errno==ERANGE){V_return=1;return 1;}
    printf("%s",buf);
    V_return=0;
    return 0;
}

int point_interrogation(){
    return V_return;
}

int exit_bis(int val){
    pid_t p=fork();// tentative 1
    if(p>0){
        if(kill(p,0)==0){
            V_return=1;
            return 1;
        }else{
            if(val!=0){
                V_return=val;
                return val;
            }else{
                return V_return;
            }
        }
    }else{
        
    }
   
}
int cd(char *path){
    int test=chdir(path);
    if(test==0){
        printf("réussite du cd");
    }else{
        perror("erreur");
    }
}