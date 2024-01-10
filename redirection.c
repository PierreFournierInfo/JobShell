#include "redirection.h"
#include "prompt.h"
#include "signal_handler.h"

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
    char** t = malloc(sizeof(char*) * (taille + 1));

    if (t == NULL) {
        perror("Erreur d'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    int ind = 0;

    for(size_t i = 0; i < taille + 1; i++){
        if(strcmp(res[i], "<") == 0 ||
            strcmp(res[i], ">") == 0 ||
            strcmp(res[i], ">|") == 0 ||
            strcmp(res[i], ">>") == 0 ||
            strcmp(res[i], "2>") == 0 ||
            strcmp(res[i], "2>|") == 0 ||
            strcmp(res[i], "2>>") == 0){
                break;
        }
        ind++;
        t[i] = res[i];
    }
    t[ind] = NULL;

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
    
            execute_internal_command(res[0]);

            if (strncmp(res[0], "exit", 4) == 0) {
                exit(EXIT_SUCCESS);
            }

            exit(EXIT_FAILURE);
        }
        else {  // Code du processus parent
            close(pipefd[1]);  // Fermer la fin inutilisée du pipe en écriture 

            // Attendre la fin de l'exécution du processus enfant
            int status = 0;
            waitpid(child_pid, &status, 0);
            
            if (WIFEXITED(status)) {
                // Vérifier le code de sortie du processus fils
                int exit_status = WEXITSTATUS(status);
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
        //printf(" Else commande externe \n ");
        int pipefd[2];
        pid_t child_pid;
        if (pipe(pipefd) != 0) { perror("Erreur au pipe"); exit(EXIT_FAILURE);}

        child_pid = fork();
        if (child_pid < 0) { perror("Erreur fork");  exit(EXIT_FAILURE);}

        
        if (child_pid == 0)    {  // Code du processus enfant
            //afficherTableauChar(res);
            redirect(res,pipefd,taille); 
    
            char** utilisation = before_com(res);
         
            execute_command(utilisation[0],utilisation);
            
            exit(EXIT_SUCCESS);
        }
        else { 
             // Code du processus parent
            close(pipefd[1]);
            // Attendre la fin de l'exécution du processus enfant
            int status;
            waitpid(child_pid, &status, 0);
           
           // Vérifier si le processus fils s'est terminé normalement
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);

                // Vérifier le code de sortie du processus fils
                if (exit_status == EXIT_SUCCESS) {
                } else {
                    // Le processus fils s'est terminé avec une erreur
                    valeur_de_retour = 1;
                }
            } else {
                // Le processus fils ne s'est pas terminé normalement
                if (WIFSIGNALED(status)) {
                    //printf("Le processus fils a été tué par le signal : %d\n", WTERMSIG(status));
                }
            }

            // Réinitialiser l'entrée standard pour permettre l'interaction utilisateur
            // Restaurer les descripteurs de fichiers standard
            if (dup2(STDIN_FILENO, 0) == -1 || dup2(STDOUT_FILENO, 1) == -1 || dup2(STDERR_FILENO, 2) == -1) {
                perror("Erreur lors de la réinitialisation des descripteurs de fichiers standard");
                exit(EXIT_FAILURE);
            }
        }

    }
}

/*------------------------- GESTION PRINCIPAL DES REDIRECTIONS ---------------------------------*/
void redirect(char** res, int* pipefd, int taille) {
    close(pipefd[0]);  // Fermer la fin inutilisée du pipe

    for (int i = 0; i < taille - 1; i++) {
        if (strcmp(res[i], "<") == 0) {
            if (verif_fic(res[i + 1]) == true) {
                int fd = open(res[i + 1], O_RDONLY);
                if (fd < 0) {
                    perror("JSH : Erreur lors de l'ouverture du fichier");
                    valeur_de_retour = 1;
                    exit(EXIT_FAILURE);
                }

                // Rediriger l'entrée standard vers le fichier
                int fd2 = dup2(fd, STDIN_FILENO);
                if (fd2 == -1) {
                    perror("Erreur lors de la redirection de l'entrée standard");
                    valeur_de_retour = 1;
                    exit(EXIT_FAILURE);
                }

                close(fd);
            } else {
                perror(strcat(res[i + 1], ": Aucun fichier ou dossier de ce type"));
                valeur_de_retour = 1;
                exit(EXIT_FAILURE);
            }

        } else if (strcmp(res[i], ">") == 0 || strcmp(res[i], ">>") == 0 || strcmp(res[i], "2>") == 0 || strcmp(res[i], "2>>") == 0 || strcmp(res[i], ">|") == 0 || strcmp(res[i], "2>|") == 0) {
            int flags;
            int mode;

            if (strcmp(res[i], ">") == 0) {
                flags = O_WRONLY | O_CREAT | O_EXCL | O_TRUNC;
                mode = 0666;
            } else if (strcmp(res[i], ">>") == 0 || strcmp(res[i], "2>>") == 0) {
                flags = O_WRONLY | O_CREAT | O_APPEND;
                mode = 0666;
            } else if (strcmp(res[i], ">|") == 0 || strcmp(res[i], "2>|") == 0) {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                mode = 0666;
            } else {  // res[i] is "2>"
                flags = O_WRONLY | O_CREAT | O_TRUNC | O_EXCL;
                mode = 0666;
            }

            // Ouvrir ou créer le fichier en écriture avec les bons drapeaux
            int fd = open(res[i + 1], flags, mode);
            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                exit(EXIT_FAILURE);
            }

            // Rediriger la sortie vers le fichier
            int fd2;
            if (strcmp(res[i], "2>") == 0 || strcmp(res[i], "2>>") == 0 || strcmp(res[i], "2>|") == 0) {
                fd2 = dup2(fd, STDERR_FILENO);
            } else {
                fd2 = dup2(fd, STDOUT_FILENO);
            }

            if (fd2 == -1) {
                perror("Erreur lors de la redirection de la sortie standard ou erreur");
                exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
        }
    }
}


