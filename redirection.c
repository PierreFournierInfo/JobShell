#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

size_t tailleTableauChar(char **tableau) {
    size_t taille = 0;

    while (tableau[taille] != NULL) {
        taille++;
    }

    return taille;
}
bool verif_fic(const char *fic){
        struct stat file_stat;
    return stat(fic, &file_stat) == 0 && S_ISREG(file_stat.st_mode);
}

void redirect(char** tab){
    for(size_t i=0;i<tailleTableauChar(tab)-1;i++){

        if(strcmp(tab[i], "<") == 0){
            if(verif_fic(tab[i+1])==true){
                int fd = open(tab[i+1],O_RDONLY);
    
                if (fd < 0) {
                    perror("Erreur lors de l'ouverture du fichier");
                    //exit(EXIT_FAILURE);
                }
                // Rediriger l'entrée standard vers le fichier
                if(dup2(fd, STDIN_FILENO) == -1) {
                    perror("Erreur lors de la redirection de l'entrée standard");
                   // exit(EXIT_FAILURE);
                }
                printf("Fin");
                //exit(EXIT_SUCCESS);
            }else{
               perror(strcat(tab[i+1],": Aucun fichier ou dossier de ce type"));
            }
            
        }
        
        if((strcmp(tab[i], ">") == 0 && strcmp(tab[i+1], ">") != 0 )||
            (strcmp(tab[i], ">") == 0 && strcmp(tab[i+1], "|") != 0)){
            
                // Ouvrir ou créer le fichier en écriture seulement, en ajoutant au contenu existant
                int fd = open(tab[i+1], O_WRONLY | O_CREAT | O_APPEND|O_EXCL, 0666);

                if (fd == -1) {
                    perror("Erreur lors de l'ouverture ou de la création du fichier");
                    //exit(EXIT_FAILURE);
                }

                // Rediriger la sortie standard vers le fichier
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Erreur lors de la redirection de la sortie standard");
                    //exit(EXIT_FAILURE);
                }

                // Fermer le descripteur de fichier supplémentaire
                close(fd);
        }
        
        if(strcmp(tab[i], ">") == 0 && strcmp(tab[i+1], "|") == 0){

            // Ouvrir ou créer le fichier en écriture seulement, en écrasant le contenu existant 
            int fd = open(tab[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                //exit(EXIT_FAILURE);
            }

            // Rediriger la sortie standard vers le fichier
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                //exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
        }
        
        if(strcmp(tab[i], ">") == 0 && strcmp(tab[i+1], ">") == 0){

            // Ouvrir ou créer le fichier en écriture seulement, en concaténant le contenu existant
            int fd = open(tab[i+2], O_WRONLY | O_CREAT | O_APPEND, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                //exit(EXIT_FAILURE);
            }

            // Rediriger la sortie standard vers le fichier
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                //exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
        }
        
        if(strcmp(tab[i], "2") == 0 && strcmp(tab[i+1], ">") == 0){
            if(verif_fic(tab[i+1])==false){
                 // Ouvrir ou créer le fichier en écriture seulement, en ajoutant au contenu existant
                int fd = open(tab[i+2], O_WRONLY | O_CREAT | O_APPEND, 0666);

                if (fd == -1) {
                    perror("Erreur lors de l'ouverture ou de la création du fichier");
                    // exit(EXIT_FAILURE);
                }

                // Rediriger la sortie erreur vers le fichier
                if (dup2(fd, STDERR_FILENO) == -1) {
                    perror("Erreur lors de la redirection de la sortie standard");
                    // exit(EXIT_FAILURE);
                }

            }else{
                perror("Erreur le fichier existe deja");
                // exit(EXIT_FAILURE);
            }
        }
        
        if(strcmp(tab[i], "2") == 0 && strcmp(tab[i+1], ">") == 0 && strcmp(tab[i+2], ">") == 0){
            // Ouvrir ou créer le fichier en écriture seulement, en écrasant le contenu existant 
            int fd = open(tab[i+3], O_WRONLY | O_CREAT | O_TRUNC, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                // exit(EXIT_FAILURE);
            }

            // Rediriger la sortie erreur standard vers le fichier
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                // exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
        }
        
        if(strcmp(tab[i], "2") == 0 && strcmp(tab[i+1], ">") == 0 && strcmp(tab[i+2], "|") == 0){
            // Ouvrir ou créer le fichier en écriture seulement, en concaténant le contenu existant
            int fd = open(tab[i+3], O_WRONLY | O_CREAT | O_APPEND, 0666);

            if (fd == -1) {
                perror("Erreur lors de l'ouverture ou de la création du fichier");
                // exit(EXIT_FAILURE);
            }

            // Rediriger la sortie erreur standard vers le fichier
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                // exit(EXIT_FAILURE);
            }

            // Fermer le descripteur de fichier supplémentaire
            close(fd);
        }
    }
}
