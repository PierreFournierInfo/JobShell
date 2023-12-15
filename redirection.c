#include "redirection.h"
#include "prompt.h"

size_t tailleTableauChar(char **tableau) {
   size_t taille = 0;
    while ((tableau[taille] != NULL) && (
        strcmp(tableau[taille], "<") != 0 &&
        strcmp(tableau[taille], ">") != 0 &&
        strcmp(tableau[taille], ">|") != 0 &&
        strcmp(tableau[taille], ">>") != 0 &&
        strcmp(tableau[taille], "2>") != 0 &&
        strcmp(tableau[taille], "2>|") != 0 &&
        strcmp(tableau[taille], "2>>") != 0
    )) {
        taille++;
    }
    return taille;
}

char** before_com(char **res){
    size_t taille = tailleTableauChar(res);
    //printf("%ld \n", taille);
    int ind=0;
    char** t = malloc(sizeof (taille+1));
    for(size_t i=0; i < taille+1 ;i++){
        if(strcmp(res[i],"<")==0 ||
            strcmp(res[i],">")==0 ||
            strcmp(res[i],">|")==0 ||
            strcmp(res[i],">>")==0 ||
            strcmp(res[i],"2>")==0 ||
            strcmp(res[i],"2>|")==0 ||
            strcmp(res[i],"2>>")==0){
                break;
        }
        //printf("%s\n",res[i]);
        ind++;
        t[i]=res[i];
    }
    //printf("%d\n", ind);
    t[ind]=NULL;
    //free(res);
    return t;
}

bool verif_fic(const char *fic){
    struct stat file_stat;
    return stat(fic, &file_stat) == 0 && S_ISREG(file_stat.st_mode);
}

void command_r(char** res,int taille){
    if(strcmp(res[0], "pwd") == 0 || strncmp(res[0], "cd", 2) == 0 || strcmp(res[0], "?") == 0 ||  strncmp(res[0], "exit",4) == 0) {
        // Les pipes utiles pour les redirection 
        int pipefd[2];
        pid_t child_pid;
       
        if (pipe(pipefd) != 0) { perror("Erreur au pipe"); exit(EXIT_FAILURE);}

        // Creation du fils pour executer la commande à gauche 
        child_pid = fork();

        if (child_pid < 0) {
            perror("Erreur fork");
            exit(EXIT_FAILURE);
        } 
        
        if (child_pid == 0)    {  // Code du processus enfant
        redirect(res,pipefd,taille); 
  
        // Exécuter la commande interne dans le processus enfant
        execute_internal_command(res[0]);

        // Si la commande est "exit", terminer le processus enfant avec exit
        if (strncmp(res[0], "exit", 4) == 0) {
            exit(EXIT_SUCCESS);
        }

        // Sinon, le processus enfant se termine sans exit
        exit(EXIT_FAILURE);
    }
        else {  // Code du processus parent
            close(pipefd[1]);  // Fermer la fin inutilisée du pipe en écriture 

            // Attendre la fin de l'exécution du processus enfant
            int status = 0;
            waitpid(child_pid, &status, 0);
            
            // Vérifier si le processus fils s'est terminé normalement
            if (WIFEXITED(status)) {
            // Vérifier le code de sortie du processus fils
            int exit_status = WEXITSTATUS(status);
            //valeur_de_retour = exit_status;
            // Vous pouvez utiliser exit_status pour savoir si la commande était "exit"
            if (exit_status == EXIT_SUCCESS) {
                //printf("Le processus fils a exécuté la commande 'exit'\n");
                exit(valeur_de_retour);
            } else {
                //printf("Le processus fils s'est terminé sans exécuter la commande 'exit'\n");
            }
        }

            // Réinitialiser l'entrée standard pour permettre l'interaction utilisateur
            if (dup2(STDIN_FILENO, 0) == -1) {
                perror("Erreur lors de la réinitialisation de l'entrée standard");
                exit(EXIT_FAILURE);
            }
        }
    }
    else{
        //printf(" Else commandd _ r\n ");
        int pipefd[2];
        pid_t child_pid;
        if (pipe(pipefd) != 0) { perror("Erreur au pipe"); exit(EXIT_FAILURE);}

        // Creation du fils pour executer la commande à gauche 
        child_pid = fork();
        if (child_pid < 0) { perror("Erreur fork");  exit(EXIT_FAILURE);}

        if (child_pid == 0)    {  // Code du processus enfant
        //afficherTableauChar(res);
        redirect(res,pipefd,taille); 
  
        // Exécuter la commande interne dans le processus enfant
        //printf("test commande");
        char** utilisation = before_com(res);
        
        execute_command(utilisation[0],utilisation);
        exit(EXIT_SUCCESS);
        }
        else {  // Code du processus parent
            close(pipefd[1]);  // Fermer la fin inutilisée du pipe en écriture 

            // Attendre la fin de l'exécution du processus enfant
            int status;
            waitpid(child_pid, &status, 0);
           
           // Vérifier si le processus fils s'est terminé normalement
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);

                // Vérifier le code de sortie du processus fils
                if (exit_status == EXIT_SUCCESS) {
                    // Le processus fils s'est terminé normalement
                } else {
                    // Le processus fils s'est terminé avec une erreur
                    valeur_de_retour = 1;
                }
            } else {
                // Le processus fils ne s'est pas terminé normalement
                if (WIFSIGNALED(status)) {
                    printf("Le processus fils a été tué par le signal : %d\n", WTERMSIG(status));
                }
            }

            // Réinitialiser l'entrée standard pour permettre l'interaction utilisateur
            if (dup2(STDIN_FILENO, 0) == -1) {
                perror("Erreur lors de la réinitialisation de l'entrée standard");
                exit(EXIT_FAILURE);
            }
        }

    }
}

