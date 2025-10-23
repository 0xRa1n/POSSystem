#include "POSCashier.h" // Include the header for this class
#include "POSAdmin.h"   // Include the header for the class we need to use
#include "utilities.h"  // Include for utility functions

#include <cctype> // for toupper()
#include <sstream> // for stringstream
#include <windows.h> // for Sleep() and system("cls")
#include <bits/stdc++.h> // for this file, here are the functions used along with this package: fstream, vector, string, ctime (time.h), sstream, regex, iomanip (setw and left), limits, stdexcept (invalid_argument and out_of_range)

using namespace std;

// the cart
vector<vector<string>> cart; // 2D vector to hold cart items: { {productName, productCategory, quantity, price}, ... }
// vector of vector is used to hold multiple items, each item is a vector of strings
// so it will be easy to access each item and its details (by just looping over the cart vector, and using the index to access each item's details)

// we define all member functions of POSCashier here

class POSAdmin admin;

bool POSCashier::viewCart(string username){ // view cart is also bool since if the user proceeds to checkout, we need to know in the main menu to not break the loop
    // and if the user cancels the transaction and he went with the view cart, it will just return to the main menu
    system("cls");
    cout << "---------------------------------" << endl;
    cout << "P.O.S (Cashier)" << endl ;
    cout << "---------------------------------" << endl;

    cout << "Cart Summary" << endl;
    double totalAmount = 0.0;

    // check if the cart is empty
    if(cart.empty()){ // if the vector is empty
        cout << "\nYour cart is empty.\n";
        Sleep(1200);
        return false;
    } else {
        for(const auto& item : cart){ // Iterate over each item (which is a vector<string>) in the cart.
            cout << "\nProduct name: " << item[0] << "\n";
            cout << "Category: " << item[1] << "\n";
            cout << "Quantity: " << item[2] << "\n";
            cout << "Price: P" << item[3] << "\n";

            totalAmount += stoi(item[3]) * stoi(item[2]);
        }

        cout << "\nTotal Amount: P" << totalAmount << endl;
        // cout << "VAT (12%): P" << (totalAmount) * 0.12 << endl;
        // cout << "Amount Due: P" << (totalAmount) + ((totalAmount) * 0.12) << endl;

        cout << "\nDo you want to proceed to checkout? (1/0): ";
        int confirmation;
        cin >> confirmation;

        if(handleInputError()) return false; // handle invalid inputs

        switch(confirmation){
            case 1:
                return processTransaction(username);
            case 0:
                cout << "Returning to main menu.\n";
                Sleep(1200);
                return false; // go back to main menu without breaking the whole loop
            default:
                cout << "Invalid selection. Returning to main menu.\n";
                Sleep(1200);
                return false;
        }
    }
    return false; // just to satisfy the compiler, this line will never be reached
}

void POSCashier::saveTransaction(string productNames, string productQuantities, int initialAmount, int totalAmount, int change, string cashierName){
    string database = "database/transactions/cashierTransactions.csv";
    // get the current date and time
    time_t timestamp = time(NULL);
    struct tm datetime = *localtime(&timestamp);        

    char date[50];
    char time[50];

    strftime(date, 50, "%m_%d_%y", &datetime);
    strftime(time, 50, "%I:%M:%S_%p", &datetime);

        // strftime = format date and time as string

    // 50 means the maximum size of the char array
    // %m == month
    // %d == day
    // %y == year (last two digits)

    // %I == hour (12-hour clock)
    // %M == minute
    // %S == second
    // %p == AM or PM

    // &datetime == pointer to struct tm

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
            << initialAmount << ","
            << initialAmount * 0.12 << ","
            << totalAmount << ","
            << change << ","
            << date << ","
            << time << ","
            << cashierName << endl;

    fout.close(); // close the file
    // if not closed, it may lead to data loss or corruption if the program ends unexpectedly
}

