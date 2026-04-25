#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "compte.h"

const char* FILE_COMPTES = "comptes.txt";
const char* FILE_DEMANDES = "demandes_creation.txt";

// Variables globales pour stocker les comptes en memoire
Compte g_comptes[MAX_COMPTES];
int g_num_comptes = 0;
int g_loaded = 0; // Flag pour verifier si c'est charge

// Fonction pour s'assurer que les donnees sont chargees
void ensure_loaded() {
    if (!g_loaded) {
        load_accounts();
        g_loaded = 1;
    }
}

int load_accounts() {
    FILE *f = fopen(FILE_COMPTES, "r");
    if (f == NULL) {
        g_num_comptes = 0;
        return 0;
    }

    g_num_comptes = 0;
    char buffer[512];
    
    // Format: id,pass,date_crea,nom,cin,date_nais,age,adr,tel,cit,type,solde,locked,role
    while (g_num_comptes < MAX_COMPTES && fgets(buffer, sizeof(buffer), f)) {
        Compte *c = &g_comptes[g_num_comptes];
        sscanf(buffer, "%d,%49[^,],%19[^,],%49[^,],%19[^,],%19[^,],%d,%99[^,],%19[^,],%19[^,],%d,%lf,%d,%d",
               &c->id, c->password, c->date_creation, c->nom, c->cin, c->date_naissance,
               &c->age, c->adresse, c->telephone, c->citoyennete,
               &c->type_compte, &c->solde, &c->locked, &c->role);
        g_num_comptes++;
    }
    fclose(f);
    return g_num_comptes;
}

void save_all_accounts() {
    FILE *f = fopen(FILE_COMPTES, "w");
    if (f == NULL) {
        printf("Erreur: Impossible de sauvegarder les comptes.\n");
        return;
    }

    for (int i = 0; i < g_num_comptes; i++) {
        Compte *c = &g_comptes[i];
        fprintf(f, "%d,%s,%s,%s,%s,%s,%d,%s,%s,%s,%d,%.2f,%d,%d\n",
               c->id, c->password, c->date_creation, c->nom, c->cin, c->date_naissance,
               c->age, c->adresse, c->telephone, c->citoyennete,
               c->type_compte, c->solde, c->locked, c->role);
    }
    fclose(f);
}

Compte* get_account_by_id(int id) {
    ensure_loaded();
    for (int i = 0; i < g_num_comptes; i++) {
        if (g_comptes[i].id == id) {
            return &g_comptes[i];
        }
    }
    return NULL;
}

void create_account_request(Compte newAccount) {
    FILE *f = fopen(FILE_DEMANDES, "a");
    if (f == NULL) return;

    // On sauvegarde la demande avec le meme format que le compte
    fprintf(f, "%d,%s,%s,%s,%s,%s,%d,%s,%s,%s,%d,%.2f,%d,%d\n",
            newAccount.id, newAccount.password, newAccount.date_creation, newAccount.nom, 
            newAccount.cin, newAccount.date_naissance, newAccount.age, newAccount.adresse, 
            newAccount.telephone, newAccount.citoyennete, newAccount.type_compte, 
            newAccount.solde, newAccount.locked, newAccount.role);
    
    fclose(f);
    printf("Demande de creation envoyee a l'admin.\n");
}

void process_requests() {
    // On lit toutes les demandes
    FILE *f = fopen(FILE_DEMANDES, "r");
    if (f == NULL) {
        printf("Aucune demande en attente.\n");
        return;
    }

    Compte requests[50];
    int num_req = 0;
    char buffer[512];

    while (num_req < 50 && fgets(buffer, sizeof(buffer), f)) {
        Compte *c = &requests[num_req];
        sscanf(buffer, "%d,%49[^,],%19[^,],%49[^,],%19[^,],%19[^,],%d,%99[^,],%19[^,],%19[^,],%d,%lf,%d,%d",
               &c->id, c->password, c->date_creation, c->nom, c->cin, c->date_naissance,
               &c->age, c->adresse, c->telephone, c->citoyennete,
               &c->type_compte, &c->solde, &c->locked, &c->role);
        num_req++;
    }
    fclose(f);

    if (num_req == 0) {
        printf("Aucune demande en attente.\n");
        return;
    }

    // On traite chaque demande
    FILE *f_new = fopen(FILE_DEMANDES, "w"); // On vide le fichier, on reecrira ceux qui restent
    
    for (int i = 0; i < num_req; i++) {
        printf("\nDemande %d:\nNom: %s, CIN: %s, Solde initial: %.2f\n", i+1, requests[i].nom, requests[i].cin, requests[i].solde);
        printf("Accepter (1) ou Rejeter (0) ? ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            // On ajoute aux comptes globaux
            ensure_loaded();
            // On donne un nouvel ID unique
            requests[i].id = get_next_account_id();
            g_comptes[g_num_comptes++] = requests[i];
            save_all_accounts();
            printf("Compte cree avec ID %d.\n", requests[i].id);
        } else {
            printf("Demande rejetee.\n");
            // On pourrait garder une trace, mais ici on supprime.
        }
    }
    fclose(f_new); // Le fichier est vide donc les demandes traitees disparaissent
}

void delete_account(int id) {
    ensure_loaded();
    int found_idx = -1;
    for (int i = 0; i < g_num_comptes; i++) {
        if (g_comptes[i].id == id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1) {
        // On decale les comptes suivants pour combler le vide
        for (int i = found_idx; i < g_num_comptes - 1; i++) {
            g_comptes[i] = g_comptes[i+1];
        }
        g_num_comptes--;
        save_all_accounts();
        printf("Compte %d supprime.\n", id);
    } else {
        printf("Compte non trouve.\n");
    }
}

int get_next_account_id() {
    ensure_loaded();
    int max_id = 0;
    for (int i = 0; i < g_num_comptes; i++) {
        if (g_comptes[i].id > max_id) max_id = g_comptes[i].id;
    }
    return max_id + 1;
}