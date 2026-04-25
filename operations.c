#include <stdio.h>
#include <string.h> 
#include "structures.h"     
#include "operations.h"     
#include "compte.h"         
#include "historique.h"     


/*
 vérifier si le montant est positif.
 */
int validate_amount(double amount) {
    if (amount <= 0.0) { // On ne peut pas déposer 0 ou moins.
        printf("Erreur (Op): Le montant doit etre positif.\n");
        return 0;
    }
    return 1;
}


/*
 * Logique pour déposer de l'argent
 */
int deposit(int accountId, double amount) {
    
    printf("LOG (Op): Depot de %.2f sur le compte %d...\n", amount, accountId);
    
    // Étape 1: Vérifier si le montant est valide.
    if (validate_amount(amount) == 0) {
        return 0; // Échec
    }

    // Étape 2: Demander au module 'compte' de trouver le compte.
    // (On suppose que get_account_by_id existe dans compte.c)
    Compte* ptrCompte = get_account_by_id(accountId); 
    
    // Étape 3: Vérifier si on l'a trouvé.
    if (ptrCompte == NULL) {
        printf("Erreur (Op): Compte ID %d non trouve.\n", accountId);
        return 0; // Échec
    }
    
    // Étape 4: Vérifier s'il est verrouillé.
    if (ptrCompte->locked == 1) {
        printf("Erreur (Op): Le compte %d est verrouille. Depot impossible.\n", accountId);
        return 0; // Échec
    }

    // OK, on peut faire l'opération :
    
    // On change le solde en mémoire
    ptrCompte->solde = ptrCompte->solde + amount;
    
    // On dit au module 'historique' d'enregistrer ce qu'on a fait
    add_history(accountId, "depot", amount, 0); 

    // On dit au module 'compte' de sauvegarder tous les changements dans le fichier txt
    save_all_accounts(); 
    
    printf("LOG (Op): Depot reussi. Nouveau solde: %.2f\n", ptrCompte->solde);
    return 1; // Succès
}


/*
 * Logique pour retirer de l'argent
 */
int withdraw(int accountId, double amount) {
    
    printf("LOG (Op): Tentative de retrait de %.2f du compte %d...\n", amount, accountId);

    // Étape 1: Montant valide ?
    if (validate_amount(amount) == 0) {
        return 0;
    }

    // Étape 2: Trouver le compte (comme pour deposit)
    Compte* ptrCompte = get_account_by_id(accountId); 
    
    // Étape 3: Il existe ?
    if (ptrCompte == NULL) {
        printf("Erreur (Op): Compte ID %d non trouve.\n", accountId);
        return 0;
    }
    
    // Étape 4: Il est verrouillé ?
    if (ptrCompte->locked == 1) {
        printf("Erreur (Op): Le compte %d est verrouille. Retrait impossible.\n", accountId);
        return 0;
    }

    // Étape 5: (LA VÉRIF IMPORTANTE) Assez d'argent ?
    if (ptrCompte->solde < amount) {
        printf("Erreur (Op): Solde insuffisant (Solde: %.2f, Demande: %.2f).\n", ptrCompte->solde, amount);
        return 0;
    }

    // OK, on peut faire le retrait :
    
    // On change le solde
    ptrCompte->solde = ptrCompte->solde - amount;
    
    // On enregistre dans l'historique
    add_history(accountId, "retrait", amount, 0); 

    // On sauvegarde tout
    save_all_accounts(); 
    
    // Message de succès
    printf("LOG (Op): Le retrait d'un montant de %.2f a ete effectue sur le compte %d par succes.\n", amount, accountId);
    printf("LOG (Op): Nouveau solde: %.2f\n", ptrCompte->solde);
    
    return 1; // Succès
}


/*
 * Logique pour un virement (le plus dur)
 */
int transfer(int fromAccountId, int toAccountId, double amount) {
    
    printf("LOG (Op): Tentative de virement de %.2f de %d vers %d...\n", amount, fromAccountId, toAccountId);
    
    // Étape 1: Montant valide ?
    if (validate_amount(amount) == 0) {
        return 0;
    }

    // On vérifie que ce n'est pas le même compte
    if (fromAccountId == toAccountId) {
        printf("Erreur (Op): Un compte ne peut pas se faire un virement a lui-meme.\n");
        return 0;
    }

    // Étape 2: Trouver l'expéditeur
    Compte* ptrCompteFrom = get_account_by_id(fromAccountId);
    
    // Étape 3: Trouver le destinataire
    Compte* ptrCompteTo = get_account_by_id(toAccountId);

    // Étape 4: Vérifier que les deux existent et ne sont pas verrouillés
    if (ptrCompteFrom == NULL) {
        printf("Erreur (Op): Compte expediteur %d non trouve.\n", fromAccountId);
        return 0;
    }
    if (ptrCompteTo == NULL) {
        printf("Erreur (Op): Compte destinataire %d non trouve.\n", toAccountId);
        return 0;
    }
    
    if (ptrCompteFrom->locked == 1) {
        printf("Erreur (Op): Compte expediteur %d est verrouille.\n", fromAccountId);
        return 0;
    }
    if (ptrCompteTo->locked == 1) {
        printf("Erreur (Op): Compte destinataire %d est verrouille. Virement impossible.\n", toAccountId);
        return 0;
    }

    // Étape 5: Vérifier si l'expéditeur a assez d'argent
    if (ptrCompteFrom->solde < amount) {
        printf("Erreur (Op): Solde insuffisant pour le compte %d (Solde: %.2f, Demande: %.2f).\n", fromAccountId, ptrCompteFrom->solde, amount);
        return 0;
    }

    // C'est bon, on fait l'opération
    
    // On retire l'argent du premier...
    ptrCompteFrom->solde = ptrCompteFrom->solde - amount;
    
    // ...et on le donne au second.
    ptrCompteTo->solde = ptrCompteTo->solde + amount;
    
    // On enregistre l'historique pour l'expéditeur
    add_history(fromAccountId, "transfert_de", amount, toAccountId);
    
    // On enregistre l'historique pour le destinataire
    add_history(toAccountId, "transfert_vers", amount, fromAccountId);
    
    // On sauvegarde tout (UNE SEULE FOIS !)
    save_all_accounts();

    // Message de succès
    printf("LOG (Op): Virement de %.2f de %d vers %d reussi.\n", amount, fromAccountId, toAccountId);
    printf("LOG (Op): Nouveau solde expediteur (%d): %.2f\n", fromAccountId, ptrCompteFrom->solde);
    printf("LOG (Op): Nouveau solde destinataire (%d): %.2f\n", toAccountId, ptrCompteTo->solde);

    return 1;
}