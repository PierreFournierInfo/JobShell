#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "command_parser.h"

void execute_command(char *command, char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processus fils
        valeur_de_retour = execvp(command, args);
        perror("bash");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent
        int status;
        waitpid(pid, &status, 0); // status sert à vérifier le resultat de l'état de processus du fils , 0 est l'option de comportement 
        //Rappel : on utilise en général 0 pour une attente normal 

        if (WIFEXITED(status)) {
            // Le processus fils s'est terminé normalement
            valeur_de_retour = WEXITSTATUS(status);
            //printf("Le processus fils s'est terminé avec le statut : %d\n", valeurProcessusFils);
        } else {
            // Le processus fils ne s'est pas terminé normalement
            fprintf(stderr, "Le processus fils ne s'est pas terminé normalement.\n");
        }

    }
}
