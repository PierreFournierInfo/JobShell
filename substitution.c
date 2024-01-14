#include "substitution.h"

// Compter le nombre de sub
int nb_sub(char** commande){
    int total=0;
    for(size_t i = 0; commande[i] != NULL; ++i) {
        if(sous_sub(commande,i))total++;
	}
    return total;
}

// voir les sous sub 
int sous_sub(char **commande, int i){
    if (strcmp(commande[i], "<(") == 0){
        ++i;
        while(commande[i] != NULL){
            if (strcmp(commande[i], ")") == 0) return true;
            ++i;
        }
    }
    return false;
}

// Obtenir la première comamnde 
void tokenizer_first_command(char *token[], char *s, char *delimParameter, int *total){    
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



void sub(char* command){//,char** commande_tab){
    //dprintf(STDOUT_FILENO," SUB \n");
    char *subCommand[10000];
    int numSubCommands = 0;
    
    tokenizer_first_command(subCommand, command, "<()", &numSubCommands);
    
    // si on a une seule -1 sinon -2
    if(numSubCommands == 2){ numSubCommands --;}
    else{ numSubCommands -=2 ;}

    //char adresse[numSubCommands][256];

    //int pipes[numSubCommands][2];
    //int ind = 0;
    
    for (int i = 0; i < numSubCommands; i++) {
        int lenOfEachPipeCommand = 0;
        char *spaceSepCommand[10000];

        // commencer à l'indice de seconde commande 
        if(i%2 == 0){ 
            tokenizer_first_command(spaceSepCommand, subCommand[i+1], ")", &lenOfEachPipeCommand);
        }
        else{
            tokenizer_first_command(spaceSepCommand, subCommand[i+1],"<(", &lenOfEachPipeCommand);
        }
        //dprintf(STDOUT_FILENO," %s\n",spaceSepCommand[i]);
    }

    for (int i = 0; i < numSubCommands; i++){
        wait(NULL);
    }

    // execution de la première commande à la fin 

}