// CRUD-Related Functions
bool POSCashier::processTransaction(string username) { // processTransaction is bool to indicate if the transaction was successful, if yes, it will go back to the main menu after the user had paid
    // also, if the user decided to cancel the transaction, it will return false to go back to the main menu without breaking the loop
    if(cart.empty()){
        cout << "Your cart is empty. Cannot process transaction.\n";
        Sleep(1200);
        return false;
    }

    int confirmation;
    // make a function that will first print the order summary
    system("cls");
    cout << "---------------------------------" << endl;
    cout << "P.O.S (Cashier)" << endl ;
    cout << "---------------------------------" << endl;

    cout << "Order Summary" << endl;
    double itemTotal = 0.0;
    int quantity = 0;
    double discountAmount = 0.0;
    int userMoney;
    
    // map is similar to vector and array, but it uses key-value pairs (just like JSON)
    map<string, double> categoryDiscounts; // expected output: { "Tops": 0.10, "Bottoms": 0.15, ... }
    ifstream discountFile("database/discounts.csv"); // open the discounts file
    if (!discountFile.is_open()) { 
        cerr << "Error opening discounts file." << endl;
        Sleep(1200);
        return false;
    }

    string line;
    getline(discountFile, line); // Skip header
    while (getline(discountFile, line)) {
        stringstream ss(line); // create a stringstream object to parse the line
        string category, percentageStr;

        getline(ss, category, ',');
        getline(ss, percentageStr, ',');
        if (!percentageStr.empty()) { // if it's not empty
            categoryDiscounts[category] = stod(percentageStr) / 100.0; // convert percentage to decimal, and store it in the map
        }
    }
    discountFile.close(); // close the file

    map<string, int> categoryCounts;
    for (const auto& item : cart) {
        categoryCounts[item[1]] += stoi(item[2]); // count the quantity of each category in the cart
        itemTotal += stoi(item[3]) * stoi(item[2]); // calculate the total amount before discount
    }

    // Step 3: For each category that qualifies, calculate the discount on its FIRST 3 items.
    for (auto const& [category, count] : categoryCounts) {
        // Check if the category has 3 or more items AND has a discount defined.
        if (count >= 3 && categoryDiscounts.count(category) > 0) {
            // if this was >= instead of ==, it will only count exactly 3 times, ignoring the discount if there are more than 3 items
            // below will calculate the subtotal for the first 3 items in this category
            
            double subtotalForDiscount = 0.0;
            int itemsCounted = 0;

            // Find the first 3 items belonging to this category in the cart.
            for (const auto& item : cart) {
                if (item[1] == category) { // If the item is in the right category
                    int price = stoi(item[3]);
                    int quantity = stoi(item[2]);

                    // Process each unit of this item (e.g., if quantity is 2, loop twice)
                    for (int i = 0; i < quantity; ++i) {
                        if (itemsCounted < 3) { // Only count up to 3 items, otherwise, stop counting
                            subtotalForDiscount += price;
                            itemsCounted++;
                        } else {
                            break; // Stop counting for this item if we already have 3
                        }
                    }
                }
            }
            // Add the calculated discount for this category to the total discount amount.
            discountAmount += subtotalForDiscount * categoryDiscounts[category]; // apply the discount rate
        }
    }

    // --- END OF DISCOUNT LOGIC ---

    for(const auto& item : cart){
        cout << "\nProduct name: " << item[0] << "\n";
        cout << "Product category: " << item[1] << "\n";
        cout << "Quantity: " << item[2] << "\n";
        cout << "Price: P" << stoi(item[3]) * stoi(item[2]) << "\n";
    }

    cout << "\nTotal Amount: P" << itemTotal << endl;
    cout << "Discounts Applied: P" << discountAmount << endl;
    cout << "Amount Due: P" << itemTotal - discountAmount << endl;

    cout << "\nProceed to purchase? (1/0): ";
    cin >> confirmation;
    
    // Correct VAT Calculation
    double discountedTotal = itemTotal - discountAmount;
    double vatAmount = discountedTotal * 0.12;
    double finalAmountDue = discountedTotal + vatAmount;

    if(handleInputError()) return false; // handle invalid inputs

    switch(confirmation){
        case 1:
            system("cls");
            cout << "---------------------------------" << endl;
            cout << "P.O.S (Cashier)" << endl ;
            cout << "---------------------------------" << endl;

            cout << "Order Receipt" << endl;
            cout << "\n";
            for(const auto& item : cart){
                cout << "Product name: " << item[0] << "\n";
                cout << "Product category: " << item[1] << "\n";
                cout << "Quantity: " << item[2] << "\n";
                cout << "Price: P" << item[3] << "\n\n";
            }

            cout << "Total Amount: P" << itemTotal << endl;
            cout << "Discounts Applied: P" << discountAmount << endl;

            cout << "VAT (12%): P" << vatAmount << endl;
            cout << "Amount Due: P" << finalAmountDue << endl;
            break;
        case 0:
            int choice;
            cout << "\nDo you want to cancel the transaction? (1/0): ";
            cin >> choice;

            if(handleInputError()) return false; // handle invalid inputs

            switch(choice){
                case 1:
                    // clear the cart after cancelling the transaction
                    cart.clear();

                    cout << "Transaction cancelled.\n";
                    Sleep(1200);
                    return true; // since the user cancelled the transaction, we return true to go back to the main menu
                    // technically this is incorrect since in the POS.cpp, we break if the boolean is true, but this is more intuitive for the user
                case 0:
                    cout << "Returning to main menu.\n";
                    Sleep(1200);
                    return true;
                default:
                    cout << "Invalid selection.\n"; // fallback, but should not reach here
                    Sleep(1200);
                    return false;
            }
            break;
        default:
            cout << "Invalid selection.\n"; // fallback, but should not reach here
            Sleep(1200);
            return false;
    }

    cout << "---------------------------------" << endl;
    cout << "Enter the customer's money: ";
    cin >> userMoney;

    if(handleInputError()) return false; // handle invalid inputs

    int change = userMoney - ((itemTotal) + ((itemTotal) * 0.12));

    // check if the user has sufficient money
    if(userMoney < (itemTotal) + ((itemTotal) * 0.12)){
        int confirmation;
        cout << "Insufficient money. Try again? (1/0): ";
        cin >> confirmation;

        if(handleInputError()) return false; // handle invalid inputs

        switch(confirmation){
            case 1:
                return processTransaction(username);
            case 0:
                // clear the cart after cancelling the transaction
                cart.clear();

                cout << "Transaction cancelled.\n";
                Sleep(1200);
                return true;
            default:
                cout << "Invalid selection.\n"; // fallback, but should not reach here
                Sleep(1200);
                return true;
        }
    } else {
        // we need these for two purposes: backup and main transaction logging
        // pass the vector of product names and quantities to a single string, separated by semicolons
        stringstream namesStream, quantitiesStream;
        for(int i = 0; i < cart.size(); i++){ // iterate through the cart
            namesStream << cart[i][0]; // append the product name to the stream
            quantitiesStream << cart[i][2]; // append the product quantity to the stream
            if(i < cart.size() - 1){ // append semicolon everywhere except the last item
                namesStream << ";";
                quantitiesStream << ";";
            }
        }

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
            return false;
        }
        
        // to do: add quantity
        backupFile << "ProdNames,ProdQty,Amt,DcAmt,Tax,TotalAmt,UserMoney,Change,Date,Time,Cashier\n" << namesStream.str() << ","
                    << quantitiesStream.str() << ","
                    << itemTotal << ","
                    << discountAmount << ","
                    << vatAmount << ","
                    << finalAmountDue << ","
                    << userMoney << ","
                    << change << ","
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
            getline(ss, token, ','); // skip the category
            getline(ss, productQuantity, ','); // get the quantity

            // since there are multiple products, we need to loop through the vector of product names
            for(int i = 0; i < cart.size(); i++){ // iterate through the cart
                if(productName == cart[i][0]){
                    int updatedQuantity = stoi(productQuantity) - stoi(cart[i][2]); // subtract the quantity purchased | stoi means string to integer
                    // update the quantity in the database
                    admin.updateInformation(productsDatabase, productName, "productQuantity", to_string(updatedQuantity), username, "Purchased_Quantity_Deducted"); // use the previous function to update the quantity | we used to_string since we are not only using the function for products
                }
            }
        }
        
        // save the transaction to the main database
        saveTransaction(namesStream.str(), quantitiesStream.str(), itemTotal, finalAmountDue, change, username);

        // remove the backup file after saving the transaction to the main database
        // remove("database/transactions/backup.csv");

        // clear the cart after the transaction
        cart.clear();

        cout << "Change: " << change << endl;
    }
    system("pause");
    return true;
}

