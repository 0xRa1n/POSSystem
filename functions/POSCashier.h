#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

class POSCashier {
    public:
    // this part is related to adding an a product
    int getLastId(string filename) {
        ifstream fin(filename);
        string line;
        int lastId = 0;

        while (getline(fin, line)) {
            if (line.empty()) continue; // skip empty lines
            stringstream ss(line); 
            string idStr;
            if (getline(ss, idStr, ',')) {
                try {
                    int id = stoi(idStr);
                    if (id > lastId) lastId = id; // update lastId if current id is greater
                // if it fails, do nothing
                } catch (...) {}
            }
        }
        // return the last ID found
        return lastId;
    }

    bool isAlreadyInCsv(string filename, string productNameToCheck) {
        ifstream fin(filename);
        string line;

        while (getline(fin, line)) {
            if (line.find(productNameToCheck) != string::npos) {
                return true;  // Product name found somewhere in the database
            }
        }
        return false;
    }

    // CRUD-Related Functions
    void processTransaction(string productName, int productQuantity, int productPrice){
        char confirmation;
        // make a function that will first print the order summary
        system("cls");
        cout << "---------------------------------" << endl;
        cout << "P.O.S (Cashier)" << endl ;
        cout << "---------------------------------" << endl;

        cout << "Order Summary" << endl;
        cout << "\nProduct Name: " << productName << endl;
        cout << "Quantity: " << productQuantity << endl;
        cout << "Price: " << productPrice << endl;

        cout << "\nProceed to purchase? (Y/N): ";
        cin >> confirmation;

        if(toupper(confirmation) == 'Y'){
            system("cls");
            cout << "---------------------------------" << endl;
            cout << "P.O.S (Cashier)" << endl ;
            cout << "---------------------------------" << endl;

            cout << "Order Receipt" << endl;
            cout << "\nProduct Name: " << productName << endl;
            cout << "Quantity: " << productQuantity << "\n\n";

            cout << "Total Amount: " << productPrice * productQuantity << endl;
            cout << "VAT (12%): " << (productPrice * productQuantity) * 0.12 << endl;
            cout << "Amount Due: " << (productPrice * productQuantity) + ((productPrice * productQuantity) * 0.12) << endl;

        } else {
            cout << "Purchase cancelled." << endl;
        }
        cout << "---------------------------------" << endl;
        cout << "Press any key to create another purchase\n" << endl;
        getch();

    }

    void readProductsBySubcategory(string productsDatabase, string subCategory){
        int quantityToPurchase;
        ifstream file(productsDatabase);
              
        if (!file.is_open()) {
            cout << "Failed to open file\n";
            cout << "Press any key to continue...";
            getch();
            return;
        }

        // Read and filter T-Shirts products
        vector<vector<string>> tshirtRows;
        string line;
        
        // Skip header and read all lines
        getline(file, line); // skip header
        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            vector<string> row;
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            // Only add if ProductSubCategory is T_Shirts (index 2)
            if (row.size() > 2 && row[2] == subCategory) { // only change this value if you want to search for a different subcategory
                tshirtRows.push_back(row);
            }
        }
        file.close();

        if (tshirtRows.empty()) {
            cout << "No T-Shirts available.\n";
            cout << "Press any key to continue...";
            getch();
            return;
        }

        // Display T-Shirts with proper formatting
        cout << "ID    Product Name           Quantity  Price\n";
        cout << "----  --------------------   --------  -----\n";
        
        for (auto &row : tshirtRows) {
            cout << left << setw(6) << row[0]        // ID
                << setw(23) << row[1]               // Product Name
                << setw(10) << row[3]               // Quantity
                << row[4] << "\n";                  // Price
        }

        // Get user input for product ID
        cout << "\nEnter product ID to select (0 to go back): ";
        int selectedId;
        cin >> selectedId;

        if (selectedId == 0) return;

        // Find and display selected product
        string productName;
        int productQuantity, productPrice;
        bool found = false;
        for (auto &row : tshirtRows) {
            if (stoi(row[0]) == selectedId) {
                productName = row[1];
                productQuantity = stoi(row[3]);
                productPrice = stoi(row[4]);

                cout << "\nSelected Product:\n";
                cout << "Product Name: " << productName << "\n";
                cout << "Price: " << productPrice << "\n";
                found = true;
                break;
            }
        }

        cout << "\nEnter quantity to purchase: ";
        cin >> quantityToPurchase;

        if (!found) {
            cout << "Invalid product ID!\n";
        }
        processTransaction(productName, quantityToPurchase, productPrice);
    }
};