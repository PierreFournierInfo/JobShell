
#include <signal.h>
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
    } else if (pid == 0) {
         // Processus fils
        if (background) {
            //signal_f();
        }

        valeur_de_retour = execvp(command, args);
        
        perror("JSH ");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent
        if (!background) {
            // Attendre le processus fils si ce n'est pas un processus en arrière-plan
            int status;
            waitpid(pid, &status, 0);

            // Mise à jour de l'état du job
            if (WIFEXITED(status)) {
                valeur_de_retour = WEXITSTATUS(status);
                update_job_status(pid, WEXITSTATUS(status));
            } else if (WIFSTOPPED(status)) {
                create_job(pid, command);
                valeur_de_retour = WEXITSTATUS(status);
                update_job_status(pid, -1);
            } else if (WIFSIGNALED(status)) {
                valeur_de_retour = WEXITSTATUS(status);
                update_job_status(pid, WTERMSIG(status));
            } else {
                perror("Le processus fils ne s'est pas terminé normalement.\n");
            }
        } else {
            //int status;
            //waitpid(pid, &status, WNOHANG);
            // Processus en arrière-plan, ne pas attendre
            create_job(pid, command);   
            //printf("[%d] %s en arrière-plan\n", pid, command);
        }
    }
}

void signal_f(){
    struct sigaction act = {0};
    act.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &act, NULL);
}



