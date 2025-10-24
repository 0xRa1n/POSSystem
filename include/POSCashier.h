#pragma once // Ensure the header is only included once

#include <iostream>
#include <vector>
#include <string>

#include "utilities.h"
class POSAdmin;

class POSCashier {
public:
    // we declare all member functions of POSCashier here
    bool viewCart(string username);
    void saveTransaction(string productNames, string productQuantities, double initialAmount, double totalAmount, double change, string cashierName);
    bool processTransaction(string username);
    bool readProductsBySubcategory(string productsDatabase, string subCategory, string username);
    void deductPurchasedQuantities(string productsDatabase, string query, string username, string quantities);
};