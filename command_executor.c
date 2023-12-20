#include <signal.h>
#include "prompt.h"
#include "command_executor.h"

void execute_command(char *command, char *args[]) {
    int background = 0;  // Variable pour indiquer s'il s'agit d'un processus en arrière-plan

   // Vérifier si le dernier argument est "&"
    size_t args_count = 0;
    while (args[args_count] != NULL) {
        args_count++;
    }

    if (args_count > 0 && strcmp(args[args_count - 1], "&") == 0) {
        background = 1;
        // Supprimer "&" de la liste d'arguments
        args[args_count - 1] = NULL;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        setpgid(0,0);
         // Processus fils
        if (background) {
            signal_f();
        }

        valeur_de_retour = execvp(command, args);
        
        perror("JSH ");
        exit(EXIT_FAILURE);
    } 
    else {
        // Processus parent
        if (!background) { // pas à l'arrière plan 
            int status;
            char* res = concatenate_arguments(args);
            create_job(pid,res,0);
            if (waitpid(pid, &status, WNOHANG) == 0) {
                // Le processus fils est toujours en cours d'exécution
                waitpid(pid, &status, 0);
                
                // Mise à jour de l'état du job
                if (WIFEXITED(status)) {
                    valeur_de_retour = WEXITSTATUS(status);
                    update_job_status(pid,JOB_STATUS_DONE);
                }
                else if (WIFSTOPPED(status)) {
                    valeur_de_retour = WEXITSTATUS(status);
                    update_job_status(pid,status);
                }
                else if (WIFSIGNALED(status)) {
                    //valeur_de_retour = W(status);
                    update_job_status(pid,status);
                } 
                else {
                    perror("Le processus fils ne s'est pas terminé normalement.\n");
                }
            }
            free(res);
        } 
        else {
            // Processus en arrière-plan, ne pas attendre
            char* res = concatenate_arguments(args);
            create_job(pid, res,1);   
       }
        freeAll(args,args_count+1);
    }
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



