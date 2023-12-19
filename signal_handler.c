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
#include "prompt.h"
#include "command_executor.h"


int taille(char** chaine){
    int length = 0;

    // Parcourir le tableau jusqu'à un élément NULL
    while (chaine[length] != NULL) {
        length++;
    }
    return length;
}

//Prends en argument une chaine de caractere de type: "kill [-sig] %job" ou "kill [-sig] pid"
int killProject(char ** c){

    //par défaut
    int signal=SIGTERM;
    char debut= c[0][0];
    int size= taille(c);

    //Trop d'arguments erreur de commande
    if(size>3){
        fprintf(stderr, "Erreur: Trop d'arguments\n");
        return 1;
    }

    //Seulement 1 argument, on utilise SIGTERM par défaut
    else if (size<3){
        if(c[1][0]=='%'){ //  kill %jobX  ---> ici on cherche à envoyer SIGTERM à tous les processus du job numéro jobX
           
            int numJob=atoi(c[1]+1); 
            Job *job = find_job_by_id(numJob);
            if(job){
                killpg(job->process_id, signal);
            }else{
                fprintf(stderr,"Job %d non trouvé\n", numJob);
                return 2;
            }
        }else{  //  kill pid  ---> ici on cherche à envoyer SIGTERM au processus de l'identifiant pid

            pid_t pid=atoi(c[1]);
            if(kill(pid,signal)<0){
                fprintf(stderr, "Probleme kill"); 
                return 3;
            }
        }
    }

    //2 arguments, on envoie le signal c[1]
    else{ 
        signal=atoi(c[1]+1);

        if(c[2][0]=='%'){ //  kill [-sig]  %jobX  ---> ici on cherche à envoyer sig (signal) à tous les processus du job numéro jobX
           int numJob=atoi(c[2]+1);
           Job *job = find_job_by_id(numJob);
            if(job){
                killpg(job->process_id, signal);
            }else{
                fprintf(stderr,"Job %d non trouvé\n", numJob);
                return 4;
            }
        }else{  //  kill [-sig] pid  ---> ici on cherche à envoyer sig(signal) au processus de l'identifiant pid
           //* 
           pid_t pid=atoi(c[2]);
           if(kill(pid,signal)<0){
                fprintf(stderr, "Probleme kill"); 
                return 5;
            }

        }

    }

    return 0;
}

int pipeLimitedTwo( char * c1, char * c2){

    int fd [2];
    int taille1,taille2;
    char** command1=separerParEspaces(c1,&taille1);
    char** command2=separerParEspaces(c2,&taille2);


    if(pipe(fd)<0){
        return 1;
    }

    pid_t pid1=fork();

    if (pid1<0){
        return 2;
    }

    if (pid1==0){
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        close(fd[1]);

        execute_command(command1[0],command1);
        perror("execute_commande");
        exit(EXIT_FAILURE);
    }else{
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
        close(fd[0]);

        wait(NULL);

        execute_command(command2[0],command2);
        perror("execute_commande");
        return 0;

    }



    return 0;
}

// Normalement dans signal handler 
void ignore_signals() {
    struct sigaction act = {0};
    
    // Ignorer le signal Ctrl+C (^C)
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignorer le signal Ctrl+Z (^Z)
    if (sigaction(SIGTSTP, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}
