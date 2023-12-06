
#ifndef REDIRECTION_H
#define REDIRECTION_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

void command_r(char** res);
size_t tailleTableauChar(char **tableau);
bool verif_fic(const char *fic);
void redirect(char** res, int* pipefd, pid_t child_pid );
#endif 