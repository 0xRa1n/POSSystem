#pragma once // Use pragma once instead of manual include guards

#include <iostream>
#include <vector>
#include <string>

#include "utilities.h"
class POSAdmin;

class POSCashier {
public:
    // --- DECLARATIONS ONLY ---
    void viewCart(string username);
    void saveTransaction(string productNames, string productQuantities, int totalAmount, string cashierName);
    void processTransaction(vector<string>& productNames, vector<int>& productQuantities, vector<int>& productPrices, string username);
    void readProductsBySubcategory(string productsDatabase, string subCategory, string username);
    // ... any other method declarations ...
};