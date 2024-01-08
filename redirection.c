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

bool redirection_verify_pipe(char** tableau,int t){
    int taille = 0;
    while (taille < t){
        //dprintf(STDOUT_FILENO,"verif : %s \n",tableau[taille]);
        if(
        strcmp(tableau[taille], "<") == 0 ||
        strcmp(tableau[taille], ">") == 0 ||
        strcmp(tableau[taille], ">|") == 0 ||
        strcmp(tableau[taille], ">>") == 0 ||
        strcmp(tableau[taille], "2>") == 0 ||
        strcmp(tableau[taille], "2>|") == 0 ||
        strcmp(tableau[taille], "2>>") == 0
        ){ return true; }
        taille++;
    }
    return false;
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
            if (dup2(STDIN_FILENO, 0) == -1) {
                perror("Erreur lors de la réinitialisation de l'entrée standard");
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



//--------------------------------------------------------------- PIPE -----------------------------------------------------------------------

void tokenizer(char *token[], char *s, char *delimParameter, int *total){    
    int index = 0;

    token[0] = strtok(s, delimParameter);
    
    while(token[index]!=NULL){
        //printf("%s %d\n", token[index],index);
        token[++index] = strtok(NULL, delimParameter);
    }

    // Returns the total no. of commands
    *total = index;

    //printf("Token end \n");
}

void command_pipe(char *command) {
    // printf(" PIPE \n", command);
    char *pipedCommand[10000];
    int numPipeCommands = 0;

    tokenizer(pipedCommand, command, "|", &numPipeCommands);

    // File descriptor
    //printf("%d \n", numPipeCommands);
    int pipes[numPipeCommands - 1][2];
    
    for (int i = 0; i < numPipeCommands; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numPipeCommands; i++) {
        int lenOfEachPipeCommand = 0;
        char *spaceSepPipedCommand[10000];

        tokenizer(spaceSepPipedCommand, pipedCommand[i], " ", &lenOfEachPipeCommand);
        
        int returnedFork = fork();
        if (returnedFork < 0) {
            printf("failed to fork\n");
        } else if (returnedFork == 0) {
            restore_default_signals();

            if (i > 0) {
                close(pipes[i - 1][1]); 
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
            }

            if (i < numPipeCommands - 1) {
                close(pipes[i][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            }
            
            // if( redirection_verify_pipe(spaceSepPipedCommand,lenOfEachPipeCommand) ) {
                
            //     dprintf(STDOUT_FILENO," Redirection \n");
            //     //afficherTableauChar(spaceSepPipedCommand);
            //     //command_r(spaceSepPipedCommand,lenOfEachPipeCommand);
            // }
            // else{
            //     dprintf(STDOUT_FILENO," Redirection faux  \n");
            // }
            // else{
                execvp(spaceSepPipedCommand[0], spaceSepPipedCommand);
           // }
            perror("Failed execvp in pipe ");
            exit(0);
        } else {
             if (i > 0) {
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
            }
        }
    }

    
    for (int i = 0; i < numPipeCommands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < numPipeCommands; i++) {
        int wstatus = 0;
        wait(&wstatus);
    }
    

    return;
}