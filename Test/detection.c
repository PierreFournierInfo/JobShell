#include <stdbool.h>

// Fonction pour détecter la vraie redirection dans la commande
// Retourne l'indice du tableau où se trouve la vraie redirection, -1 sinon
int detect_true_redirection(char **res, int taille) {
    bool in_redirection = false;

    for (int i = 0; i < taille; i++) {
        if (in_redirection) {
            // Si nous sommes déjà dans une redirection, vérifier si c'est un opérateur de redirection
            if (strcmp(res[i], "<") == 0 || strcmp(res[i], ">") == 0 || strcmp(res[i], ">|") == 0 ||
                strcmp(res[i], ">>") == 0 || strcmp(res[i], "2>") == 0 || strcmp(res[i], "2>|") == 0 ||
                strcmp(res[i], "2>>") == 0) {
                // C'est un autre opérateur de redirection, la vraie redirection est terminée
                return i - 1;
            }
        } else {
            // Si nous ne sommes pas dans une redirection, vérifier si c'est le début d'une nouvelle redirection
            if (strcmp(res[i], "<") == 0 || strcmp(res[i], ">") == 0 || strcmp(res[i], ">|") == 0 ||
                strcmp(res[i], ">>") == 0 || strcmp(res[i], "2>") == 0 || strcmp(res[i], "2>|") == 0 ||
                strcmp(res[i], "2>>") == 0) {
                // C'est le début d'une nouvelle redirection
                in_redirection = true;
            }
        }
    }

    // Si nous sommes toujours dans une redirection à la fin, alors la vraie redirection est à la fin
    if (in_redirection) {
        return taille ;
    }

    // Aucune vraie redirection détectée
    return -1;
}
