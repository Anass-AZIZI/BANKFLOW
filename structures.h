#ifndef STRUCTURES_H
#define STRUCTURES_H

// Constantes
#define MAX_COMPTES 100
#define MAX_HISTORIQUE 1000

// Structure pour un Compte
typedef struct {
    int id;
    char password[50];
    char date_creation[20];
    char nom[50];
    char cin[20];
    char date_naissance[20];
    int age;
    char adresse[100];
    char telephone[20];
    char citoyennete[20];
    int type_compte; // 0: Courant, 1: Epargne, etc.
    double solde;
    int locked;      // 0: Actif, 1: Verrouille
    int role;        // 0: Client, 1: Admin
} Compte;

// Structure pour l'Historique
typedef struct {
    int idCompte;
    char typeOperation[20]; // "depot", "retrait", "transfert_de", "transfert_vers"
    double montant;
    char date[30];
    int destinataireId;     // ID de l'autre compte (si transfert), sinon 0
} Historique;

#endif
