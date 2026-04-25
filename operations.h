#ifndef OPERATIONS_H
#define OPERATIONS_H

// Verifie si le montant est positif
int validate_amount(double amount);

// Deposer de l'argent sur le compte
int deposit(int accountId, double amount);

// Retirer de l'argent du compte
int withdraw(int accountId, double amount);

// Virer de l'argent entre comptes
int transfer(int fromAccountId, int toAccountId, double amount);

#endif
