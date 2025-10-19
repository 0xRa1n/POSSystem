#pragma once // Ensure the header is only included once

#include <iostream>
#include <vector>
#include <string>

#include "utilities.h"
class POSAdmin;

class POSCashier {
public:
    void viewCart(string username);
    void saveTransaction(string productNames, string productQuantities, int totalAmount, string cashierName);
    void processTransaction(vector<string>& productNames, vector<int>& productQuantities, vector<int>& productPrices, string username);
    bool readProductsBySubcategory(string productsDatabase, string subCategory, string username);
};