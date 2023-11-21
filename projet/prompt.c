#include "prompt.h"
#include "command_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_PROMPT_LEN 30

void display() {
    long size = pathconf(".", _PC_PATH_MAX);

    if ((size == -1) && (errno == 0)) {size = 4096;}
    else if (size == -1) {
        perror("Erreur lors de la récupération de la taille du chemin");
        return;
    }

    char *buffer = malloc((size_t)size);
    if (buffer == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        return;
    }

    if (getcwd(buffer, (size_t)size) != NULL) {
        size_t buffer_length = strlen(buffer);

        if (buffer_length >= 30) {
            char* last_30_characters = buffer + (buffer_length - 30);
            printf("...\033[34m%s\033[00m $  ", last_30_characters);
        } else {
            printf("\033[34m%s\033[00m $  ",buffer);
        }
    } else {
        perror("Erreur lors de la récupération du répertoire courant");
    }
    free(buffer);
}

void update_prompt() {

    char *prompt = malloc(MAX_PROMPT_LEN);  // Alloue dynamiquement de la mémoire pour stocker le prompt
    if (prompt == NULL) {
        perror("Erreur lors de l'allocation de mémoire pour le prompt");
        return;  
    }

    printf("\033[32m [%d] \033[00m$ ", getpid());
    display();

    //rl_outstream = stderr;  // Redirige la sortie readline vers la sortie standard
    //rl_prompt = prompt;  // Affecte le prompt à la bibliothèque readline

    // Remarque : La mémoire allouée pour le prompt doit être libérée ultérieurement
    // après que readline a fini de l'utiliser. Cela peut être fait dans la fonction prompt ou ailleurs.
}

int prompt() {
    while (1) {
        update_prompt();  // Met à jour le prompt avant de lire chaque nouvelle commande

        char *input = readline("");  
        if (!input) {
            break;  
        }
        add_history(input);  // Ajoute la commande à l'historique readline

        // Vérifie si la commande est une commande interne (pwd, cd, ?, exit)
        if (strcmp(input, "pwd") == 0 || 
            strncmp(input, "cd", 2) == 0 || 
            strcmp(input, "?") == 0 || 
            strcmp(input, "exit") == 0) {
            execute_internal_command(input); 
            free(input);  // Libère la mémoire rl_outstreamallouée pour la ligne de commande lue
         } 
         else {   
            fprintf(stderr, "Commande inconnue : %s\n", input);  // Affiche un message d'erreur pour les commandes inconnues
            free(input);  // Libère la mémoire allouée pour la ligne de commande lue
        }
    }

    return 0;
}