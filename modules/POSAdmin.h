#pragma once // Use pragma once instead of manual include guards

#include <iostream>
// ... other necessary includes ...
#include "utilities.h"

class POSAdmin {
public:
    // --- DECLARATIONS ONLY ---
    void saveLogs(string type, string operation, string affectedEntry, string adminName);

    // CREATE
    void addProduct(string database, string username);
    void addUser(string database, string accessingUsername);

    // READ
    void readProducts(string database);
    void getTotalSales(string database);
    void getTodaysSales(string database);

    // UPDATE
    int updateInformation(string filename, string query, string type, string newValue, string username);
    void updateProductFields(string type, string database, string field, string username);

    // DELETE
    void deleteInformation(string type, string filename, string username);
    // ... any other method declarations ...
};