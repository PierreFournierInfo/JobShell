# Nom de l'exécutable à produire
TARGET = jsh

# Compilateur à utiliser
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -g

# Recherche des fichiers .c pour construire la liste des fichiers source
SRC = $(wildcard *.c)

# Conversion des fichiers .c en .o pour la liste des fichiers objets
OBJ = $(SRC:.c=.o)

# Règle par défaut
all: $(TARGET)

# Règle pour construire l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Inclut les dépendances des en-têtes
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(OBJ) $(TARGET)

# Empêche la confusion en cas de fichier nommé clean
.PHONY: clean
