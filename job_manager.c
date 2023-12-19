#include "job_manager.h"

// Variables globales pour stocker les jobs
Job* jobs_list = NULL;
int job_count = 0;

// Fonction pour créer un nouveau job
void create_job(pid_t process_id, const char *command) {
    Job *new_job = malloc(sizeof(Job));
    if (new_job != NULL) {
        //job_count++/* assigner un ID unique */;
        new_job->id = ++job_count;
        new_job->process_id = process_id;
        new_job->status = JOB_STATUS_RUNNING;
        new_job->exit_status = 0; // Initialiser le code de sortie
        // Vérification de notre allocation de mémoire 
        new_job -> command = (char *)malloc(strlen(command) + 1);
        if(new_job->command == NULL ) { fprintf(stderr,"Erreur job allocation de mémoire"); exit(EXIT_FAILURE);}
        strcpy(new_job->command,command);
        new_job->next = NULL;

        // Créer un nouveau groupe de processus avec le PID du job
        if (setpgid(process_id, process_id) == -1) {
            perror("setpgid");
            exit(EXIT_FAILURE);
        }
    }
    // ajouter dans notre liste de Job le nouveau Job
    add_job(new_job);
    fprintf(stderr,"[%d]  %d  Running\t %s\n", new_job->id, new_job->process_id, new_job->command);

}

// Fonction pour ajouter un job à la liste
void add_job( Job *job) {
      // Vérifier si un job avec le même ID existe déjà (normalement ça devrait pas arriver )
    Job *existing_job = find_job_by_process_id(job->id);

    if (existing_job != NULL) {
        // Mise à jour du job existant au lieu d'en ajouter un nouveau
        existing_job->process_id = job->process_id;
        existing_job->status = job->status;
        existing_job->exit_status = job->exit_status;
        // Vous pouvez mettre à jour d'autres champs si nécessaire

        // Libérer la mémoire du job à ajouter
        free(job->command);
        free(job);
    } else {
        // Ajouter le nouveau job à la liste
        if (jobs_list == NULL) {
            // La liste est vide, ajouter le job en tant que tête de liste
            jobs_list = job;
        } else {
            // La liste n'est pas vide, trouver le dernier job et ajouter le nouveau job
            Job *current_job = jobs_list;
            while (current_job->next != NULL) {
                current_job = current_job->next;
            }
            current_job->next = job;
        }
    }
}

// Fonction pour vérifier l'état d'un job à partir de son PID
enum JobStatus check_job_status(pid_t process_id){ //Job* current_Job) {
    int status;
    pid_t result = waitpid(process_id, &status, WNOHANG);

    if (result > 0) {
        return JOB_STATUS_RUNNING;
    } else if (result == 0) {
        return JOB_STATUS_DONE;
    } else {
        return JOB_STATUS_STOPPED;  // ou un autre statut selon vos besoins
    }
}

// Fonction pour mettre à jour l'état d'un job
void update_job_status(pid_t process_id, int status) {
    Job *current_job = jobs_list;
    while (current_job != NULL) {
        if (current_job->process_id == process_id) {
            current_job->exit_status = status;  // Utilisez le statut du processus, pas l'enum JobStatus
            if (WIFEXITED(status)) {
                current_job->status = JOB_STATUS_DONE;
            } else if (WIFSTOPPED(status)) {
                current_job->status = JOB_STATUS_STOPPED;
            } else if (WIFSIGNALED(status)) {
                current_job->status = JOB_STATUS_STOPPED; 
            }
            break;
        }
        current_job = current_job->next;
    }
}

void print_jobs_f(Job *node) {
    if (node == NULL) {
        return;
    }
    // Récursion pour atteindre la fin de la liste
    print_jobs_f(node->next);

    // Afficher le nœud actuel
    if (node->status == JOB_STATUS_DONE) {
        fprintf(stderr,"[%d]  %d  Done\t %s\n", node->id, node->process_id, node->command);
    }
    else if(node->status == JOB_STATUS_STOPPED){
        fprintf(stderr,"[%d]  %d  Killed\t %s\n", node->id, node->process_id, node->command);
    }
}


