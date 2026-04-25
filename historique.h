#ifndef HISTORIQUE_H
#define HISTORIQUE_H

#include "structures.h"

// Adds an entry to the history file
void add_history(int idCompte, const char *type, double montant, int destId);

// Loads history from file into an array
int load_history(const char *filename, Historique tab[], int max);

// Saves history array to file
void save_history(const char *filename, Historique tab[], int n);

// Prints history for a specific account
void print_history(int idCompte);

#endif
