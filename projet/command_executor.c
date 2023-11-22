#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void execute_command(char *command, char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processus fils
        execvp(command, args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent
        int status;
        waitpid(pid, &status, 0); // status sert à vérifier le resultat de l'état de processus du fils , 0 est l'option de comportement 
        // on utilise en général 0 pour une attente normal 

        /*if (WIFEXITED(status)) {
            printf("... exécution terminée\n");
            exit(0);
        } else {
            printf("... échec de l'exécution\n");
        }*/
    }
}
