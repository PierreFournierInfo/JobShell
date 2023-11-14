
#include "command_parser.h"

int main() {
    const char *user_command = "pwd";
    
    if (is_internal_command(user_command)) {
        execute_internal_command(user_command);
    } else {
        // Traitement des commandes externes
    }
   /*char user_input[256]; // Buffer pour stocker l'entrée utilisateur

    while (1) {
        // Affiche l'invite de commande
        printf("Enter command (pwd, cd, ?, exit): ");
        
        // Obtient l'entrée de l'utilisateur
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            // Si l'utilisateur a saisi CTRL+D (fin de fichier)
            printf("\nExiting shell.\n");
            break;
        }

        // Supprime le saut de ligne ajouté par fgets
        user_input[strcspn(user_input, "\n")] = '\0';

        // Vérifie la commande saisie et exécute la fonction appropriée
        if (strcmp(user_input, "pwd") == 0 || 
            strcmp(user_input, "cd") == 0 || 
            strcmp(user_input, "?") == 0 || 
            strcmp(user_input, "exit") == 0) {
            execute_internal_command(user_input);
        } else {
            // Message d'erreur pour les commandes non reconnues
            fprintf(stderr, "Unknown command: %s\n", user_input);
        }
    }

    return 0;*/
    return 0;
}
