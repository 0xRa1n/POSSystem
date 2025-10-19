#pragma once // Ensure the header is only included once

#include <iostream>
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
    void getAllAccounts(string database);

    // UPDATE
    int updateInformation(string filename, string query, string type, string newValue, string username);
    void updateProductFields(string type, string database, string field, string username);

    // DELETE
    void deleteInformation(string type, string filename, string username);
};