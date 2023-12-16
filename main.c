
#include "command_parser.h"
#include "prompt.h"
#include "command_executor.h"
#include "prompt.h"
#include "job_manager.h"
#include"redirection.h"

// Normalement dans signal handler 
void ignore_signals() {
    struct sigaction act = {0};
    
    // Ignorer le signal Ctrl+C (^C)
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignorer le signal Ctrl+Z (^Z)
    if (sigaction(SIGTSTP, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

int main() {

 // Gestion pour les jobs 
 initialize_job_manager();

 // Ignorer les signaux qu'on a dit
 ignore_signals();
 
 
 while (1) {
        char* prompt = update_prompt();
        rl_outstream = stderr;
        char *input = readline(prompt);
        free(prompt);  
        if (!input || input == NULL) {
            exit(valeur_de_retour);
            break;  
        }

        add_history(input);  // Ajoute à la commande à l'historique readline
         if(redirection_verif(input)){  // vérification de la possibilité de redirection 
            int taille=0;
            char** res = separerParEspaces(input,&taille);
            //afficherTableauChar(res);
            command_r(res,taille);
            freeAll(res,taille);
            free(input); 
        }
        // Jobs sans option pour le moment 
        else if(strcmp(input,"jobs")==0){
            print_jobs();
            free(input);
        }
        //bg ou fg
        else if(strncmp(input,"bg",2)==0 || strncmp(input,"fg",2)==0){
            int taille = 0;
            char** res = separerParEspaces(input,&taille);
            //printf("bg fg :  %s \n", res[1]);
            if(strncmp(input,"bg",2)==0) bg_command(res[1]);
            else fg_command(res[1]);

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
                //freeAll(res,taille);
                free(input);  // Libère la mémoire allouée pour la ligne de commande lue
        }
    }
    return 0;
}
