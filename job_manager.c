#include "job_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

// Exemple d'implémentation d'un module de gestion des tâches

// Variables globales pour stocker les jobs
static Job* jobs_list = NULL;
static int job_count = 0;

// Fonction pour initialiser le gestionnaire de tâches
void initialize_job_manager() {
    jobs_list = NULL;
    job_count = 0;
}

// Fonction pour créer un nouveau job
 Job* create_job(pid_t process_id, const char *command) {
    Job *new_job = malloc(sizeof(Job));
    if (new_job != NULL) {
        new_job->id = job_count++/* assigner un ID unique */;
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

    while (current != NULL) {
        char* status;
        if(current->status == JOB_STATUS_RUNNING){
            status = "Running";
        }
        else if(current->status == JOB_STATUS_DONE ){
            status = "Done";
        }
        else{
            status = "Stopped";
        }
        printf("[%d]  %d  %s\t %s \n", current->id, current->process_id,status, current->command);
        current = current->next;
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
