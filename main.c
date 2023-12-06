
#include "command_parser.h"
#include "prompt.h"
#include "command_executor.h"
#include "prompt.h"
#include "job_manager.h"
#include"redirection.h"

int main() {

 while (1) {
        char* prompt = update_prompt();
        rl_outstream = stderr;
        char *input = readline(prompt);
        free(prompt);  
        if (!input || input == NULL) {
            printf(" test break ma gueule ");
            exit(valeur_de_retour);
            break;  
        }

        add_history(input);  // Ajoute à la commande à l'historique readline
         if(redirection_verif(input)){  // vérification de la possibilité de redirection 
            int taille=0;
            char** res = separerParEspaces(input,&taille);
            command_r(res);
            freeAll(res,taille);
            free(input); 
        }
        // Vérifie si la commande est une commande interne (pwd, cd, ?, exit)
        else if(strcmp(input, "pwd") == 0 || 
            strncmp(input, "cd", 2) == 0 || 
            strcmp(input, "?") == 0 || 
            strncmp(input, "exit",4) == 0) {
            execute_internal_command(input); 
            free(input);  // Libère la mémoire rl_outstreamallouée pour la ligne de commande lue
         } 
         else { 
                int taille=0;
                char** res = separerParEspaces(input,&taille);
            
                // Affichage des résultats tests 
                if(strlen(input)>0) execute_command(res[0],res);
                freeAll(res,taille);
                free(input);  // Libère la mémoire allouée pour la ligne de commande lue
        }
    }
    return 0;
}