/*------------------------- GESTION PRINCIPAL DES REDIRECTIONS ---------------------------------*/

void redirect(char** res, int* pipefd , int taille){
       
    close(pipefd[0]);  // Fermer la fin inutilisée du pipe

    for (int i = 0; i < taille; i++) {
        //printf("%s %s \n",res[i],res[i+1]);
        
        if (strcmp(res[i], "<") == 0) {
            //printf("%s \n",res[i+1]);
            if (verif_fic(res[i + 1]) == true) {
                    
                int fd = open(res[i + 1], O_RDONLY);
                //printf("test 2");
                if (fd < 0) {
                    perror("JSH : Erreur lors de l'ouverture du fichier");
                    valeur_de_retour=1;
                    exit(EXIT_FAILURE);
                }

                // Rediriger l'entrée standard vers le fichier
                int fd2 = dup2(fd, STDIN_FILENO);
                if (fd2 == -1) {
                    perror("Erreur lors de la redirection de l'entrée standard");
                    valeur_de_retour=1;
                    exit(EXIT_FAILURE);
                }

                close(fd);
                break;  // Fermer le descripteur de fichier supplémentaire
            } else {
                perror(strcat(res[i + 1], ": Aucun fichier ou dossier de ce type"));
                valeur_de_retour=1;
                exit(EXIT_FAILURE);
            }
        }

        else if(strcmp(res[i], ">") == 0){
                // Ouvrir ou créer le fichier en écriture seulement, en ajoutant au contenu existant
                //printf("%s 1\n",res[i+1]);
                int fd = open(res[i+1],O_WRONLY | O_CREAT | O_EXCL |  O_APPEND, 0666);
                if (fd == -1) {
                   // printf( "Erreur numéro %d \n", errno);

                    perror("bash: sortie: cannot overwrite existing file");
                    exit(EXIT_FAILURE);
                }

                // Rediriger la sortie erreur vers le fichier
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Erreur lors de la redirection de la sortie standard");
                    valeur_de_retour=1;
                    exit(EXIT_FAILURE);
                }

                // Fermer le descripteur de fichier supplémentaire
                close(fd);
                break;
        }
        else if(strcmp(res[i], ">|")==0){
                //printf("%s 2\n",res[i+1]);
                // Ouvrir ou créer le fichier en écriture seulement, en ajoutant au contenu existant
                int fd = open(res[i+1], O_WRONLY | O_CREAT |O_TRUNC, 0666);

                if (fd == -1) {
                    perror("Erreur lors de l'ouverture ou de la création du fichier");
                    exit(EXIT_FAILURE);
                }

                // Rediriger la sortie standard vers le fichier
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Erreur lors de la redirection de la sortie standard");
                    exit(EXIT_FAILURE);
                }

                // Fermer le descripteur de fichier supplémentaire
                close(fd);
                break;
        }
        else if(strcmp(res[i], ">>")==0){
            // Ouvrir ou créer le fichier en écriture seulement, en concaténant le contenu existant
            //printf("%s 4\n",res[i+1]);
            int fd = open(res[i+1], O_WRONLY | O_CREAT | O_APPEND, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                exit(EXIT_FAILURE);
            }

            // Rediriger la sortie standard vers le fichier
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
            break;
        }
        else if(strcmp(res[i], "2>") == 0){
                 // Ouvrir ou créer le fichier en écriture seulement, en ajoutant au contenu existant
                //printf("%s 6\n",res[i+1]);
                int fd = open(res[i+1], O_WRONLY | O_CREAT | O_APPEND | O_EXCL, 0666);

                if (fd == -1) {
                    perror("Erreur lors de l'ouverture ou de la création du fichier");
                    exit(EXIT_FAILURE);
                }

                // Rediriger la sortie erreur vers le fichier
                if (dup2(fd, STDERR_FILENO) == -1) {
                    perror("Erreur lors de la redirection de la sortie standard");
                    exit(EXIT_FAILURE);
                }

                // Fermer le descripteur de fichier supplémentaire
                close(fd);
                break;
        }
        
        else if(strcmp(res[i], "2>>") == 0){
            // Ouvrir ou créer le fichier en écriture seulement, en écrasant le contenu existant 
            //printf("%s 7\n",res[i+1]);
            int fd = open(res[i+1], O_WRONLY | O_CREAT | O_APPEND, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                exit(EXIT_FAILURE);
            }

            // Rediriger la sortie erreur standard vers le fichier
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
            break;
        }
        
        else if(strcmp(res[i], "2>|") == 0){
            // Ouvrir ou créer le fichier en écriture seulement, en concaténant le contenu existant
            //printf("%s 8\n",res[i+1]);
            int fd = open(res[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                exit(EXIT_FAILURE);
            }

            // Rediriger la sortie erreur standard vers le fichier
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
            break;
        }
    }
    //printf("FIN \n");
}
