#include <stdio.h>
#include <string.h>
#include <time.h>
#include "structures.h"
#include "historique.h"

// Fonction pour avoir la date actuelle
void get_current_date(char *buffer, int size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void add_history(int idCompte, const char *type, double montant, int destId) {
    FILE *f = fopen("historique.txt", "a");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir historique.txt\n");
        return;
    }

    char date[30];
    get_current_date(date, sizeof(date));

    // Format: ID,Type,Montant,Date,DestID
    fprintf(f, "%d,%s,%.2f,%s,%d\n", idCompte, type, montant, date, destId);
    fclose(f);
}

int load_history(const char *filename, Historique tab[], int max) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) return 0;

    int count = 0;
    char buffer[256];
    
    while (count < max && fgets(buffer, sizeof(buffer), f)) {
        // On lit la ligne
        // Format: %d,%[^,],%lf,%[^,],%d
        sscanf(buffer, "%d,%19[^,],%lf,%29[^,],%d", 
               &tab[count].idCompte,
               tab[count].typeOperation,
               &tab[count].montant,
               tab[count].date,
               &tab[count].destinataireId);
        count++;
    }
    fclose(f);
    return count;
}

void print_history(int idCompte) {
    Historique history[MAX_HISTORIQUE];
    int count = load_history("historique.txt", history, MAX_HISTORIQUE);
    
    printf("\n--- Historique du compte %d ---\n", idCompte);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (history[i].idCompte == idCompte) {
            printf("[%s] %s : %.2f", history[i].date, history[i].typeOperation, history[i].montant);
            if (history[i].destinataireId != 0) {
                printf(" (Dest: %d)", history[i].destinataireId);
            }
            printf("\n");
            found = 1;
        }
    }
    if (!found) {
        printf("Aucune operation trouvee.\n");
    }
    printf("-------------------------------\n");
}

void save_history(const char *filename, Historique tab[], int n) {
    // Pas vraiment necessaire si on ajoute a la fin, mais utile pour nettoyer
    FILE *f = fopen(filename, "w");
    if (f == NULL) return;
    
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d,%s,%.2f,%s,%d\n", 
                tab[i].idCompte, 
                tab[i].typeOperation, 
                tab[i].montant, 
                tab[i].date, 
                tab[i].destinataireId);
    }
    fclose(f);
}