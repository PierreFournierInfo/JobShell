#include "prompt.h"
#include "command_parser.h"
#include "command_executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_PROMPT_LEN 30

char* display() {
    long size = pathconf(".", _PC_PATH_MAX);
    if ((size == -1) && (errno == 0)) { size = 4096;}
    else if (size == -1) { perror("Erreur lors de la récupération de la taille du chemin"); return NULL;}

    char *buffer = malloc((size_t)size);
    if (buffer == NULL) { perror("Erreur lors de l'allocation de mémoire"); return NULL; }

    if (getcwd(buffer, (size_t)size) != NULL) {
        size_t buffer_length = strlen(buffer);

        if (buffer_length >= 30) {
            char* point = "...";
            char* last_22_characters = buffer + (buffer_length - 22); // on compte la taille max avec les crochets et le $
            int l = strlen(point)+strlen(last_22_characters) + 1;
            char* res = (char*) malloc(l);
            
            if (res != NULL) {
                snprintf(res, l, "%s%s", point, last_22_characters);
            }  
            free(buffer);
            return res;
        } else {
            return buffer;
        }
    } 
    else {  free(buffer); perror("Erreur lors de la récupération du répertoire courant"); return NULL; }
}

char* update_prompt() {
    int x=0;
    char* p = "\033[32m[%d]\033[00m";
    char* d = display();
    char*fin="$ ";
    if(d == NULL) perror("Erreur dans prompt update");
    int i = strlen(p) + strlen(d) +strlen(fin)+ 1;
    char* res = (char*) malloc(i);

    if (res != NULL) {
        // Utiliser snprintf pour concaténer les chaînes
        //snprintf(res, i, "%s%s%s", p, d, fin);
        snprintf(res, i, p, x);
        strcat(res, d);
        strcat(res, fin);
    }    
    free(d);
    return res;
}

char** separerParEspaces(const char* chaine, int* taille) {
    if (chaine == NULL || *chaine == '\0') {return NULL;}

    // Compter le nombre d'espaces pour déterminer la taille du tableau
    int nombreEspaces = 0;
    const char* it = chaine;
    
    while (*it) {
        if (*it == ' ') {
            nombreEspaces++;
            // Ignorer les espaces consécutifs
            while (*(it + 1) == ' ') {it++;}
        }
        it++;
    }

    // Allouer de la mémoire pour le tableau de pointeurs
    *taille = nombreEspaces + 2; // pour gérer les fins de mots avec ou sans espaces et pour avoir suffisamment de place
    char** result = malloc((nombreEspaces + 2) * sizeof(char*));
    if (result == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour le tableau.\n");
        exit(EXIT_FAILURE);
    }

    // Remplir le tableau avec les mots
    int index = 0;
    it = chaine;
    while (*it) {
        if (*it != ' ') {
            const char* debutMot = it;
            // Trouver la fin du mot
            while (*it && *it != ' ') {it++;}
            // Allouer de la mémoire pour le mot
            result[index] = malloc((it - debutMot + 1) * sizeof(char));
            if (result[index] == NULL) {
                fprintf(stderr, "Erreur lors de l'allocation de mémoire pour le mot.\n");
                exit(EXIT_FAILURE);
            }
            // Copier le mot dans le tableau
            strncpy(result[index], debutMot, it - debutMot);
            result[index][it - debutMot] = '\0'; // Ajouter le caractère de fin de chaîne
            index++;
        }
         else {
            it++;
        }
    }
    // Marquer la fin du tableau avec NULL
    result[index] = NULL;

    return result;
}
/**/
char *** separeParPipe (const char* chaine, int* taille){
    if (chaine == NULL || *chaine == '\0') {return NULL;}
    int nombrePipe=0;
    const char* copie=chaine;

    while (*copie){
        if(*copie=='|'){
            nombrePipe+=1;
            while(*(copie+1) =='|'){copie+=1;}
        }
        copie++;
    }

    *taille=nombrePipe+1;
    char*** result=malloc(sizeof( char*)* (*taille) );

    if (result == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour le tableau.\n");
        exit(EXIT_FAILURE);

    }

    copie=chaine;
    int index=0;
    const char* debutCommande=copie;

    while (*copie) {
        if (*copie == '|' || *(copie + 1) == '\0') {
            if (*copie == '|') {
                result[index] = separerParEspaces(debutCommande, copie - debutCommande);
            } else { // Pour la dernière commande
                result[index] = separerParEspaces(debutCommande, copie - debutCommande + 1);
            }
            debutCommande = copie + 1;
            index++;
        }
        copie++;
    }
    /*
    while(*copie){
        if(*copie!='|'){

            //On marque le début du mot
            const char* debutCommande=*copie;

            //On se déplace à la fin du mot
            while(*copie && *copie!='|'){ copie+=1;}

            //On allour la mémoire pour stocker la commande dans le tableau
            result[index]=malloc(sizeof(char**)*(copie-debutCommande+1));

            if (result[index] == NULL) {
                fprintf(stderr, "Erreur lors de l'allocation de mémoire pour la commande.\n");
                exit(EXIT_FAILURE);
            }

            //On met la commande dans le tableau
            char** commandeI=separerParEspaces(debutCommande, copie - debutCommande);

            for(int i=0; i<copie-debutCommande;i++){
                
                strncpy(result[index][i], debutCommande, copie - debutCommande);
                result[index][i][copie - debutCommande] = '\0';// Ajouter le caractère de fin de chaîne
            }
            
            index++;

        }else{
            copie++;
        }
        

    }*/
    return result;


}

void freeAll(char** lib,int t){
    for(int i=0; i<t-1;i++){
        free(lib[i]);
    }
    free(lib);
}
