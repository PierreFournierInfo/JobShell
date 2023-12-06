#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "command_parser.h"
#include "job_manager.h"

void execute_command(char *command, char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processus fils
        valeur_de_retour = execvp(command, args); // execution de commande sans avoir besoin du path
        perror("JSH ");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent
        int status;
        waitpid(pid, &status, 0); // status sert à vérifier le resultat de l'état de processus du fils , 0 est l'option de comportement 
        
        //Rappel : on utilise en général 0 pour une attente normal 

        // Créer un job avec les informations nécessaires
        create_job(pid, command);

         // Mise à jour de l'état du job
        if (WIFEXITED(status)) {
            valeur_de_retour = WEXITSTATUS(status);
            update_job_status(pid, WEXITSTATUS(status));
        } else if (WIFSTOPPED(status)) {
            valeur_de_retour = WEXITSTATUS(status);
            update_job_status(pid, -1); // Vous pouvez utiliser -1 ou un autre code pour les états arrêtés
        } else if (WIFSIGNALED(status)) {
            valeur_de_retour = WEXITSTATUS(status);
            update_job_status(pid, WTERMSIG(status));
        }
        else {
             // Le processus fils ne s'est pas terminé normalement
            perror("Le processus fils ne s'est pas terminé normalement.\n");

        }
    }
}

