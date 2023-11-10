# Architecture du Projet - jsh (Shell)

## 1. Module Principal (`main.c`)

- Gère le flux principal du programme.
- Initialise le shell, lit les commandes de l'utilisateur, et les transmet au module de gestion de la ligne de commande.

## 2. Module de Gestion de la Ligne de Commande (`command_parser.c`)

- Analyse la ligne de commande saisie par l'utilisateur.
- Identifie les commandes internes (pwd, cd, ?, exit) et les traite directement.
- Pour les commandes externes, délègue la gestion au module d'exécution des commandes.

## 3. Module d'Exécution des Commandes (`command_executor.c`)

- Prend en charge l'exécution des commandes externes.
- Gère la création de nouveaux processus pour chaque commande.
- Gère les redirections des entrées/sorties standard.
- Gère les combinaisons par tube.

## 4. Module de Gestion des Tâches (`job_manager.c`)

- Gère la création et la gestion des jobs.
- Maintient une liste des jobs en cours avec leurs états (en cours d'exécution, suspendu, terminé, etc.).
- Permet la bascule entre l'avant-plan et l'arrière-plan des jobs.

## 5. Module de Gestion des Signaux (`signal_handler.c`)

- Gère les signaux spécifiés dans le projet (SIGINT, SIGTERM, etc.).
- Peut inclure la gestion personnalisée de l'exit pour envoyer des signaux SIGHUP aux processus en cours lors de la terminaison du shell.

## 6. Module d'Affichage du Prompt (`prompt.c`)

- Gère la génération du prompt en fonction du nombre de jobs en cours.
- Utilise la bibliothèque Readline pour faciliter la lecture de la ligne de commande.

## 7. Module de Redirection (`redirection.c`)

- Gère les différentes formes de redirection (entrée/sortie standard, combinaisons par tube, etc.).

## 8. Module de Tests (`tests.c`)

- Contient des tests unitaires pour chaque module afin de s'assurer du bon fonctionnement de chaque composant du shell.

## 9. Fichiers d'En-tête (`*.h`)

- Déclare les prototypes des fonctions pour chaque module.
- Facilite la séparation de l'interface et de l'implémentation.

## 10. Makefile (`Makefile`)

- Facilite la compilation et la gestion des dépendances du projet.

