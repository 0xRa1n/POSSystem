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
    POSAdmin admin;
    
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

    void saveTransaction(string productNames, string productQuantities, int totalAmount, string cashierName){
        string database = "database/transactions/cashierTransactions.csv";
        // get the current date and time
        time_t timestamp = time(NULL);
        struct tm datetime = *localtime(&timestamp);        

        char date[50];
        char time[50];

        strftime(date, 50, "%m_%d_%y", &datetime);
        strftime(time, 50, "%I:%M:%S_%p", &datetime);

        // generate an id based on the previous id 
        int newId = getLastId(database) + 1;

        // open the transactions.csv file in append mode
        ofstream fout(database, ios::app);
        if (!fout) {
            cerr << "Error opening database for writing." << endl;
            return;
        }

        // write the transaction details to the file
        fout << newId << ","
             << productNames << ","
             << productQuantities << ","
             << totalAmount << ","
             << date << ","
             << time << ","
             << cashierName << endl;

        fout.close();
    }

    // CRUD-Related Functions
    void processTransaction(vector<string>& productNames, vector<int>& productQuantities, vector<int>& productPrices, string username) {
        char confirmation;
        // make a function that will first print the order summary
        system("cls");
        cout << "---------------------------------" << endl;
        cout << "P.O.S (Cashier)" << endl ;
        cout << "---------------------------------" << endl;

        cout << "Order Summary" << endl;
        double totalAmount = 0.0;
        int userMoney;
        for(size_t i = 0; i < productNames.size(); i++){ // use size_t to avoid signed/unsigned comparison warnings
            cout << "\n" <<productNames[i] << "\n";
            cout << "Quantity: " << productQuantities[i] << "\n";
            cout << "Price: P" << productPrices[i] * productQuantities[i] << "\n";

            totalAmount += productPrices[i] * productQuantities[i];
        }

        cout << "\nProceed to purchase? (Y/N): ";
        cin >> confirmation;

        if(toupper(confirmation) == 'Y' || tolower(confirmation) == 'y'){
            system("cls");
            cout << "---------------------------------" << endl;
            cout << "P.O.S (Cashier)" << endl ;
            cout << "---------------------------------" << endl;

            cout << "Order Receipt" << endl;
            cout << "\n";
            for(size_t i = 0; i < productNames.size(); i++){
                cout << productNames[i] << "\n";
                cout << "Quantity: " << productQuantities[i] << "\n";
                cout << "Price: P" << productPrices[i] * productQuantities[i] << "\n\n";
            }

            cout << "Total Amount: P" << totalAmount << endl;
            cout << "VAT (12%): P" << (totalAmount) * 0.12 << endl;
            cout << "Amount Due: P" << (totalAmount) + ((totalAmount) * 0.12) << endl;
        } else {
            // clear the cart after cancelling the transaction
            productNames.clear();
            productQuantities.clear();
            productPrices.clear();

            cout << "Purchase cancelled." << endl;
            Sleep(1200);
            return;
        }

        cout << "---------------------------------" << endl;
        cout << "Enter the user's money: ";
        cin >> userMoney;

        if(userMoney < (totalAmount) + ((totalAmount) * 0.12)){
            string confirmation;
            cout << "Insufficient money. Try again? (Y/N).\n";
            cin >> confirmation;
            if(toupper(confirmation[0]) == 'Y' || tolower(confirmation[0]) == 'y'){
                processTransaction(productNames, productQuantities, productPrices, username);
            } else {
                // clear the cart after cancelling the transaction
                productNames.clear();
                productQuantities.clear();
                productPrices.clear();

                cout << "Transaction cancelled.\n";
                Sleep(1200);
                return;
            }
        } else {
            cout << "Change: " << userMoney - ((totalAmount) + ((totalAmount) * 0.12)) << endl;
        }
        system("pause");


        // first, find the product in the database, then, update the quantity using the function updateInformation
        const string productsDatabase = "database/products.csv";
        // get the value of the quantity from the database, then, subtract it with the quantity purchased
        ifstream fileIn(productsDatabase);
        
        string fileContent, line;
        while(getline(fileIn, line)){
            stringstream ss(line);
            string productName, productQuantity, token;
            getline(ss, token, ',');
            getline(ss, productName, ','); // get the product name
            getline(ss, token, ','); // skip the sub-category
            getline(ss, productQuantity, ','); // get the quantity

            // since there are multiple products, we need to loop through the vector of product names
            for(size_t i = 0; i < productNames.size(); i++){ // iterate through the cart
                if(productName == productNames[i]){
                    int updatedQuantity = stoi(productQuantity) - productQuantities[i]; // subtract the quantity purchased | stoi means string to integer
                    // update the quantity in the database
                    admin.updateInformation(productsDatabase, productName, "productQuantity", to_string(updatedQuantity), username); // use the previous function to update the quantity | we used to_string since we are not only using the function for products
                }
            }

        }
        
        // pass the vector of product names and quantities to a single string, separated by semicolons
        stringstream namesStream, quantitiesStream;
        for(size_t i = 0; i < productNames.size(); i++){ // iterate through the cart
            namesStream << productNames[i]; // append the product name to the stream
            quantitiesStream << productQuantities[i]; // append the product quantity to the stream
            if(i < productNames.size() - 1){ // append semicolon everywhere except the last item
                namesStream << ";";
                quantitiesStream << ";";
            }
        }
        saveTransaction(namesStream.str(), quantitiesStream.str(), totalAmount + (totalAmount * 0.12), username);
        // clear the cart after the transaction
        cartProducts.clear();
        cartQuantities.clear();
        cartPrices.clear();
    }

    vector<string> cartProducts;
    vector<int> cartQuantities;
    vector<int> cartPrices;
    void readProductsBySubcategory(string productsDatabase, string subCategory, string username){
        int quantityToPurchase;
        ifstream file(productsDatabase);
              
        if (!file.is_open()) {
            cout << "Failed to open file\n";
            Sleep(1200);
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
            Sleep(1200);
            return;
        }

        // Display T-Shirts with proper formatting
        cout << "ID    Product Name           Quantity  Price\n";
        cout << "----  --------------------   --------  -----\n";
        
        for (auto &row : tshirtRows) {
            cout << left << setw(6) << row[0]        // ID
                << setw(23) << row[1]               // Product Name
                << setw(10) << row[3]               // Quantity
                << "P" << row[4] << "\n";                  // Price
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

                if (productQuantity == 0){
                    cout << "Product is out of stock!\n";
                    system("pause");
                    return;
                } else {
                    cout << "\nSelected Product:\n";
                    cout << "Product Name: " << productName << "\n";
                    cout << "Price: P" << productPrice << "\n";
                    found = true;
                    break;
                }   
            }
        }

        if (!found) {
            cout << "Invalid product ID!\n";
        }

        cout << "\nEnter quantity to purchase: ";
        cin >> quantityToPurchase;

        // ask if the user wants to add more products to the cart
        cout << "Do you want to add more? (Y/N): ";
        char addMore;
        cin >> addMore;

        // save or append the previous product
        cartProducts.push_back(productName);
        cartQuantities.push_back(quantityToPurchase);
        cartPrices.push_back(productPrice);

        if(toupper(addMore) == 'Y' || tolower(addMore) == 'y'){
            return;
        }        

        processTransaction(cartProducts, cartQuantities, cartPrices, username);
    }
};