bool POSCashier::readProductsBySubcategory(string productsDatabase, string subCategory, string username){
    int quantityToPurchase;
    ifstream file(productsDatabase);
            
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        Sleep(1200);
        return false;
    }
    cout << "Format: ID, ProductName, Category, SubCategory, Quantity, Price\n" << endl;

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
        if (row.size() > 3 && row[3] == subCategory) { // if row size is 2 to avoid out of range error since we are trying to get the 3rd index which is subcategory
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
        return false;
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

    string productName, productCategory;
    int productQuantity, productPrice;

    // Get user input for product ID
    int selectedId;
    cout << "\nEnter product ID to select (0 to go back): ";
    cin >> selectedId;

    if (handleInputError()) return false; // handle invalid inputs
    if (selectedId == 0) return false;

    // Find and display selected product
    bool found = false;

    for (auto row : subCategoryRows) {
        if (stoi(row[0]) == selectedId) {
            productName = row[1];
            productCategory = row[2];
            productQuantity = stoi(row[4]);
            productPrice = stoi(row[5]);

            if (productQuantity == 0){
                cout << "Product is out of stock!\n";
                system("pause");
                return false;
            } else {
                found = true;
                cout << "\nSelected Product:\n";
                cout << "Product Name: " << productName << "\n";
                cout << "Product category: " << productCategory << "\n";
                cout << "Available Quantity: " << productQuantity << "\n";
                cout << "Price: P" << productPrice << "\n";
                break;
            }   
        }
    }
    if(!found){
        cout << "Invalid product ID\n";
        Sleep(1200);
        return false;
    }

    cout << "\nEnter quantity to purchase (0 to go back): ";
    cin >> quantityToPurchase;

    if(handleInputError()) return false; // handle invalid inputs
    if (quantityToPurchase == 0) return false;

    // if the quantity is greater than the available quantity, show an error message
    if (quantityToPurchase > productQuantity) {
        cout << "Insufficient stock available!\n";
        Sleep(1200);
        return false;
    } else if(quantityToPurchase < 0){
        cout << "Quantity cannot be negative. Please enter a valid integer." << endl;
        Sleep(1200);
        return false;
    }

    // ask if the user wants to add more products to the cart
    cout << "Product added successfully. Do you want to add more? (1/0): ";
    int addMore;
    cin >> addMore;

    if(handleInputError()) return false; // handle invalid inputs

    // save or append the previous product
    cart.push_back({productName, productCategory, to_string(quantityToPurchase), to_string(productPrice)});

    switch(addMore){
        case 1:
            return false; // return to the previous menu to add more products
        case 0:
            // do nothing just proceed to the next step
            break;
        default:
            system("cls");
            cout << "Invalid selection.\n"; // fallback, but cin.fail() should catch this
            Sleep(1200);
            return false;
    }

    return processTransaction(username);
};