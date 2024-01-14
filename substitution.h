#ifndef SUBSITUTION_H
#define SUBSTITUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

int nb_sub(char** commande);
int sous_sub(char **commande, int i);
void tokenizer_first_command(char *token[], char *s, char *delimParameter, int *total);

void sub(char* command);//,char** commande_tab);
#endif 
