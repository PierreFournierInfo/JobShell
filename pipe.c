#include "pipe.h"


//--------------------------------------------------------------- PIPE -----------------------------------------------------------------------
bool redirection_verify_pipe(char* tableau){
    return 
        strcmp(tableau, "<") == 0 ||
        strcmp(tableau, ">") == 0 ||
        strcmp(tableau, ">|") == 0 ||
        strcmp(tableau, ">>") == 0 ||
        strcmp(tableau, "2>") == 0 ||
        strcmp(tableau, "2>|") == 0 ||
        strcmp(tableau, "2>>") == 0 ;
}

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

void redirection_p(char* redirection, char* file){
    int flags, mode, fd;
    if(strcmp("<",redirection)==0){
        flags = O_RDONLY;
        mode = 0666;
        fd = open(file,flags,mode);
        dup2(fd, STDIN_FILENO);
    }
    else if(strcmp(">",redirection)==0){
        flags = O_WRONLY | O_CREAT | O_EXCL | O_TRUNC;
        mode = 0666;    
        fd = open(file,flags,mode);
        dup2(fd, STDOUT_FILENO);
    }
    else if(strcmp(">|",redirection)==0){
        flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode = 0666;
         fd = open(file,flags,mode);
        dup2(fd, STDOUT_FILENO);
    }
    else if(strcmp(">>",redirection)==0){
        flags = O_WRONLY | O_CREAT | O_APPEND;
        mode = 0666;
         fd = open(file,flags,mode);
        dup2(fd, STDOUT_FILENO);
    }
    else if(strcmp("2>",redirection)==0){
        flags = O_WRONLY | O_CREAT | O_TRUNC | O_EXCL;
        mode = 0666;
         fd = open(file,flags,mode);
        dup2(fd, STDERR_FILENO);
    }
    else if(strcmp("2>|",redirection)==0){
        flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode = 0666;
         fd = open(file,flags,mode);
        dup2(fd, STDERR_FILENO);
    }
    else if(strcmp("2>>",redirection)==0){
        flags = O_WRONLY | O_CREAT | O_APPEND;
        mode = 0666;
         fd = open(file,flags,mode);
        dup2(fd, STDERR_FILENO);
    }
    close(fd);
}

void command_pipe(char *command) {
    // dprintf(STDOUT_FILENO, " PIPE %s\n", command);
    char *pipedCommand[10000];
    int numPipeCommands = 0;
    int* position=malloc(sizeof(int));

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

            int ind = 0;
            while (spaceSepPipedCommand[ind] != NULL) {
                if (redirection_verify_pipe(spaceSepPipedCommand[ind])) {
                        redirection_p(spaceSepPipedCommand[ind], spaceSepPipedCommand[ind+1]);
                        spaceSepPipedCommand[ind] = NULL;
                        ind++;
                    }
                ind++;
            }
       
            execvp(spaceSepPipedCommand[0], spaceSepPipedCommand);
            
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
     
    free(position);
    return;
}