#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "signal_handler.h"
#include "job_manager.h"

//kill()

int killProjet(int sig, Job job ){
    return 1;
}
int killProjet( Job job ){
    return 1;
}

int killProjet(int sig, int pid ){
    return 1;
}
int killProjet(int pid){
    return 1;
}



//Je commence par une fonction limitée qui exécute deux commandes à la fois (ne fonction donc qu'avec un seul pipe)
int pipeLimitedTwo( char ** command1, char ** command2){
    return 0;
}
