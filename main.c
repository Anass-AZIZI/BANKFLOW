#include <stdio.h>
#include <string.h>
#include "compte.h"
#include "interface.h"

int main() {
    // Chargement des donnees
    int n = load_accounts();
    
    // Creation automatique de l'admin si le systeme est vide
    if (n == 0) {
        printf("Systeme vide. Creation du compte Admin par defaut...\n");
        Compte admin;
        admin.id = 1;
        strcpy(admin.password, "admin");
        strcpy(admin.date_creation, "2023-01-01");
        strcpy(admin.nom, "Administrateur");
        strcpy(admin.cin, "ADMIN001");
        strcpy(admin.date_naissance, "2000-01-01");
        admin.age = 30;
        strcpy(admin.adresse, "Banque Centrale");
        strcpy(admin.telephone, "0000000000");
        strcpy(admin.citoyennete, "N/A");
        admin.type_compte = 0;
        admin.solde = 1000000.00;
        admin.locked = 0;
        admin.role = 1; // ADMIN

        // On ajoute manuellement au tableau global dans compte.c
        // Comme on ne peut pas acceder a g_comptes directement, on utilise une astuce.
        // Pour faire simple, on ajoute au fichier et on recharge.
        
        FILE *f = fopen("comptes.txt", "w");
        if (f) {
            fprintf(f, "%d,%s,%s,%s,%s,%s,%d,%s,%s,%s,%d,%.2f,%d,%d\n",
                admin.id, admin.password, admin.date_creation, admin.nom, admin.cin, admin.date_naissance,
                admin.age, admin.adresse, admin.telephone, admin.citoyennete,
                admin.type_compte, admin.solde, admin.locked, admin.role);
            fclose(f);
        }
        n = load_accounts(); // On recharge
        printf("Compte Admin cree: ID=1, Password=admin\n");
    }

    printf("Systeme initialise. %d comptes charges.\n", n);
    
    // Lancement de l'interface
    main_menu();
    
    return 0;
}
