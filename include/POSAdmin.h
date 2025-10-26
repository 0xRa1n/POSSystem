#pragma once // Ensure the header is only included once

#include "utilities.h"

class POSAdmin {
public:
    // we declare all member functions of POSAdmin here (the ones that will be defined in POSAdmin.cpp)

    // CREATE
    void addProduct(string database, string username); // used to add products to the inventory 
    void addUser(string database, string accessingUsername); // used to add new user accounts
    void saveLogs(string type, string operation, string affectedEntry, string adminName, string message); // used to save logs for auditing purposes
    void saveRefundLogs(int transactionID, string productNames, int productQuantities, double amountRefunded, string transactionPaymentMethod, string username); // used to save logs for refunds

    // READ
    void readProducts(string database); // read and display all products from the inventory
    void readRefundLogs(); // read and display all refund logs
    void getDailySales(); // get the daily sales from the database
    void getTotalSales(); // get the total sales from the database
    void getMonthlySales(); // get the monthly sales from the database
    void getYearlySales(); // get the yearly sales from the database
    void getAllAccounts(string database); // get all user accounts from the database
    void getAllLogs(string type); // get all logs of a specific type
    void readBackupTransactions(string database); // read and display the backup transaction from a power outage or system crash
    void readProductsByCategory(string productsDatabase, string category, string username);

    // UPDATE
    void updateProduct(string filename, string query, string valueToUpdate, string type, string username); // used to update product's name, price, quantity, category, or sub-category
    void updateAccount(string filename, string type, string username); // used to update user's password, role, or status
    void updateDiscounts(string username); // used to update discounts on a product category
    void processRefunds(string username); // used to process refunds for a transaction

    // DELETE
    void deleteInformation(string type, string filename, string username); // used to delete products or accounts from the database
}; // semicolon to indicate the end of class declaration