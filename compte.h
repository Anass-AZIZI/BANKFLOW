#ifndef COMPTE_H
#define COMPTE_H

#include "structures.h"

// Constantes pour les noms de fichiers
extern const char* FILE_COMPTES;
extern const char* FILE_DEMANDES;

// Fonctions principales pour gerer les comptes
Compte* get_account_by_id(int id);
void save_all_accounts();
int load_accounts(); // Retourne le nombre de comptes charges

// Fonctions pour l'administrateur
void create_account_request(Compte newAccount);
void process_requests(); // Interface admin pour accepter/refuser
void delete_account(int id);

// Fonction utilitaire
int get_next_account_id();

#endif
