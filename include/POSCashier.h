#pragma once // Ensure the header is only included once

#include "utilities.h"
class POSAdmin;

class POSCashier {
public:
    // we declare all member functions of POSCashier here (the ones that will be defined in POSCashier.cpp)

    // CREATE
    void saveTransaction(string productNames, string productQuantities, double initialAmount, double totalAmount, double change, string cashierName);
    bool processTransaction(string username);

    // READ
    bool viewCart(string username);
    bool readProductsBySubcategory(string productsDatabase, string subCategory, string username);

    // UPDATE
    void deductPurchasedQuantities(string productsDatabase, string query, string username, string quantities);
};