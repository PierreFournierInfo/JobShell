#include <signal.h>
#include "prompt.h"
#include "command_executor.h"
#include "job_manager.h"

void execute_command(char *command, char *args[]) {
       
    int background = 0; 
   
    // Vérifier si le dernier argument est "&"
    size_t args_count = 0;
    while (args[args_count] != NULL) {
        args_count++;
    }

    if (args_count > 0 && strcmp(args[args_count - 1], "&") == 0) {
        background = 1;
        // Supprimer "&" de la liste d'arguments
        free(args[args_count - 1]);
        args[args_count - 1] = NULL;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        restore_default_signals();
        //setpgid(0,0);
        
        valeur_de_retour = execvp(command, args);
        
        perror("JSH ");
        exit(EXIT_FAILURE);
    } 
    else {
        char* res = concatenate_arguments(args);
        create_job(pid,res,background);
        
        // pipeline job for plus tard 
        
        Job* job = find_job_by_process_id(pid);

        if(job->process_group_id == 0){
            job->process_group_id = pid;
            if(job->background==0){
                //printf("Background");
                tcsetpgrp(STDIN_FILENO,pid);
                tcsetpgrp(STDOUT_FILENO,pid);
                tcsetpgrp(STDERR_FILENO,pid);
            }
        }
        setpgid(pid,job->process_group_id);

        if (!background) { // pas à l'arrière plan 
        
            int status;
            //printf("Je suis en avant \n");
            if (waitpid(pid, &status, WCONTINUED | WUNTRACED )>0) {
                  
                tcsetpgrp(STDIN_FILENO,getpgrp()); 
                tcsetpgrp(STDOUT_FILENO,getpgrp()); 
                tcsetpgrp(STDERR_FILENO,getpgrp());
               
                // Mise à jour de l'état du job
                if (WIFEXITED(status)) {
                    //printf("J'ai fini\n");
                    valeur_de_retour = WEXITSTATUS(status);
                    
                    if(job != NULL) {
                        job->status = JOB_STATUS_DONE;
                        remove_job(job);
                        free(res);
                        freeAll(args,args_count+1);
                        return;
                    }   
                } else if (WIFSIGNALED(status)) {
                    dprintf(STDERR_FILENO,"Signal reçu \n");
                    if(job != NULL) {
                        job->status = JOB_STATUS_KILLED;
                    }
                } else if (WIFSTOPPED(status)) {
                    //printf(" je suis stoppé\n");
                    if(job != NULL) {
                        job->status = JOB_STATUS_STOPPED;
                    }
                }
                else if(WIFCONTINUED(status)) {
                    if(job != NULL) {
                        job->status = JOB_STATUS_RUNNING;
                    }
                }
                print_one_job(STDERR_FILENO,job);
            }
            free(res);
        }
        else{
            free(res);
            print_one_job(STDERR_FILENO,job);
        }
    }
    freeAll(args,args_count+1);
}

void signal_f(){
    struct sigaction act = {0};
    act.sa_handler = SIG_DFL;  // Restaure le gestionnaire par défaut

    // Restaure le gestionnaire par défaut pour SIGTSTP
    if (sigaction(SIGTSTP, &act, NULL) == -1) {
        perror("sigaction(SIGTSTP)");
        exit(EXIT_FAILURE);
    }

    // Restaure le gestionnaire par défaut pour SIGCHLD
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        perror("sigaction(SIGCHLD)");
        exit(EXIT_FAILURE);
    }  
}

void printf_r(char ** args){
    int i=0;
    while(args[i] != NULL && (strcmp(args[i],"&") != 0)){
        printf("%s", args[i]);
        i++;
    }
    printf("\n");
}

char* concatenate_arguments(char *args[]) {
     // Initialiser la taille de la chaîne résultante
    size_t total_length = 0;

    // Calculer la taille totale nécessaire pour la chaîne résultante
    int i;
    for (i = 0; args[i] != NULL && strcmp(args[i], "&") != 0; ++i) {
        total_length += strlen(args[i]) + 1; // +1 pour l'espace entre les arguments
    }

    // Allouer de la mémoire pour la chaîne résultante
    char *result = (char *)malloc(total_length + 1);
    if (result == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Concaténer les arguments dans la chaîne résultante
    result[0] = '\0'; // Assurer que la chaîne résultante est initialement vide
    for (int j = 0; j < i; ++j) {
        strcat(result, args[j]);
        if (j < i - 1 && strcmp(args[j + 1], "&") != 0) {
            strcat(result, " ");
        }
    }

    return result;
}

void restore_default_signals() {
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("signal(SIGINT)");
    }

    if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {
        perror("signal(SIGTERM)");
    }

    if (signal(SIGTTIN, SIG_DFL) == SIG_ERR) {
        perror("signal(SIGTTIN)");
    }

    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
        perror("signal(SIGQUIT)");
    }

    if (signal(SIGTTOU, SIG_DFL) == SIG_ERR) {
        perror("signal(SIGTTOU)");
    }

    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
        perror("signal(SIGTSTP)");
    }
}


