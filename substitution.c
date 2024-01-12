#include "substitution.h"

void sub(char** commande){
    //dprintf(STDOUT_FILENO," SUB \n");
    int num = 0;
	for(size_t i = 0; commande[i] != NULL; ++i) {
		if(strcmp(commande[i], "<(") == 0) num++; 
	}

    dprintf(STDOUT_FILENO,"le nombre de substitutions est %d \n", num);


}