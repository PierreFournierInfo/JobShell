#include "job_manager.h"

// Variables globales pour stocker les jobs
Job* jobs_list = NULL;
int job_count = 0;

// Fonction pour créer un nouveau job
void create_job(pid_t process_id, const char *command, int back) {
    Job *new_job = malloc(sizeof(Job));
    if (new_job != NULL) {
        new_job->background = back;
        /* assigner un ID unique */
        new_job->id = ++job_count;
        new_job->process_id = process_id;

        new_job->process_group_id = 0;

        new_job->status = JOB_STATUS_RUNNING;
        new_job->exit_status = 0; // Initialiser le code de sortie
        // Vérification de notre allocation de mémoire 
        new_job -> command = (char *)malloc(strlen(command) + 1);
        if(new_job->command == NULL ) { dprintf(STDERR_FILENO,"Erreur job allocation de mémoire"); exit(EXIT_FAILURE);}
        strcpy(new_job->command,command);
        new_job->next = NULL;
    }
    // ajouter dans notre liste de Job le nouveau Job
    add_job(new_job);
}

// Fonction pour ajouter un job à la liste
void add_job( Job *job) {
      // Vérifier si un job avec le même ID existe déjà (normalement ça devrait pas arriver )
    
        // Ajouter le nouveau job à la liste
        if (jobs_list == NULL) {
            // La liste est vide, ajouter le job en tant que tête de liste
            jobs_list = job;
        } 
        else {
            // La liste n'est pas vide, trouver le dernier job et ajouter le nouveau job
            Job *current_job = jobs_list;
            while (current_job->next != NULL) {
                current_job = current_job->next;
            }
            current_job->next = job;
        }
}



// Fonction pour supprimer les jobs terminés de la liste
void remove_job(Job* j) {
    Job *current = jobs_list;
    Job *previousPtr = NULL;
 
    while (current != NULL && current != j) {
       previousPtr = current;
       current = current->next;
    }

    if(current == NULL) return; 
    if(previousPtr == NULL) jobs_list= current->next;
    else {
        previousPtr->next = current->next;
    }

    job_count--;
    free(current->command);
    free(current);
}


void print_one_job(int fd,Job* current){
     char* status;
     switch(current->status) {
        case JOB_STATUS_DONE:
            status = "Done";
            break;
        case JOB_STATUS_STOPPED:
            status = "Stopped";
            break;
        case JOB_STATUS_KILLED:
            status = "Killed";
            break;
        case JOB_STATUS_RUNNING:
            status = "Running";
            break;
     }
    dprintf(fd,"[%d] %d %s %s\n", current->id, current->process_id, status, current->command);
    current = current->next;
}

// Fonction pour afficher la liste des jobs
void print_jobs() {
    Job *current;
    pid_t res;
    int status;
        while ((res = waitpid(-1, &status, WNOHANG)) > 0) { // attendre tous les processus dans le même groupe 
            current = jobs_list;
             while(current != NULL) {
                pid_t pid = current->process_id;
                Job* next = current->next;
                // en fait il change d'état avant du coup faut waitpid
                if (pid == res){
                    // Mettre à jour l'état du job
                    if (WIFEXITED(status)) {
                        current->status = JOB_STATUS_DONE;
                    } 
               }
                // car il y a un plus sur le vrai terminal donc on affiche sur la sortie erreur standart 
                if(current->status == JOB_STATUS_DONE){ 
                     print_one_job(STDOUT_FILENO,current);
                    remove_job(current);
                }
                current = next;
            }
        }
        current = jobs_list;
        while(current != NULL){
            print_one_job(STDOUT_FILENO,current);
            current = current->next;
        }
}

// Fonction pour afficher la liste des jobs avec l'option -t
void print_jobs_t(int job_id) {
    Job *current;
    int x = -1;
    pid_t res;
    int status;

    while ((res = waitpid(x, &status, WNOHANG)) > 0) {
        current = jobs_list;
             while(current != NULL) {
                pid_t pid = current->process_id;
                Job* next = current->next;
                if (pid == res){
                    if (WIFEXITED(status)) {
                        current->status = JOB_STATUS_DONE;
                    } 
               }
                if(current->status == JOB_STATUS_DONE){ 
                    print_one_job(STDOUT_FILENO,current);
                    remove_job(current);
                }
                current = next;
            }
    }

    if(job_id == 0) {
     current = jobs_list;
    }
    else{
        current = find_job_by_id(job_id);
        x = current->process_id;
    }
     while (current != NULL) {
            if (job_id == 0 || current->id == job_id) {
                print_one_job(STDOUT_FILENO, current);

                //waitpid(-current->process_id, NULL, WUNTRACED);
                print_process_tree(current->process_id, 1);
            }
            current = current->next;
    }
}

