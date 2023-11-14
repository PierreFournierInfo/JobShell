#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int V_return;

void pwd(){
    char *buf;
    size_t size;
    getcwd(buf,size);
    if(errno==ERANGE){V_return=1;return 1;}
    printf(buf);
    V_return=0;
    return 0;
}

void point_interrogation(){
    return V_return;
}

void exit(int val){
    pid_t p=fork();// tentative 1
    if(p>0){
        if(kill(p,0)==0){
            V_return=1;
            return 1;
        }else{
            if(val!=NULL){
                V_return=val;
                return val;
            }else{
                return V_return;
            }
        }
    }
   
}

void jobs(){
    pid_t enfant=fork();
    if(enfant==-1){
        perror("erreur lors du fork");
        return -1;

    }else if(enfant==0){
        printf("processus enfant (PID: %d)/n",getpid());
        execlp("ps","ps","-ef",NULL);

    }else{
        pritnf("processus parent (PID: %d), mon enfant a le pid %d/n",getpid(),enfant);
        wait(NULL);
    }
    return 0;
}