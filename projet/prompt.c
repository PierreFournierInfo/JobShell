#include "prompt.h"
#include "command_parser.h"
#include "command_executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_PROMPT_LEN 30

char* display() {
    long size = pathconf(".", _PC_PATH_MAX);
    if ((size == -1) && (errno == 0)) { size = 4096;}
    else if (size == -1) { perror("Erreur lors de la récupération de la taille du chemin"); return NULL;}

    char *buffer = malloc((size_t)size);
    if (buffer == NULL) { perror("Erreur lors de l'allocation de mémoire"); return NULL; }

    if (getcwd(buffer, (size_t)size) != NULL) {
        size_t buffer_length = strlen(buffer);

        if (buffer_length >= 30) {
            char* point = "...";
            char* last_30_characters = buffer + (buffer_length - 30);
            int l = strlen(point)+strlen(last_30_characters) + 1;
            char* res = (char*) malloc(l);
             if (res != NULL) {
                snprintf(res, l, "%s%s", point, last_30_characters);
            }  
            return res;
        } else {
            return buffer;
        }
    } 
    else {  free(buffer); perror("Erreur lors de la récupération du répertoire courant"); return NULL; }
}

char* update_prompt() {
    char* p = "\033[32m [0]\033[00m";
    char* d = display();
    char*fin="$ ";
    if(d == NULL) perror("Erreur dans prompt update");
    int i = strlen(p) + strlen(d) +strlen(fin)+ 1;
    char* res = (char*) malloc(i);

    if (res != NULL) {
        // Utiliser snprintf pour concaténer les chaînes
        snprintf(res, i, "%s%s%s", p, d, fin);
    }    
    return res;
}


int prompt() {
    rl_outstream = stderr;
    while (1) {
        char* prompt = update_prompt();
        char *input = readline(prompt);  
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
            printf("erreur \n");
            free(input);  // Libère la mémoire allouée pour la ligne de commande lue
        }
    }
    return 0;
}