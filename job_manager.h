#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_JOBS 512
#define MAX_BUFFER_SIZE 1024
// Définir les états des jobs
extern int job_count;
enum JobStatus {
    JOB_STATUS_RUNNING,
    JOB_STATUS_STOPPED,
    JOB_STATUS_DONE,
    JOB_STATUS_KILLED,
};

// Structure pour représenter un job
typedef struct Job{
    int id;               // Identifiant du job
    pid_t process_id;     // Identifiant du processus , processus su groupe plus tard 
    enum JobStatus status; // État du job
    char* command;         // Commande associée au job
    struct Job* next;   // Pointeur vers le prochain job dans la liste
    int exit_status;  // Code de sortie du processus
    
    int background ; // si on a une fonction en arrière plan 0 si oui 
} Job;

// Fonctions du module de gestion des tâches
void create_job(pid_t process_id, const char *command, int back);
void add_job( Job *job);

void remove_job(Job* j);
void print_one_job();
void free_jobs();

Job *find_job_by_process_id(int job_pid);
Job *find_job_by_id(int job_id);

enum JobStatus get_job_status(int status);
void update_background_jobs();

void check_all();
void print_jobs_f(Job *node) ;
void print_jobs();

bool empty_jobs();

void print_jobs_t(int job_id);
void print_process_tree(pid_t parent_pid, int depth);

#endif 
