#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void stringToCharArray(const char* inputString) {
    char* tab[50];
    // Vérifier que les pointeurs ne sont pas nuls
    if (inputString == NULL || tab == NULL) {
        printf("Erreur : Pointeur nul.\n");
        return;
    }

    // Copier chaque caractère dans le tableau de caractères
    int i = 0;
    while (inputString[i] != '\0') {
        tab[i] = inputString[i];
        i++;
    }

    // Ajouter le caractère nul pour terminer le tableau
    tab[i] = '\0';
}


void redirect(char* tab){
     char **mots = separerChaine(tab, delimiteur, &nombreDeMots);
        
        if(strcmp(tab[i], "<") == 0){
            int fd = open("nom_du_fichier.txt",O_CREAT|O_WRONLY);
    
            if (fd < 0) {
                perror("Erreur lors de l'ouverture du fichier");
                exit(EXIT_FAILURE);
            }

            // Rediriger l'entrée standard vers le fichier
            if(dup2(fd, STDIN_FILENO) == NULL) {
                perror("Erreur lors de la redirection de l'entrée standard");
                exit(EXIT_FAILURE);
            }
        }
        
        if(strcmp(tab[i], ">") == 0){
            // Ouvrir ou créer le fichier en écriture seulement, en ajoutant au contenu existant
            int fd = open("fic", O_WRONLY | O_CREAT | O_APPEND, 0644);

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
        }
        
        if(strcmp(tab[i], ">") == 0 && strcmp(tab[i+1], "|") == 0){

            // Ouvrir ou créer le fichier en écriture seulement, en écrasant le contenu existant
            int fd = open("fic", O_WRONLY | O_CREAT | O_TRUNC, 0644);

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
        }
        
        if(strcmp(tab[i], ">") == 0 && strcmp(tab[i+1], ">") == 0){

            // Ouvrir ou créer le fichier en écriture seulement, en concaténant le contenu existant
            int fd = open("fic", O_WRONLY | O_CREAT | O_APPEND, 0644);

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
        }
        
        if(strcmp(tab[i], "2") == 0 && strcmp(tab[i+1], ">") == 0){
        }
        
        if(strcmp(tab[i], "2") == 0 && strcmp(tab[i+1], ">") == 0 && strcmp(tab[i+2], ">") == 0){
        }
        
        if(strcmp(tab[i], "2") == 0 && strcmp(tab[i+1], ">") == 0 && strcmp(tab[i+2], "|") == 0){
        }
}
