#pragma once // Ensure the header is only included once

#include <iostream>
#include <vector>
#include <string>

#include "utilities.h"
class POSAdmin;

class POSCashier {
public:
    bool viewCart(string username);
    void saveTransaction(string productNames, string productQuantities, int totalAmount, string cashierName);
    bool processTransaction(string username);
    bool readProductsBySubcategory(string productsDatabase, string subCategory, string username);
};