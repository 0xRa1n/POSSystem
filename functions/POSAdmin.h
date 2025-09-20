#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

class POSAdmin {
    public:
    // this part is related to adding an a product
    int getLastId(const string& filename) {
        ifstream fin(filename);
        string line;
        int lastId = 0;

        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr;
            if (getline(ss, idStr, ',')) {
                try {
                    int id = stoi(idStr);
                    if (id > lastId) lastId = id;
                // if it fails, do nothing
                } catch (...) {}
            }
        }
        // return the last ID found
        return lastId;
    }

    bool isAlreadyInCsv(const string& filename, const string& productNameToCheck) {
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
    // ampersand is added so that it will make sure that the filename argument would not be modified
    void addProduct(const string& database) {
        string productName;
        int quantity, price;

        // ask the user for the product name (if 0 is entered, it will go back to the menu)
        cout << "Enter the product name (type 0 to return): ";
        cin >> productName;

        if (productName == "0") return;

        // ask the user for the product's quantity and price
        cout << "Enter the quantity: ";
        cin >> quantity;
        cout << "Enter the price: ";
        cin >> price;

        // use the function to check if the entry is already in the database
        if (isAlreadyInCsv(database, productName)) {
            cout << "Product '" << productName << "' is already in the CSV.\n";
        } else {
            // get the last product id from the database
            int lastId = getLastId(database);
            int newId = lastId + 1;

            // open the database file, and add the new product
            fstream fout;
            fout.open(database, ios::out | ios::app);

            fout << newId << ","
                << productName << ","
                << quantity << ","
                << price << "\n";

            fout.close();
            cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
        }

        // wait for 1.2 seconds to go back to the main menu
        Sleep(1200);
    }

    void addUser(const string& database){
        string username, password, role;
                                
        // ask the user for the username (if 0 is entered, it will go back to the menu)
        cout << "Enter the username you want to add (type 0 to return): ";
        cin >> username;

        if (username == "0") return;

        // ask for the password and role
        cout << "Enter the password: ";
        cin >> password;
        cout << "What is the role of the user? (Admin/Manager/Cashier): ";
        cin >> role;

        // use the function to check if the entry is already in the database
        if(isAlreadyInCsv(database, username)){
            cout << "User is already in the database!";
        } else {
            // get the last user id from the dtabase
            int lastUserId = getLastId(database);
            int newUserId = lastUserId + 1;

            fstream fout;
            fout.open(database, ios::out | ios::app);

            // open the database file, and add the new product
            fout << newUserId << ","
                << username << ","
                << password << ","
                << role << "\n";

            fout.close();
            cout << "Successfully added user " << username << endl;
        }
        // wait for 1.2 seconds before going back to the menu
        Sleep(1200);
    }

    void readProducts(const string& database) {
        ifstream file(database);
        if (!file.is_open()) {
            cout << "Failed to open file\n";
            return;
        }

        // Read all rows first
        vector<vector<string>> rows;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            vector<string> row;
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            rows.push_back(row);
        }
        file.close();

        // stop if the rows are empty
        if (rows.empty()) return;

        // Find max width of each column
        size_t cols = 0;
        for (auto &r : rows) cols = max(cols, r.size());
        vector<size_t> widths(cols, 0);
        for (auto &r : rows) {
            for (size_t c = 0; c < r.size(); ++c)
                widths[c] = max(widths[c], r[c].size());
        }

        // Add a little padding for readability
        for (auto &w : widths) w += 2;

        // Print
        for (auto &r : rows) {
            for (size_t c = 0; c < r.size(); ++c) {
                cout << left << setw(static_cast<int>(widths[c])) << r[c];
            }
            cout << '\n';
        }
    }

    void deleteInformation(const string& filename){
        string deleteProductInput;

        cout << "Enter the entry name you want to delete (type 0 to return): ";
        cin >> deleteProductInput;

        if (deleteProductInput == "0") return;

        ifstream input_file(filename);
        vector<string> rows;
        string line;
        bool found = false;

        // Read each line and filter out the one with the username
        while (getline(input_file, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ',');     // read id (not used)
            getline(ss, token, ',');     // read username
            if(token != deleteProductInput){
                rows.push_back(line);
            } else {
                found = true;
            }
        }
        input_file.close();

        if (!found) {
            cout << "Query not found";
        } else {
            // Write back filtered rows
            ofstream output_file(filename, ios::trunc);
            for (const auto& row : rows) {
                output_file << row << "\n";
            }
            output_file.close();

            cout << "Successfully deleted " << deleteProductInput << endl;
        }
        Sleep(1200);
    }
        
    void updateInformation(const string& filename, const string& query, const string& type, const string& newValue) {
        ifstream fileIn(filename);
        if (!fileIn) {
            cout << "Cannot open file " << filename << endl;
        }

        string fileContent, line;
        bool found = false;

        while (getline(fileIn, line)) {
            stringstream ss(line);
            string indexOne, indexTwo, indexThree, indexFour;
            getline(ss, indexOne, ',');
            getline(ss, indexTwo, ',');
            getline(ss, indexThree, ',');
            getline(ss, indexFour, ',');

            if (indexTwo == query) {
               if(type == "productName"){
                    indexTwo = newValue;
               } else if(type == "productQuantity"){
                    indexThree = newValue;
               } else if(type == "productPrice"){
                    indexFour = newValue;
               } else if(type == "accountUsername"){
                    indexTwo = newValue;
               } else if(type == "accountPassword"){
                    indexThree = newValue;
               } else if(type == "accountRole"){
                    indexFour = newValue;
               }
                found = true;
            }
            fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "\n";
        }
        fileIn.close();

        if (!found) {
            cout << "Error: product '" << query << "' not found.\n";
        } else {
            ofstream fileOut(filename);
            if (!fileOut) {
                cout << "Cannot write to file " << filename << endl;
            }

            fileOut << fileContent;
            fileOut.close();

            cout << "Updated successfully.\n";
        }
        Sleep(1200);
    }
    
    void updateProductFields(const string &type, const string& productsDatabase, string field){
        system("cls");
        cout << "---------------------------------" << endl;
        cout << "Update " + type + " " + field << endl;
        cout << "---------------------------------" << endl;
        string originalInputName;

        cout << "Enter the " + type + " name (type 0 to go back): ";
        cin >> originalInputName;

        // just somehow if the user decides to change his mind
        if(originalInputName == "0") return;
        string newInputField;

        cout << "Enter the new " + type + " " + field + ": ";
        cin >> newInputField;

        field[0] = toupper(field[0]);

        updateInformation(productsDatabase, originalInputName, type + field, newInputField);
    }

};