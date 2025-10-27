#pragma once // Ensure the header is only included once

#include "utilities.h"
class POSAdmin;

class POSCashier {
public:
    // we declare all member functions of POSCashier here (the ones that will be defined in POSCashier.cpp)

    // CREATE
    void saveTransaction(string productNames, string productQuantities, double initialAmount, double totalAmount, double discount, double moneyTendered, double change, string cashierName, string paymentMethod, int referenceID); // used to log transactions on the transactions folder
    bool processTransaction(string username); // used to verify the transaction, and afterwards, use the function saveTransaction to log the transaction
    void saveAbandonedCarts(string productNames, string productQuantities);

    // READ
    bool viewCart(string username); // used to view the items in the cart
    bool readProductsBySubcategory(string productsDatabase, string subCategory, string username); // used to get the products from a specific sub-category

    // UPDATE
    void deductPurchasedQuantities(string productsDatabase, string query, string username, string quantities); // used to deduct the purchased quantities from the inventory
}; // semicolon to indicate the end of class declaration