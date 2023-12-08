#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_JOBS 100
// Définir les états des jobs
extern int job_count;
enum JobStatus {
    JOB_STATUS_RUNNING,
    JOB_STATUS_STOPPED,
    JOB_STATUS_DONE,
};

// Structure pour représenter un job
typedef struct Job{
    int id;               // Identifiant du job
    pid_t process_id;     // Identifiant du processus
    enum JobStatus status; // État du job
    char* command;         // Commande associée au job
    struct Job* next;   // Pointeur vers le prochain job dans la liste
    int exit_status;  // Code de sortie du processus
} Job;

// Fonctions du module de gestion des tâches
void initialize_job_manager();
Job* create_job(pid_t process_id, const char *command);
void add_job( Job *job);
void update_job_status(pid_t process_id, enum JobStatus new_status);
void remove_completed_jobs();
void print_jobs();
void free_jobs();

#endif  // JOB_MANAGER_H
