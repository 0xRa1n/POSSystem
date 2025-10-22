#pragma once // Ensure the header is only included once

#include <iostream>
#include "utilities.h"

class POSAdmin {
public:
    // --- DECLARATIONS ONLY ---
    void saveLogs(string type, string operation, string affectedEntry, string adminName, string message);

    // CREATE
    void addProduct(string database, string username);
    void addUser(string database, string accessingUsername);

    // READ
    void readProducts(string database);
    void getTotalSales(string database);
    void getTodaysSales(string database);
    void getAllAccounts(string database);
    void getAllLogs(string type);

    // UPDATE
    int updateInformation(string filename, string query, string type, string newValue, string username, string reason);
    void updateProductFields(string type, string database, string field, string username);
    void updateDiscounts(string username);

    // DELETE
    void deleteInformation(string type, string filename, string username);
};