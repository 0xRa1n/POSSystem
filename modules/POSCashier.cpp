#include "POSCashier.h" // Include the header for this class
#include "POSAdmin.h"   // Include the header for the class we need to use
#include "utilities.h"  // Include for utility functions

#include <cctype>
#include <sstream>
#include <windows.h>
#include <bits/stdc++.h>

using namespace std;
// the cart
vector<string> cartProducts;
vector<int> cartQuantities;
vector<int> cartPrices;

class POSAdmin admin;

void POSCashier::viewCart(string username){
    system("cls");
    cout << "---------------------------------" << endl;
    cout << "P.O.S (Cashier)" << endl ;
    cout << "---------------------------------" << endl;

    cout << "Cart Summary" << endl;
    double totalAmount = 0.0;

    // check if the cart is empty
    if(cartProducts.empty()){
        cout << "\nYour cart is empty.\n";
        Sleep(1200);
        return;
    } else {
        for(int i = 0; i < cartProducts.size(); i++){
            cout << "\nProduct name: " << cartProducts[i] << "\n";
            cout << "Quantity: " << cartQuantities[i] << "\n";
            cout << "Price: P" << cartPrices[i] * cartQuantities[i] << "\n";

            totalAmount += cartPrices[i] * cartQuantities[i];
        }

        cout << "\nTotal Amount: P" << totalAmount << endl;
        // cout << "VAT (12%): P" << (totalAmount) * 0.12 << endl;
        // cout << "Amount Due: P" << (totalAmount) + ((totalAmount) * 0.12) << endl;

        cout << "\nDo you want to proceed to checkout? (1/0): ";
        int confirmation;
        cin >> confirmation;

        if(handleInputError()) return; // handle invalid inputs

        switch(confirmation){
            case 1:
                processTransaction(cartProducts, cartQuantities, cartPrices, username);
                break;
            case 0:
                cout << "Returning to main menu.\n";
                Sleep(1200);
                return;
            default:
                cout << "Invalid selection. Returning to main menu.\n";
                Sleep(1200);
                return;
        }
    }
}

