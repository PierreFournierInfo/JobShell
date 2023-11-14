#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

// Codes de couleur ANSI
#define COLOR_RESET "\033[00m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_END "\033[0m"

// Ajoutez d'autres codes de couleur au besoin
#define PATH_MAX 30;

void display_prompt(int num_jobs) {
    char current_dir[PATH_MAX];
    char color1[10], color2[10];

    // Obtenir le répertoire de travail courant
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    // Définir les codes de couleur en fonction du nombre de jobs
    if (num_jobs > 0) {
        strcpy(color1, COLOR_YELLOW); // Jaune pour indiquer des jobs en cours
    } else {
        strcpy(color1, COLOR_GREEN);  // Vert pour indiquer aucun job en cours
    }

    // Ajoutez des conditions pour d'autres codes de couleur en fonction des besoins

    strcpy(color2, COLOR_RESET); // Réinitialisation de la couleur

    // Afficher le prompt formaté
    printf("%s[%d]%s %s%s$ ", color1, num_jobs, color2, current_dir, COLOR_RESET);
}

int main() {
    // Exemple d'utilisation du prompt
    int num_jobs = 0;

    while (1) {
        // Afficher le prompt
        display_prompt(num_jobs);

        // Lire la commande de l'utilisateur
        // Ajoutez ici la logique pour lire la commande et exécuter les actions nécessaires

        // Exemple : Simule l'incrémentation du nombre de jobs
        num_jobs++;

        // Ajoutez ici la logique pour gérer les autres commandes et la mise à jour du nombre de jobs
    }

    return 0;
}
