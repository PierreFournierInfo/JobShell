#include "job_manager.h"
#include <signal.h>

// Exemple d'implémentation d'un module de gestion des tâches

// Variables globales pour stocker les jobs
Job* jobs_list = NULL;
int job_count = 0;

// Fonction pour initialiser le gestionnaire de tâches
void initialize_job_manager() {
    /*struct sigaction sa;

    // Initialiser la structure sa
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);

    // Utiliser sigaction pour gérer SIGCHLD
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    */
    jobs_list = NULL;
    job_count = 0;
}

// Fonction pour créer un nouveau job
 Job* create_job(pid_t process_id, const char *command) {
    Job *new_job = malloc(sizeof(Job));
    if (new_job != NULL) {
        job_count++/* assigner un ID unique */;
        new_job->id = job_count;
        new_job->process_id = process_id;
        new_job->status = JOB_STATUS_RUNNING;
        new_job->exit_status = 0; // Initialiser le code de sortie
        // Vérification de notre allocation de mémoire 
        new_job -> command = (char *)malloc(strlen(command) + 1);
        if(new_job->command == NULL ) { fprintf(stderr,"Erreur job allocation de mémoire"); exit(EXIT_FAILURE);}
        strcpy(new_job->command,command);
        new_job->next = NULL;
    }

    // ajouter dans notre liste de Job le nouveau Job
    add_job(new_job);
    return new_job;
}

// Fonction pour ajouter un job à la liste
void add_job( Job *job) {
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

// Fonction pour vérifier l'état d'un job à partir de son PID
enum JobStatus check_job_status(pid_t process_id) {
    int status;
    pid_t result = waitpid(process_id, &status, WNOHANG);

    if (result == 0) {
        // Le processus est toujours en cours d'exécution
        printf("Running job\n");
        return JOB_STATUS_RUNNING;
    } else if (result > 0) {
        // Le processus s'est terminé normalement
        if (WIFEXITED(status)) {
            return JOB_STATUS_DONE;
        } else {
            // Le processus s'est terminé en raison d'un signal
            return JOB_STATUS_DONE;  // ou un autre statut selon vos besoins
        }
    } else {
        // Une erreur s'est produite lors de l'appel à waitpid
        perror("waitpid");
        return JOB_STATUS_DONE;  // ou un autre statut selon vos besoins
    }
}

// Fonction pour mettre à jour l'état d'un job
void update_job_status(pid_t process_id, enum JobStatus new_status) {
      Job *current_job = jobs_list;  // Assurez-vous d'avoir un pointeur de tête pour la liste des jobs
    while (current_job != NULL) {
        if (current_job->process_id == process_id) {
            current_job->exit_status = new_status;
            if (WIFEXITED(new_status)) {
                current_job->status = JOB_STATUS_DONE;
            } else if (WIFSTOPPED(new_status)) {
                current_job->status = JOB_STATUS_STOPPED;
            } else if (WIFSIGNALED(new_status)) {
                current_job->status = JOB_STATUS_DONE; // ou JOB_STATUS_RUNNING selon votre logique
            }
            break;  // Le processus a été trouvé, inutile de continuer la recherche
        }
        current_job = current_job->next;
    }
}

// Fonction pour mettre à jour l'état de tous les jobs
void update_all_jobs() {
    Job *current_job = jobs_list; // Supposons que `head` est votre pointeur de début de la liste de jobs

    while (current_job != NULL) {
        enum JobStatus new_status = check_job_status(current_job->process_id);
        update_job_status(current_job->process_id, new_status);
        current_job = current_job->next;
    }
}

// Fonction pour supprimer les jobs terminés de la liste
void remove_completed_jobs() {
     Job *current = jobs_list;
     Job *previous = NULL;

    while (current != NULL) {
        if (current->status == JOB_STATUS_DONE) {
            // Job terminé, le supprimer de la liste
            if (previous != NULL) {
                previous->next = current->next;
            } else {
                jobs_list = current->next;
            }
            // Libérer la mémoire du job
            free(current->command);
            free(current);

            current = (previous != NULL) ? previous->next : jobs_list;
        } else {
            previous = current;
            current = current->next;
        }
    }

}

// Fonction pour afficher la liste des jobs
void print_jobs() {
    Job *current = jobs_list;
    
    // Attendre un court instant pour permettre aux états des jobs de se mettre à jour
    //sleep(1);

    while (current != NULL) {
        char* status;
        if (current->status == JOB_STATUS_RUNNING) {
            status = "Running";
        } else if (current->status == JOB_STATUS_DONE) {
            status = "Done";
        } else {
            status = "Stopped";
        }
        printf("[%d]  %d  %s\t %s \n", current->id, current->process_id, status, current->command);
        current = current->next;
    }
    
}

// Fonction pour relancer à l'arrière-plan l'exécution du job spécifié en argument
void bg_command(const char *job_id_str) {
    int job_id = atoi(job_id_str);

    // Trouver le job avec l'ID spécifié
    Job *job = find_job_by_id(job_id);

    if (job != NULL) {
        // Relancer le processus en arrière-plan
        kill(job->process_id, SIGCONT);
        update_job_status(job->process_id, JOB_STATUS_RUNNING);
        printf("Le job %d (%s) a été relancé en avant-plan.\n", job->id, job->command);
    } else {
        printf("Job %d non trouvé.\n", job_id);
    }
}

// Fonction pour relancer à l'avant-plan l'exécution du job spécifié en argument
void fg_command(const char *job_id_str) {
    int job_id = atoi(job_id_str);

    // Trouver le job avec l'ID spécifié
    Job *job = find_job_by_id(job_id);

    if (job != NULL) {
        // Relancer le processus en avant-plan
        kill(job->process_id, SIGCONT);
        update_job_status(job->process_id, JOB_STATUS_RUNNING);

        // Attendre la fin du processus en avant-plan
        int status;
        waitpid(job->process_id, &status, WUNTRACED);

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

// Fonction pour trouver un job par son ID
Job *find_job_by_id(int job_id) {
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

void handle_sigchld(int signo) {
    (void)signo;  // Évite un avertissement "unused parameter"
    
    // Attendre tous les processus fils sans bloquer
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // Mettre à jour l'état des jobs ici
        update_all_jobs();
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