void POSCashier::saveTransaction(string productNames, string productQuantities, int totalAmount, string cashierName){
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
void POSCashier::processTransaction(vector<string>& productNames, vector<int>& productQuantities, vector<int>& productPrices, string username) {
    if(productNames.empty()){
        cout << "Your cart is empty. Cannot process transaction.\n";
        Sleep(1200);
        return;
    }

    int confirmation;
    // make a function that will first print the order summary
    system("cls");
    cout << "---------------------------------" << endl;
    cout << "P.O.S (Cashier)" << endl ;
    cout << "---------------------------------" << endl;

    // to do: if the product quantities are more than 10, add a 10% discount
    cout << "Order Summary" << endl;
    double totalAmount = 0.0;
    int userMoney;
    for(int i = 0; i < productNames.size(); i++){ // use size_t to avoid signed/unsigned comparison warnings
        cout << "\nProduct name: " <<productNames[i] << "\n";
        cout << "Quantity: " << productQuantities[i] << "\n";
        cout << "Price: P" << productPrices[i] * productQuantities[i] << "\n";

        totalAmount += productPrices[i] * productQuantities[i];
    }
    cout << "\nTotal Amount: P" << totalAmount << endl;

    cout << "\nProceed to purchase? (1/0): ";
    cin >> confirmation;

    if(handleInputError()) return; // handle invalid inputs

    switch(confirmation){
        case 1:
            system("cls");
            cout << "---------------------------------" << endl;
            cout << "P.O.S (Cashier)" << endl ;
            cout << "---------------------------------" << endl;

            cout << "Order Receipt" << endl;
            cout << "\n";
            for(int i = 0; i < productNames.size(); i++){
                cout << "Product name: " << productNames[i] << "\n";
                cout << "Quantity: " << productQuantities[i] << "\n";
                cout << "Price: P" << productPrices[i] * productQuantities[i] << "\n\n";
            }

            cout << "Total Amount: P" << totalAmount << endl;
            cout << "VAT (12%): P" << (totalAmount) * 0.12 << endl;
            cout << "Amount Due: P" << (totalAmount) + ((totalAmount) * 0.12) << endl;
            break;
        case 0:
            int choice;
            cout << "\nDo you want to cancel the transaction? (1/0): ";
            cin >> choice;

            if(handleInputError()) return; // handle invalid inputs

            switch(choice){
                case 1:
                    // clear the cart after cancelling the transaction
                    productNames.clear();
                    productQuantities.clear();
                    productPrices.clear();

                    cout << "Transaction cancelled.\n";
                    Sleep(1200);
                    return;
                case 0:
                    cout << "Returning to main menu.\n";
                    Sleep(1200);
                    return;
                default:
                    cout << "Invalid selection.\n"; // fallback, but should not reach here
                    Sleep(1200);
                    return;
            }
            break;
        default:
            cout << "Invalid selection.\n"; // fallback, but should not reach here
            Sleep(1200);
            return;
    }

    cout << "---------------------------------" << endl;
    cout << "Enter the customer's money: ";
    cin >> userMoney;

    if(handleInputError()) return; // handle invalid inputs

    // check if the user has sufficient money
    if(userMoney < (totalAmount) + ((totalAmount) * 0.12)){
        int confirmation;
        cout << "Insufficient money. Try again? (1/0): ";
        cin >> confirmation;

        if(handleInputError()) return; // handle invalid inputs

        switch(confirmation){
            case 1:
                processTransaction(productNames, productQuantities, productPrices, username);
                break;
            case 0:
                // clear the cart after cancelling the transaction
                productNames.clear();
                productQuantities.clear();
                productPrices.clear();

                cout << "Transaction cancelled.\n";
                Sleep(1200);
                return;
            default:
                cout << "Invalid selection.\n"; // fallback, but should not reach here
                Sleep(1200);
                return;
        }
    } else {
        // power outtage might happen here after the user had paid, so we need to have a backup that will log the transaction even if the program crashes
        // in this manner, if the power outtage happens right after the user had paid, we can still recover the transaction from the backup file

        // get the current date and time
        time_t timestamp = time(NULL);
        struct tm datetime = *localtime(&timestamp);        

        char date[50];
        char time[50];

        strftime(date, 50, "%m_%d_%y", &datetime);
        strftime(time, 50, "%I:%M:%S_%p", &datetime);

        ofstream backupFile("database/transactions/backup.csv", ios::app); // open in append mode
        if (!backupFile) {
            cerr << "Error opening backup file for writing." << endl;
            return;
        }

        // convert productNames vector to a semicolon-separated string
        stringstream productNamesToString;
        for(int i = 0; i < productNames.size(); i++){
            productNamesToString << productNames[i]; // append the product name to the variable productNamesToString
            if(i < productNames.size() - 1){ // only add semicolon if it's not the last item
                productNamesToString << ";"; // append semicolon everywhere except the last item || append a semicolon to the next item
            }
        }

        backupFile << productNamesToString.str() << ","
                    << totalAmount + (totalAmount * 0.12) << ","
                    << userMoney << ","
                    << date << ","
                    << time << ","
                    << username << "\n";

        backupFile.close();

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
            for(int i = 0; i < productNames.size(); i++){ // iterate through the cart
                if(productName == productNames[i]){
                    int updatedQuantity = stoi(productQuantity) - productQuantities[i]; // subtract the quantity purchased | stoi means string to integer
                    // update the quantity in the database
                    admin.updateInformation(productsDatabase, productName, "productQuantity", to_string(updatedQuantity), username); // use the previous function to update the quantity | we used to_string since we are not only using the function for products
                }
            }
        }
        
        // pass the vector of product names and quantities to a single string, separated by semicolons
        stringstream namesStream, quantitiesStream;
        for(int i = 0; i < productNames.size(); i++){ // iterate through the cart
            namesStream << productNames[i]; // append the product name to the stream
            quantitiesStream << productQuantities[i]; // append the product quantity to the stream
            if(i < productNames.size() - 1){ // append semicolon everywhere except the last item
                namesStream << ";";
                quantitiesStream << ";";
            }
        }
        // save the transaction to the main database
        saveTransaction(namesStream.str(), quantitiesStream.str(), totalAmount + (totalAmount * 0.12), username);

        // remove the backup file after saving the transaction to the main database
        remove("database/transactions/backup.csv");

        // clear the cart after the transaction
        cartProducts.clear();
        cartQuantities.clear();
        cartPrices.clear();

        cout << "Change: " << userMoney - ((totalAmount) + ((totalAmount) * 0.12)) << endl;
    }
    system("pause");
}

