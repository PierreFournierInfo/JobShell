#include "command_parser.h"

#define PATH_MAX_ 512

int valeur_de_retour = 0;
char chemin[1024];
char ancien[1024];

// Fonction pour vérifier l'existence d'un répertoire
int directory_exists(char *path) {
    struct stat dir_stat;
    return stat(path, &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode);
}

void execute_cd(char *command) {
    // Récupère l'argument de la commande cd (le chemin du répertoire)
    char *arg = strchr(command, ' '); // Supposant que la commande est "cd chemin"
    
    // Si l'argument n'est pas fourni, affiche un message d'erreur
    if (arg == NULL || strlen(arg) <= 1) {
        dprintf(STDERR_FILENO, "Usage: cd [directory]\n");
        return;
    }

    arg++; // Ignore l'espace après "cd"
    // Vérifie si le répertoire existe
    if (directory_exists(arg)) {
        // Change le répertoire de travail
        if (chdir(arg) != 0) {
            perror("chdir");
        }
    } else {
        dprintf(STDERR_FILENO, "Directory '%s' does not exist. \n ", arg);
    }
}

// Fonction pour récupérer la suite en ignorant les espaces
char* getSuite(char *chaine) {
    while (*chaine != '\0' && *chaine == ' ') {
        chaine++; // Ignorer les espaces
    }
    return chaine;
}


void execute_internal_command(char *command) {

    // Vérifie si la commande est "pwd"
    if (strcmp(command, "pwd") == 0) {
        //printf("test pwd 2 \n");
        // Variable pour stocker le répertoire de travail courant
        char current_dir[PATH_MAX_];
        
        // Obtient le répertoire de travail courant
        if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
            printf("%s\n", current_dir);
        } else {
            goto error;
            perror("getcwd");
        }
    } 
    else if (strncmp(command, "cd",2) == 0) {
        // Obtient le chemin du répertoire personnel de l'utilisateur
        const char *home_dir = getenv("HOME");
        // Si on a encore un chemin à la suite 
        if(command[2] == '\0'){
            // Vérifie si le chemin est obtenu avec succès
            if (home_dir != NULL) {
                if (!(getcwd(chemin, sizeof(chemin)) != NULL)) {perror("getcwd didn't work 1");} 
                // Change le répertoire de travail vers le répertoire personnel de l'utilisateur
                if (chdir(home_dir) != 0) {
                    goto error;
                    // En cas d'erreur lors du changement de répertoire
                    perror("chdir");
                }
            } else {
                // Si le répertoire personnel n'est pas défini dans l'environnement
                dprintf(STDERR_FILENO, "HOME not set.\n");
            }
        }
        else{
        // Si on a des élément à la suite de la chaine
        char* suite = getSuite(command+3);
        if(strcmp(suite,"-") == 0){ 
            getcwd(ancien,sizeof(ancien));
             
            if (chdir(chemin) != 0) {
                perror("Erreur lors du changement de répertoire 1");
                goto error;
            }
            strcpy(chemin,ancien);
        }
        else{
            if (!(getcwd(chemin, sizeof(chemin)) != NULL)) {perror("getcwd didn't work 2");} 
            if (chdir(suite) != 0) {
                perror("Erreur lors du changement de répertoire 2");
                goto error;
            }
        }
      }
    } 
    else if (strcmp(command, "?") == 0) {
        printf("%d\n" , valeur_de_retour);
        valeur_de_retour = 0;
    } 
    else if (strncmp(command, "exit",4) == 0) {
        // Vérifier les jobs en cour si il y a un souci (pour plus tard)
        if(empty_jobs()) {
            
            if((command[4]=='\0')) {
                free_jobs();
                free(command);
                exit(valeur_de_retour);}
            else {
                char* suite = getSuite(command+5);
                int val = atoi(suite); 
                free_jobs();
                free(command);
                exit(val);
            }
        }
        else{
            dprintf(STDERR_FILENO,"exit\n");
            dprintf(STDERR_FILENO,"There are stopped jobs.\n");
            //free(command);
            valeur_de_retour = 1;
        }
    }
    error : if(errno){
        valeur_de_retour = 1;
    }
}

bool is_internal_command( char *command) {
    // Ajoutez ici les autres commandes internes, si nécessaire
    return strcmp(command, "pwd") == 0 ||
           strncmp(command, "cd", 2) == 0 ||
           strcmp(command, "?") == 0 ||
           strncmp(command, "exit", 4) == 0;
}

bool is_exit_command( char *command) {
    return strncmp(command, "exit", 4) == 0;
}