void print_process_tree(pid_t parent_pid, int depth) {
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error opening /proc directory");
        exit(EXIT_FAILURE);
    }
   struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            char proc_path[256];
            snprintf(proc_path, sizeof(proc_path), "/proc/%s", entry->d_name);

            int cmdline_fd = open(strcat(proc_path, "/stat"), O_RDONLY);
            if (cmdline_fd == -1) {
                perror("Error opening stat file open");
                exit(EXIT_FAILURE);
            }

            int pid, ppid;
            if (read(cmdline_fd, &pid, sizeof(pid)) == -1) {
                perror("Error reading stat file pid");
                exit(EXIT_FAILURE);
            }

            if (read(cmdline_fd, &ppid, sizeof(ppid)) == -1) {
                perror("Error reading stat file ppid");
                exit(EXIT_FAILURE);
            }

            close(cmdline_fd);

            // Vérification processus fils du parent
            if (ppid == parent_pid) {
                for (int i = 0; i < depth; ++i) {
                    printf("  ");
                }
                printf("%d\n", pid);
                print_process_tree(pid, depth + 1);
            }
        }
    }
    closedir(dir);
}


// Fonction pour trouver un job par son PID
Job *find_job_by_process_id(int job_id) {
    Job *current_job = jobs_list; // Notre tête de liste normalement 

    while (current_job != NULL) {
        //printf("Recherche Job : %s\n", current_job->command);
        if (current_job->process_id == job_id) {
            return current_job; // Job trouvé
        }
        current_job = current_job->next;
    }
    return NULL; // Job non trouvé
}

// Par ID
Job *find_job_by_id(int job_id) {
    Job *current_job = jobs_list; // Notre tête de liste normalement 

    while (current_job != NULL) {
        //printf("Recherche Job : %s\n", current_job->command);
        if (current_job->id == job_id) {
            return current_job; // Job trouvé
        }
        current_job = current_job->next;
    }
    return NULL; // Job non trouvé
}

enum JobStatus get_job_status(int status) {
    if (WIFEXITED(status)) {
        return JOB_STATUS_DONE;
    } else if (WIFSIGNALED(status)) {
        return JOB_STATUS_KILLED;
    } else if (WIFSTOPPED(status)) {
        return JOB_STATUS_STOPPED;
    } else {
        return JOB_STATUS_RUNNING;
    }
}

// Actualistation des job
void check_all() {
    Job *current;
    pid_t res;
    int status;
        // Utilisez WNOHANG pour ne pas bloquer si le processus n'est pas terminé
        while ((res = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) { // attendre tous les processus dans le même groupe 
             current  = jobs_list;
             while(current != NULL) {
                pid_t pid = current->process_id;
                Job* next = current->next;
                if (pid == res){
                    // Mettre à jour l'état du job
                    if (WIFEXITED(status)) {
                        current->status = JOB_STATUS_DONE;
                    } else if (WIFSTOPPED(status)) { 
                        //printf("Stoppé de job\n");
                        current->status = JOB_STATUS_STOPPED;
                    } else if (WIFSIGNALED(status)) {
                        current->status = JOB_STATUS_KILLED;
                    } else if(WIFCONTINUED(status)){
                        current -> status = JOB_STATUS_RUNNING;
                    }
                    print_one_job(STDERR_FILENO,current);
                }
                if (current->status == JOB_STATUS_DONE || current->status == JOB_STATUS_KILLED) {
                    remove_job(current);
                }
                current = next;
            } 
    }
}
        

// Fonction pour libérer la mémoire des jobs
void free_jobs() {
    Job *current = jobs_list;
    Job *next;
    while (current != NULL) {
        next = current->next;
        free(current->command);
        free(current);
        current = next;
    }
}

// fonction pour vérifier si notre liste est null
bool empty_jobs(){
    return jobs_list == NULL;
}