void POSCashier::readProductsBySubcategory(string productsDatabase, string subCategory, string username){
    int quantityToPurchase;
    ifstream file(productsDatabase);
            
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }
    cout << "Format: ID, ProductName, SubCategory, Quantity, Price\n" << endl;

    // Read and filter T-Shirts products
    vector<vector<string>> subCategoryRows;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell); // add each cell to the row
            // row will look like this: {"ID", "ProductName", "SubCategory", "Quantity", "Price"
        }
        // Only add if ProductSubCategory is the desired sub-category (subCategory)
        if (row.size() > 2 && row[2] == subCategory) { // if row size is 2 to avoid out of range error since we are trying to get the 3rd index which is subcategory
            subCategoryRows.push_back(row); // add the row to the subCategoryRows
            // subCategoryRows will look like this: {{"ID", "ProductName", "SubCategory", "Quantity", "Price"}, {"1", "Product1", "T-Shirts", "10", "100"}, ...
            // the contents of the variable row will only be added if the sub-category matches
            // this way, we only have the products that belong to the desired sub-category
        }
    }
    file.close();

    if (subCategoryRows.empty()) {
        cout << "No products available in this sub-category.\n";
        Sleep(1200);
        return;
    }

    // print all the products in the said subcategory
    // Find max width of each column
    size_t cols = 0;
    for (auto &r : subCategoryRows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement
    vector<size_t> widths(cols, 0); // initialize a vector with widths of 0
    // initially: widths is {0, 0, 0, 0, 0} for 5 columns
    // then the following code will update the widths for each row

    // after we get the maximum number, we will update the widths vector
    for (auto &r : subCategoryRows) { 
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement
    }

    // Add a little padding for readability
    for (auto &w : widths) w += 2;

    // Print
    for (auto &r : subCategoryRows) {
        for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }

    // Get user input for product ID
    cout << "\nEnter product ID to select (0 to go back): ";
    int selectedId;
    cin >> selectedId;

    if(handleInputError()) return; // handle invalid inputs
    if (selectedId == 0) return;

    bool found = false;

    // Find and display selected product
    string productName;
    int productQuantity, productPrice;

    for (auto row : subCategoryRows) {
        if (stoi(row[0]) == selectedId) {
            productName = row[1];
            productQuantity = stoi(row[3]);
            productPrice = stoi(row[4]);

            if (productQuantity == 0){
                cout << "Product is out of stock!\n";
                system("pause");
                return;
            } else {
                found = true;
                cout << "\nSelected Product:\n";
                cout << "Product Name: " << productName << "\n";
                cout << "Price: P" << productPrice << "\n";
                break;
            }   
        }
    }
    if(!found){
        cout << "Invalid product ID\n";
        Sleep(1200);
        return;
    }

    cout << "\nEnter quantity to purchase (0 to go back): ";
    cin >> quantityToPurchase;

    if(handleInputError()) return; // handle invalid inputs
    if (quantityToPurchase == 0) return;

    // if the quantity is greater than the available quantity, show an error message
    if (quantityToPurchase > productQuantity) {
        cout << "Insufficient stock available!\n";
        system("pause");
        return;
    }

    // ask if the user wants to add more products to the cart
    cout << "Product added succcessfully. Do you want to add more? (1/0): ";
    int addMore;
    cin >> addMore;

    if(handleInputError()) return; // handle invalid inputs

    // save or append the previous product
    cartProducts.push_back(productName);
    cartQuantities.push_back(quantityToPurchase);
    cartPrices.push_back(productPrice);

    switch(addMore){
        case 1:
            return;
        case 0:
            // do nothing just proceed to the next step
            break;
        default:
            cout << "Invalid selection.\n"; // fallback, but cin.fail() should catch this
            Sleep(1200);
            return;
    }

    processTransaction(cartProducts, cartQuantities, cartPrices, username);
};