// Fonction pour supprimer les jobs terminés de la liste
void remove_completed_jobs() {
    Job *current = jobs_list;
    Job **previousPtr = &jobs_list;

    print_jobs_f(current);
    for (; current != NULL; current = *previousPtr) {
        if (current->status == JOB_STATUS_DONE) {
            
            *previousPtr = current->next;
            
            // Stocker le prochain nœud avant de libérer la mémoire
            Job *nextNode = current->next;

            free(current->command);
            free(current);

            job_count--;

            // Mettre à jour le pointeur vers le prochain nœud
            current = nextNode;
        }
        else if(current->status == JOB_STATUS_STOPPED){
            *previousPtr = current->next;
            
            // Stocker le prochain nœud avant de libérer la mémoire
            Job *nextNode = current->next;

            free(current->command);
            free(current);

            job_count--;

            // Mettre à jour le pointeur vers le prochain nœud
            current = nextNode;
        } else {
            previousPtr = &current->next;
        }
    }
}

// Fonction pour afficher la liste des jobs
void print_jobs() {
    Job *current = jobs_list;
    while (current != NULL) {
        char* status;
        if (current->status == JOB_STATUS_RUNNING) {
            status = "Running";
        } else if (current->status == JOB_STATUS_DONE) {
            status = "Done";
        } else {
            status = "Stopped";
        }
        printf("[%d]  %d  %s\t %s\n", current->id, current->process_id, status, current->command);
        current = current->next;
    }
}

// Fonction pour relancer à l'arrière-plan l'exécution du job spécifié en argument
void bg_command(const char *job_id_str) {
    int job_id = atoi(job_id_str);

    // Trouver le job avec l'ID spécifié
    Job *job = find_job_by_process_id(job_id);

    if (job != NULL) {
        // Relancer le processus en arrière-plan
        killpg(job->process_id, SIGCONT);  // Envoyer SIGCONT à tout le groupe de processus

        // Assigner le contrôle du terminal au groupe de processus du job
        tcsetpgrp(STDIN_FILENO, job->process_id);

        update_job_status(job->process_id, JOB_STATUS_RUNNING);
        printf("Le job %d (%s) a été relancé en arrière-plan.\n", job->id, job->command);
    } else {
        printf("Job %d non trouvé.\n", job_id);
    }
}

// Fonction pour relancer à l'avant-plan l'exécution du job spécifié en argument
void fg_command(const char *job_id_str) {
    int job_id = atoi(job_id_str);

    // Trouver le job avec l'ID spécifié
    Job *job = find_job_by_process_id(job_id);

    if (job != NULL) {
         // Relancer le processus en avant-plan
        killpg(job->process_id, SIGCONT);  // Envoyer SIGCONT à tout le groupe de processus

        // Assigner le contrôle du terminal au groupe de processus du job
        tcsetpgrp(STDIN_FILENO, getpgrp());
        printf("Le contrôle du terminal a été rendu au shell.\n");

        update_job_status(job->process_id, JOB_STATUS_RUNNING);

        // Attendre la fin du processus en avant-plan
        int status;
        waitpid(job->process_id, &status, WUNTRACED);

        // Assigner le contrôle du terminal au groupe de processus du shell
        tcsetpgrp(STDIN_FILENO, getpgrp());

        // Mise à jour de l'état du job après son achèvement
        if (WIFEXITED(status)) {
            update_job_status(job->process_id, JOB_STATUS_DONE);
            job->exit_status = WEXITSTATUS(status);
        } else if (WIFSTOPPED(status)) {
            update_job_status(job->process_id, JOB_STATUS_STOPPED);
        } else if (WIFSIGNALED(status)) {
            update_job_status(job->process_id, JOB_STATUS_DONE);
            job->exit_status = WTERMSIG(status);
        }
        printf("Le job %d (%s) a été relancé en avant-plan.\n", job->id, job->command);
    } else {
        printf("Job %d non trouvé.\n", job_id);
    }
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
        return JOB_STATUS_DONE;
    } else if (WIFSTOPPED(status)) {
        return JOB_STATUS_STOPPED;
    } else {
        return JOB_STATUS_RUNNING;
    }
}

// Actualistation des job
void check_all() {
    Job *current = jobs_list;
    while (current != NULL) {
        pid_t pid = current->process_id;
        int status;
        
        // Utilisez WNOHANG pour ne pas bloquer si le processus n'est pas terminé
        if (waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0) {
            // Mettre à jour l'état du job
            enum JobStatus new_status = get_job_status(status);
            if (current->status != new_status) {
                update_job_status(pid, status);
            }
        }   
        current = current->next;
    }
    remove_completed_jobs();

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