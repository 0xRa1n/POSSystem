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
        ifstream fin(filename); // create an input file stream
        string line;

        while (getline(fin, line)) {
            if (line.find(productNameToCheck) != string::npos) { // string::npos would return -1 if not found
                return true;  // Product name found somewhere in the database
            }
        }
        return false;
    }

    void saveLogs(string type, string operation, string affectedEntry, string adminName) {
        // use different log files for different types
        string database;
        if(type == "accounts"){
            database = "database/logs/adminUserLogs.csv";
        } else if(type == "products"){
            database = "database/logs/productsLogs.csv";
        } else {
            cout << "Invalid log type specified." << endl;
            return;
        }

        // get the current date and time
        time_t timestamp = time(NULL);
        struct tm datetime = *localtime(&timestamp);        

        char date[50];
        char time[50];

        strftime(date, 50, "%m_%d_%y", &datetime);
        strftime(time, 50, "%I:%M:%S_%p", &datetime);

        // open the transactions.csv file in append mode
        ofstream fout(database, ios::app);
        if (!fout) {
            cerr << "Error opening database for writing." << endl;
            return;
        }

        // write the transaction details to the file
        fout << operation << ","
            << affectedEntry << ","
            << date << ","
            << time << ","
            << adminName << endl;
  
        fout.close();
    }

    // CRUD-Related Functions
    void addProduct(string database, string username) {
        string productName, productSubCategory;
        int quantity, price;

        // ask the user for the product name (if 0 is entered, it will go back to the menu)
        cout << "Enter the product name (type 0 to return): ";
        cin >> productName;

        // if user changes his mind
        if (productName == "0") return;

        // use the function to check if the entry is already in the database
        if (isAlreadyInCsv(database, productName)) {
            cout << "Product '" << productName << "' is already in the CSV.\n";
        } 

        // ask the user if what product sub-category is the product

        cout << "Enter the product sub-category: ";
        cin >> productSubCategory;

        // ask the user for the product's quantity and price
        cout << "Enter the quantity: ";
        cin >> quantity;
        cout << "Enter the price: ";
        cin >> price;

        // get the last product id from the database
        int newId = getLastId(database) +  1;

        // open the database file, and add the new product
        fstream fout;
        fout.open(database, ios::out | ios::app); // append mode

        fout << newId << ","
            << productName << ","
            << productSubCategory << ","
            << quantity << ","
            << price << "\n";

        fout.close();
        cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
        saveLogs("products", "ADD", productName, username);

        // wait for 1.2 seconds to go back to the main menu
        Sleep(1200);
    }

    void addUser(string database, string accessingUsername){
        string username, password, role;
                                
        // ask the user for the username (if 0 is entered, it will go back to the menu)
        cout << "Enter the username you want to add (type 0 to return): ";
        cin >> username;

        // use the function to check if the entry is already in the database
        if(isAlreadyInCsv(database, username)){
            cout << "User is already in the database!";
        }

        // just somehow if the user decides to change his mind
        if (username == "0") return;

        // ask for the password and role
        cout << "Enter the password: ";
        cin >> password;
        cout << "What is the role of the user? (Admin/Manager/Cashier): ";
        cin >> role;

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
        saveLogs("accounts", "ADD", username, accessingUsername);
    
        // wait for 1.2 seconds before going back to the menu
        Sleep(1200);
    }

    void readProducts(string database) {
        // Open the file
        ifstream file(database);

        // check if file exists
        if (!file.is_open()) {
            cout << "Failed to open file\n";
            return;
        }

        // Read all rows first
        vector<vector<string>> rows; // 2D vector to hold rows and columns
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            vector<string> row; 
            while (getline(ss, cell, ',')) { // split by comma
                row.push_back(cell); // add each cell to the row
            }
            rows.push_back(row); // add the row to the list of rows
        }
        file.close(); // close the file

        // stop if the rows are empty
        if (rows.empty()) return;

        // Find max width of each column
        size_t cols = 0;
        for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number in  the vector rows, so that the other parts will not overlap
        vector<size_t> widths(cols, 0); // initialize widths with 0

        // after we get the maximum number, we will update the widths vector
        // this will make sure that each value will not overlap with each other

        for (auto &r : rows) { 
            for (size_t c = 0; c < r.size(); ++c) // for each column in the row
                widths[c] = max(widths[c], r[c].size()); // update max width
                // there is no curly braces here because it is a single controlled statement
        }

        // Add a little padding for readability
        for (auto &w : widths) w += 2;

        // Print
        for (auto &r : rows) {
            for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
                cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            }
            cout << '\n';
        }
    }

    void deleteInformation(string type, string filename, string username){
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
                rows.push_back(line); // keep the line if it doesn't match
            } else {
                found = true;
            }
        }
        input_file.close();

        if (!found) {
            cout << "Query not found";
        } else {
            // Write back filtered rows
            ofstream output_file(filename, ios::trunc); // open in truncate mode
            // it avoids the line that has the query, and it rewrites the csv without it
            for (const auto& row : rows) { // write each remaining row
                output_file << row << "\n"; // add newline
            } 
            output_file.close();

            if(type == "accounts"){ // save to the account logs
                saveLogs("accounts", "DELETE", deleteProductInput, username);
            } else if(type == "products"){ // save to the product logs
                saveLogs("products", "DELETE", deleteProductInput, username);
            }

            cout << "Successfully deleted " << deleteProductInput << endl;
        }
        Sleep(1200);
    }
    
    int updateInformation(string filename, string query, string type, string newValue, string username) {
        ifstream fileIn(filename); // open an input file stream, since we are reading the file first, then, writing it back
        if (!fileIn) {
            return 0;
        }

        string fileContent, line;
        bool found = false;

        while (getline(fileIn, line)) {
            stringstream ss(line); // create a string stream from the line
            string indexOne, indexTwo, indexThree, indexFour, indexFive;
            // get a copy of each entries
            getline(ss, indexOne, ',');
            getline(ss, indexTwo, ',');
            getline(ss, indexThree, ',');
            getline(ss, indexFour, ',');
            if(type.find("product") != string::npos){ // if the type contains the word "product"
                getline(ss, indexFive, ',');
            }

            // modify the entry if it matches the query
            if (indexTwo == query) {
                // update the corresponding field
                // product
               if(type == "productName"){
                    indexTwo = newValue;
               } else if(type == "productQuantity"){
                    indexFour = newValue;
               } else if(type == "productPrice"){
                    indexFive = newValue;
               }  else if(type == "productSubCategory"){
                    indexThree = newValue;
               } 
               // account
               else if(type == "accountUsername"){
                    indexTwo = newValue;
               } else if(type == "accountPassword"){
                    indexThree = newValue;
               } else if(type == "accountRole"){
                    indexFour = newValue;
               }
                found = true;
            }
            // write back to the file content
            if(type.find("product") != string::npos){
                fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "," + indexFive + "\n";
            } else {
                fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "\n";
            }
        }
        fileIn.close();

        if (!found) {
            return 0;
        } else {
            ofstream fileOut(filename);
            if (!fileOut) {
                cout << "Cannot write to file " << filename << endl;
            }

            fileOut << fileContent;
            fileOut.close();

            // we will log EVERY action made by the admin
            if(type.find("account") != string::npos){ // check if the type contains the word "account"
                if(type == "accountPassword"){
                    saveLogs("accounts", "UPDATE", query + "_PW_to_" + newValue, username);
                } else {
                    saveLogs("accounts", "UPDATE", query + "_to_" + newValue, username);
                }
            } else if(type.find("product") != string::npos){
                if(type == "productName"){
                    saveLogs("products", "UPDATE", query + "_to_" + newValue, username);
                } else {
                    if(type == "productPrice"){
                        saveLogs("products", "UPDATE", query + "_Price_to_" + newValue, username);
                    } else if(type == "productQuantity"){
                        saveLogs("products", "UPDATE", query + "_Quantity_to_" + newValue, username);
                    } else if(type == "productSubCategory"){
                        saveLogs("products", "UPDATE", query + "_SubCategory_to_" + newValue, username);
                    }
                }
            }

            return 1;
        }
    }
    void updateProductFields(string type, string database, string field, string username){
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

        // validate first if the query exists in the database
        ifstream file(database);
        string line;
        bool found = false;
        while (getline(file, line)) { // reads the file and saves the output in the variable "line"
            stringstream ss(line); // create a string stream from the line
            // stringstream helps us read a string as if it were a stream (like cin)
            // stream are the foundation of i/o operations in C++
            string token;
            getline(ss, token, ',');     // first read the id (not used) | the third parameter is the delimiter
            getline(ss, token, ',');     // second read the username or product name | the value of the second getline is saved in the variable "token"
            if(token == originalInputName){
                found = true;
                break;
            }
        }
        file.close();

        if (!found) {
            cout << "The product " + type + " was not found.\n";
            Sleep(1200);
            return;
        }

        cout << "Enter the new " + type + " " + field + ": ";
        cin >> newInputField;

        field[0] = toupper(field[0]); // capitalize the first letter of the field

        if(updateInformation(database, originalInputName, type + field, newInputField, username) == 1){ // since I am only using this function one time inside this function
            cout << "Updated successfully.\n";
        } else {
            cout << "The product " + type + " was not found.\n";
        }
        Sleep(1200);
    }

    void getTotalSales(string database){
        string line;
        int totalSales = 0;
        // open the file
        ifstream file(database);

        // check if file exists
        if(!file.is_open()){
            cout << "Failed to open file\n";
            return;
        }

        // loop through each line and get the amount
        getline(file, line); // skip the header of the csv
        while(getline(file, line)){
            stringstream ss(line); // create a string stream, making us read the line as if it were a stream
            string token;
            // skip the first three tokens
            getline(ss, token, ','); // skip id
            getline(ss, token, ','); // skip product names
            getline(ss, token, ','); // skip product quantities
            getline(ss, token, ','); // get the amount
            totalSales += stoi(token); // convert to integer and add to total sales
        }
        file.close();

        cout << "Total Sales: P" << totalSales << endl;
    }
};