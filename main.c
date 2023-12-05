
#include "command_parser.h"
#include "prompt.h"
#include "command_executor.h"
#include"redirection.h"

int main() {
   rl_outstream = stderr;
    while (1) {
        char* prompt = update_prompt();
        char *input = readline(prompt);
        free(prompt);  
        if (!input || input == NULL) {
            exit(valeur_de_retour);
            break;  
        }

        add_history(input);  // Ajoute < ala commande à l'historique readline
        if(redirection_verif(input)){
            int taille=0;
            char** res = separerParEspaces(input,&taille);
           redirect(res);
            char** res2=malloc(sizeof res);
            for(size_t i=0;i<tailleTableauChar(res)-1;i++){
                if(strcmp("|",res[i])==0 ||
                    strcmp("<",res[i])==0 ||
                    strcmp(">",res[i])==0) break;
                res2[i]=res[i];
            }
            // il est cense faire le pwd qu'on a écrit pas celui de executor
            if(strcmp(res[0], "pwd") == 0 || 
            strncmp(res[0], "cd", 2) == 0 || 
            strcmp(res[0], "?") == 0 || 
            strncmp(res[0], "exit",4) == 0) {  
                printf(" test  %s\n ",res[0]);
               execute_internal_command(res[0]);
                free(input);
                free(res);
                free(res2);
            }else{
                printf(" test 2");
                execute_command(res[0],res2);
                free(res2);
                free(input);
                free(res);
            }
            
        // Vérifie si la commande est une commande interne (pwd, cd, ?, exit)
        }else if(strcmp(input, "pwd") == 0 || 
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
