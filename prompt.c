#include "prompt.h"
#include "signal_handler.h"
#include "job_manager.h"
#include "pipe.h"
#include "substitution.h"
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
    int x=job_count;
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
        dprintf(STDERR_FILENO, "Erreur lors de l'allocation de mémoire pour le tableau.\n");
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
                dprintf(STDERR_FILENO, "Erreur lors de l'allocation de mémoire pour le mot.\n");
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

void freeAll(char** lib,int t){
    for(int i=0; i<t;i++){
        if(lib[i] != NULL)free(lib[i]);
    }
    free(lib);
}

bool redirection_verif(char* input){
    for(size_t i=0;i< strlen(input);i++){
        if(input[i]=='<')return true;
        if(input[i]=='>') return true;
        if(input[i]=='>' && input[i+1]=='|') return true;
        if(input[i]=='>' && input[i+1]=='>') return true;  
        if(input[i]=='2' && input[i+1]=='>') return true;  
        //if(input[i]=='|') ;
    }
    return false;
}

bool pipe_verif(char* input){
    size_t input_length = strlen(input);

    for (size_t i = 0; i < input_length; i++) {
        if (input[i] == '|' && input[i-1] == ' ') {
            // Vérifier s'il y a un espace après la chaîne '|'
            //dprintf(STDOUT_FILENO,"%c", input[i]);
            if (strncmp(input + i + 1, " ", 1) == 0) { 
                return true;
            }
        }
    }
   // dprintf(STDIN_FILENO,"Pas trouvé \n");
    return false;
}

bool subVerif(char* input){
    size_t input_length = strlen(input);
    for (size_t i = 0; i < input_length ; i++) {
        if (input[i] == '<' && input[i-1] == ' ') {
            if (strncmp(input + i + 1, "(", 1) == 0) { 
                //dprintf(STDERR_FILENO," sub Verif \n");
                return true;
            }
        }
    }
    return false;
}



void afficherTableauChar(char **tableau) {
    // Vérification si le tableau est NULL
    if (tableau == NULL) {
        dprintf(STDOUT_FILENO,"Le tableau est NULL.\n");
        return;
    }

    // Parcours du tableau et affichage des éléments
    for (size_t i = 0; tableau[i] != NULL; ++i) {
        dprintf(STDOUT_FILENO,"%s\n", tableau[i]);
    }
}

void traiteCommande(){
 
 while (1) {
        check_all();   
        char* prompt = update_prompt();
        rl_outstream = stderr;
        char *input = readline(prompt);
        free(prompt);  
        
        if (!input) {
            exit(valeur_de_retour);
            break;  
        }

        add_history(input); 


        // Sub
        if(subVerif(input)){
            //dprintf(STDERR_FILENO," SUB\n ");
            int taille = 0;
            char** res = separerParEspaces(input, &taille);
            sub(res);

            freeAll(res,taille);
            free(input);
            continue;
        }

        if(redirection_verif(input)){  // vérification de la possibilité de redirection 
            // dprintf(STDOUT_FILENO, "Redirection\n");
            bool pipe_v = pipe_verif(input);
            if(!pipe_v) {
                // dprintf(STDOUT_FILENO," Redirection verif \n");
                int taille=0;
                char** res = separerParEspaces(input,&taille);
                //afficherTableauChar(res);
                command_r(res,taille);
                freeAll(res,taille);
                free(input); 
            }
            else{
                // dprintf(STDOUT_FILENO," Pipe redirection\n");
                 command_pipe(input);
            }
        }

        // Pipe 
        else if(pipe_verif(input)){
            //dprintf(STDERR_FILENO,"Verif2\n");
            //afficherTableauChar(res);
            command_pipe(input);
            free(input);
        }

        // Jobs sans option pour le moment 
        else if(strcmp(input,"jobs")==0){
            print_jobs();
            free(input);
        }
        else if(strncmp(input,"jobs -t",7)==0){
            int taille=0;
            char** res = separerParEspaces(input,&taille);
            // printf("%d\n",taille);
            if(taille <= 3){
                print_jobs_t(0);
            }
            else{
                char* v = res[2];
                int num;
                sscanf(v, "%%%d", &num);
                //printf("%d\n",num);
                print_jobs_t(num);
            }
            freeAll(res,taille);
            free(input);
        }
        else if(strncmp(input,"kill",4)==0){
            int taille=0;
            char** res = separerParEspaces(input,&taille);
            killProject(res);
            freeAll(res,taille);
            free(input);
        }
        
        //bg ou fg
        else if(strncmp(input,"bg",2)==0 || strncmp(input,"fg",2)==0){
            int taille = 0;
            char** res = separerParEspaces(input,&taille);
            //printf("bg fg :  %c \n", res[1][2]);
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
            free(input);  // Libère la mémoire rl_outstream allouée pour la ligne de commande lue
         } 
         else { 
                int taille=0;
                char** res = separerParEspaces(input,&taille);
                if(strlen(input)>0) execute_command(res[0],res);  
                free(input);
        }
    }
}
