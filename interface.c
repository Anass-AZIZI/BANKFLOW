#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "compte.h"
#include "operations.h"
#include "historique.h"

// --- Fonctions d'aide pour l'affichage ---

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void print_centered(const char* text) {
    int width = 80; // Largeur standard du terminal
    int len = strlen(text);
    int padding = (width - len) / 2;
    if (padding < 0) padding = 0;
    
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s\n", text);
}

void print_line() {
    for (int i = 0; i < 80; i++) printf("*");
    printf("\n");
}

void print_header() {
    clear_screen();
    printf("\n");
    // Dessin ASCII pour BANKFLOW
    printf("  ____    _    _   _ _  _______ _     _____  __        __ \n");
    printf(" | __ )  / \\  | \\ | | |/ /  ___| |   / _ \\ \\      / / \n");
    printf(" |  _ \\ / _ \\ |  \\| | ' /| |_  | |  | | | \\ \\ /\\ / /  \n");
    printf(" | |_) / ___ \\| |\\  | . \\|  _| | |__| |_| |\\ V  V /   \n");
    printf(" |____/_/   \\_\\_| \\_|_|\\_\\_|   |_____\\___/  \\_/\\_/    \n");
    printf("\n");
    print_centered("SYSTEME DE GESTION BANCAIRE");
    print_line();
    printf("\n");
}

// --- MENUS ---

void main_menu() {
    int choice;
    while (1) {
        print_header();
        print_centered("MENU PRINCIPAL");
        printf("\n");
        print_centered("1. Connexion");
        print_centered("2. Demande d'ouverture de compte");
        print_centered("3. Quitter");
        printf("\n");
        print_line();
        printf(" Votre choix: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // On vide le buffer
            continue;
        }

        switch (choice) {
            case 1: login_screen(); break;
            case 2: register_screen(); break;
            case 3: exit(0);
            default: break;
        }
    }
}

void register_screen() {
    Compte c;
    c.id = 0; 
    c.solde = 0.0;
    c.locked = 0;
    c.role = 0; 
    
    print_header();
    print_centered("INSCRIPTION");
    printf("\n");
    
    printf(" Nom complet: ");
    scanf(" %[^\n]", c.nom);
    printf(" CIN: ");
    scanf("%s", c.cin);
    printf(" Mot de passe: ");
    scanf("%s", c.password);
    printf(" Date Naissance (YYYY-MM-DD): ");
    scanf("%s", c.date_naissance);
    printf(" Age: ");
    scanf("%d", &c.age);
    printf(" Adresse: ");
    scanf(" %[^\n]", c.adresse);
    printf(" Telephone: ");
    scanf("%s", c.telephone);
    printf(" Citoyennete: ");
    scanf("%s", c.citoyennete);
    printf(" Type Compte (0=Courant, 1=Epargne): ");
    scanf("%d", &c.type_compte);
    
    strcpy(c.date_creation, "2023-01-01"); 

    create_account_request(c);
    
    printf("\n");
    print_centered("Demande envoyee avec succes !");
    print_centered("Appuyez sur Entree pour revenir...");
    getchar(); getchar();
}

void login_screen() {
    int id;
    char password[50];
    
    print_header();
    print_centered("CONNEXION");
    printf("\n");
    
    printf(" ID Compte: ");
    scanf("%d", &id);
    printf(" Mot de passe: ");
    scanf("%s", password);
    
    Compte *c = get_account_by_id(id);
    
    if (c != NULL && strcmp(c->password, password) == 0) {
        if (c->locked) {
            printf("\n");
            print_centered("! COMPTE VERROUILLE !");
            getchar(); getchar();
            return;
        }
        
        if (c->role == 1) {
            admin_menu(c);
        } else {
            client_menu(c);
        }
    } else {
        printf("\n");
        print_centered("! Identifiants incorrects !");
        getchar(); getchar();
    }
}

void client_menu(Compte *c) {
    int choice;
    while (1) {
        print_header();
        char buffer[100];
        sprintf(buffer, "Bienvenue, %s", c->nom);
        print_centered(buffer);
        sprintf(buffer, "Solde: %.2f DH", c->solde);
        print_centered(buffer);
        printf("\n");
        
        print_centered("1. Depot");
        print_centered("2. Retrait");
        print_centered("3. Virement");
        print_centered("4. Historique");
        print_centered("5. Deconnexion");
        printf("\n");
        print_line();
        printf(" Votre choix: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }
        
        double amount;
        int destId;
        
        switch (choice) {
            case 1:
                printf("\n Montant a deposer: ");
                scanf("%lf", &amount);
                if (deposit(c->id, amount)) {
                    printf(" Succes! Nouveau solde: %.2f\n", c->solde);
                }
                getchar(); getchar();
                break;
            case 2:
                printf("\n Montant a retirer: ");
                scanf("%lf", &amount);
                if (withdraw(c->id, amount)) {
                    printf(" Succes! Nouveau solde: %.2f\n", c->solde);
                }
                getchar(); getchar();
                break;
            case 3:
                printf("\n ID Destinataire: ");
                scanf("%d", &destId);
                printf(" Montant: ");
                scanf("%lf", &amount);
                if (transfer(c->id, destId, amount)) {
                    printf(" Succes! Nouveau solde: %.2f\n", c->solde);
                }
                getchar(); getchar();
                break;
            case 4:
                print_history(c->id);
                printf("\n Appuyez sur Entree...");
                getchar(); getchar();
                break;
            case 5:
                return;
            default:
                break;
        }
    }
}

void admin_menu(Compte *admin) {
    int choice;
    while (1) {
        print_header();
        print_centered("ESPACE ADMINISTRATEUR");
        printf("\n");
        
        print_centered("1. Traiter les demandes");
        print_centered("2. Supprimer un compte");
        print_centered("3. Liste des comptes");
        print_centered("4. Deconnexion");
        printf("\n");
        print_line();
        printf(" Votre choix: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }
        
        int idDel;
        
        switch (choice) {
            case 1:
                process_requests();
                printf("\n Appuyez sur Entree...");
                getchar(); getchar();
                break;
            case 2:
                printf("\n ID du compte a supprimer: ");
                scanf("%d", &idDel);
                delete_account(idDel);
                getchar(); getchar();
                break;
            case 3:
                printf("\n --- LISTE DES COMPTES ---\n");
                for(int i=1; i<100; i++) {
                    Compte* temp = get_account_by_id(i);
                    if(temp) printf(" ID: %d | Nom: %-20s | Solde: %.2f | Role: %s\n", 
                        temp->id, temp->nom, temp->solde, temp->role ? "ADMIN" : "CLIENT");
                }
                printf("\n Appuyez sur Entree...");
                getchar(); getchar();
                break;
            case 4:
                return;
            default:
                break;
        }
    }
}
