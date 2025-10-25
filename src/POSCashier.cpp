#include "./include/POSCashier.h" // Include the header for this class
#include "./include/POSAdmin.h"   // Include the header for the class we need to use
#include "./include/utilities.h"  // Include for utility functions

// include all the packages that we only need IN THIS FILE
#include <iostream>
#include <cctype> // for toupper()
#include <sstream> // for stringstream
#include <algorithm> // used for find
#include <regex> // for regex
#include <fstream> // for fstream, ifstream
#include <iomanip> // for setw and left

// the cart
vector<vector<string>> cart; // 2D vector to hold cart items: { {productName, productCategory, quantity, price}, ... }
// vector of vector is used to hold multiple items, each item is a vector of strings
// so it will be easy to access each item and its details (by just looping over the cart vector, and using the index to access each item's details)

// we define all member functions of POSCashier here
POSAdmin admin; // we need to use the function saveLogs 

// here are all member functions of POSCashier class
// the double colon (::) is the scope resolution operator, which tells us that the function belongs to the class POSCashier

void POSCashier::deductPurchasedQuantities(string productsDatabase, string query, string username, string quantities){
    string line, fileContent;
    bool isFound;

    ifstream readFile(productsDatabase); // open the file as an ifstream since we are reading from it
    if(!readFile.is_open()){ // if file cannot be opened
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }

    while(getline(readFile, line)){
        stringstream ss(line); // create a string stream from the line
        string indexOne, indexTwo, indexThree, indexFour, indexFive, indexSix;
        // get a copy of each entries
        getline(ss, indexOne, ','); // read the id (not used) and save to variable indexOne
        getline(ss, indexTwo, ','); // read the second entry (username or product name) and save to variable indexTwo
        getline(ss, indexThree, ','); // read the product category or password and save to variable indexThree
        getline(ss, indexFour, ','); // read the product subcategory or role and save to variable indexFour
        getline(ss, indexFive, ','); // get the product quantity and save to variable indexFive
        getline(ss, indexSix, ','); // get the product price and save to variable indexSix

        // since we only need to change the quantity 
        if(indexTwo == query){
            int currentQuantity = stoi(indexFive); // convert string to int, so that we can do arithmetic operations
            int purchasedQuantity = stoi(quantities); // convert string to int, so that we can do arithmetic operations
            int updatedQuantity = currentQuantity - purchasedQuantity;
            indexFive = to_string(updatedQuantity); // convert back to string and update the quantity
        }

        fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "," + indexFive + "," + indexSix + "\n";
        isFound = true;
    }

    if(!isFound){
        cout << "Product not found in database.\n";
        Sleep(1200);
        return;
    } 
    ofstream fileOut(productsDatabase);
    if(!fileOut){
        cout << "Cannot write to file " << productsDatabase << endl;
        Sleep(1200);
        return;
    }

    fileOut << fileContent;
    fileOut.close();

    admin.saveLogs("products", "UPDATE", query + "_Quantity_to_" + quantities, username, "Purchased_Quantity_Deducted");
    // no need for printing since we only need to deduct the quantities silently
}

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
    }

    for(const auto& item : cart){ // Iterate over each item (which is a vector<string>) in the cart.
        cout << "\nProduct name: " << item[0] << "\n";
        cout << "Category: " << item[1] << "\n";
        cout << "Quantity: " << item[2] << "\n";
        cout << "Price: P" << item[3] << "\n";

        totalAmount += stoi(item[3]) * stoi(item[2]);
    }

    cout << "\nTotal Amount: P" << totalAmount - (totalAmount * 0.12) << endl;
    cout << "VAT (12%): P" << (totalAmount) * 0.12 << endl;
    cout << "Amount Due: P" << (totalAmount) + ((totalAmount) * 0.12) << endl;

    cout << "\nDo you want to proceed to checkout? (1 = Yes, 0 = No): ";
    int confirmation;
    cin >> confirmation;

    if(handleInputError()) return false; // handle invalid inputs

    switch(confirmation){
        case 1: // will go back to processTransaction to complete the transaction
            return processTransaction(username); // this is the reason why viewCart is bool, so that if the user proceeds to checkout, we can know in the main menu to not break the loop
        case 0:
            cout << "Returning to main menu.\n";
            Sleep(1200);
            return false; // go back to main menu without breaking the whole loop
        default: // for invalid input
            cout << "Invalid selection. Returning to main menu.\n";
            Sleep(1200);
            return false;
    }
    return false; // just to satisfy the compiler, this line will never be reached
}

void POSCashier::saveTransaction(string productNames, string productQuantities, double initialAmount, double totalAmount, double change, string cashierName, string paymentMethod, int referenceID){
    string database;
    if(paymentMethod == "GCash"){
        database = "database/transactions/gcash_cashierTransactions.csv";
    } else if(paymentMethod == "Cash"){
        database = "database/transactions/cash_cashierTransactions.csv";
    }
    // get the current date and time
    time_t timestamp = time(NULL); // get current time, NULL since we are not interested to set a custom timestamp
    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *

    // dereference means that we get the value that the pointer is pointing to, because if it wasn't, it will return the address only (e.g. 0x123ff456)

    // we used ampersand in the timestamp to get the address pointer of the timestamp variable, otherwise, it will return an error since it expects a pointer, not an integer

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
    ofstream fout(database, ios::app); // fout is an instance of ofstream, used to write to files
    if (!fout.is_open()) { // if file cannot be opened
        cerr << "Error opening database for writing." << endl;
        return;
    }

    // now, check if the paymentMethod is Cash or GCash
    if(paymentMethod == "Cash"){
    // write the transaction details to the file
        fout << newId << ","
                << productNames << ","
                << productQuantities << ","
                << setprecision(2) << fixed << initialAmount << ","
                << setprecision(2) << fixed << initialAmount * 0.12 << ","
                << setprecision(2) << fixed << totalAmount << ","
                << setprecision(2) << fixed << change << ","
                << date << ","
                << time << ","
                << cashierName << endl;
        fout.close(); // close the file
        // if not closed, it may lead to data loss or corruption if the program ends unexpectedly
    } else if(paymentMethod == "GCash"){
        fout << newId << ","
                << productNames << ","
                << productQuantities << ","
                << setprecision(2) << fixed << initialAmount << ","
                << setprecision(2) << fixed << initialAmount * 0.12 << ","
                << setprecision(2) << fixed << totalAmount << ","
                << setprecision(2) << fixed << change << ","
                << referenceID << ","
                << date << ","
                << time << ","
                << cashierName << endl;
        fout.close(); // close the file
        // if not closed, it may lead to data loss or corruption if the program ends unexpectedly
    } else {
        cout << "Invalid payment method." << endl;
        Sleep(1000);
        return;
    }
}

// CRUD-Related Functions
bool POSCashier::processTransaction(string username) { // processTransaction is bool to indicate if the transaction was successful, if yes, it will go back to the main menu after the user had paid
    // also, if the user decided to cancel the transaction, it will return false to go back to the main menu without breaking the loop
    if(cart.empty()){ // if the cart is empty
        cout << "Your cart is empty. Cannot process transaction.\n";
        Sleep(1200);
        return false;
    }
 
    // get the current date and time
    time_t timestamp = time(NULL); // get current time, NULL since we are not interested to set a custom timestamp
    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *

    // dereference means that we get the value that the pointer is pointing to, because if it wasn't, it will return the address only (e.g. 0x123ff456)

    // we used ampersand in the timestamp to get the address pointer of the timestamp variable, otherwise, it will return an error since it expects a pointer, not an integer

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

    int confirmation;
    // make a function that will first print the order summary
    system("cls");
    cout << "---------------------------------" << endl;
    cout << "P.O.S (Cashier)" << endl ;
    cout << "---------------------------------" << endl;

    cout << "Order Summary\n" << endl;
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

        getline(ss, category, ','); // get the category
        getline(ss, percentageStr, ','); // get the discount percentage as string
        if (!percentageStr.empty()) { // if it's not empty
            categoryDiscounts[category] = stod(percentageStr) / 100.0; // convert percentage to decimal, and store it in the map according to its category
            // for example, for tops with 10%, it will store as: categoryDiscounts["Tops"] = 0.10 and will be stored as {"Tops", 0.10}
        }
    }
    discountFile.close(); // close the file

    map<string, int> categoryCounts; // to count the number of items in each category
    for (const auto& item : cart) { // loop through each item in the cart
        categoryCounts[item[1]] += stoi(item[2]); // count the quantity of each category in the cart
        itemTotal += stoi(item[3]) * stoi(item[2]); // calculate the total price of items before discount and VAT (this will be used for the overall total)
        //item[1] is category, item[2] is quantity, item[3] is price
    }

    for (const auto& [category, count] : categoryCounts) { // loop through each category and its count
        // Check if the category has 3 or more items AND has a discount defined.
        if (count >= 3 && categoryDiscounts.count(category) > 0) {
            // if this was >= instead of ==, it will only count exactly 3 times, ignoring the discount if there are more than 3 items
            // below will calculate the subtotal for the first 3 items in this category
            
            double subtotalForDiscount = 0.0;
            int itemsCounted = 0;

            // Find the first 3 items belonging to this category in the cart.
            for (const auto& item : cart) { // loop through each item in the cart
                if (item[1] == category) { // If the item is in the right category, note that item[1] is the category
                    int price = stoi(item[3]); // item[3] is the price of the item
                    int quantity = stoi(item[2]); // item[2] is the quantity of the item

                    // Process each unit of this item (e.g., if quantity is 2, loop twice)
                    for (int i = 0; i < quantity; ++i) {
                        if (itemsCounted < 3) { // Only count up to 3 items, otherwise, it should not include that item on the discount (as per buy 3 get __ discount)
                            subtotalForDiscount += price;
                            itemsCounted++;
                        } else {
                            break; // Stop counting for this item if we already have 3
                        }
                    }
                }
            }
            // Add the calculated discount for this category to the total discount amount.
            discountAmount += subtotalForDiscount * categoryDiscounts[category]; // apply the discount rate for this category
            // expected output for this is like 200 * 0.10 = 20.0 for Tops with 10% discount on subtotal of 200
        }
    }

    double rawPrice = itemTotal / 1.12; // total before VAT

    double discountedTotal = rawPrice - discountAmount; // total after discount but before VAT
    double vatAmount = discountedTotal * 0.12; // VAT on the discounted total
    double finalAmountDue = discountedTotal + vatAmount; // final amount due after discount and VAT

    cout << left << setw(35) << "Product Name" << setw(10) << "Qty" << "Price" << endl; // setw is used to set the width of the column, left is used to align the text to the left
    cout << "--------------------------------------------------" << endl;

    for(const auto& item : cart){ // for each item in the cart
        cout << left << setw(35) << item[0]  // print with left alignment and width of 35. item[0] is the product name
                << setw(10) << item[2] // print with width of 10. item[2] is the quantity
                << "P" << item[3] << endl; // item[3] is the price
    }
    cout << "\n\n";

    cout << "Total Amount: P" << setprecision(2) << fixed << rawPrice << endl; // raw price before VAT and discount. setprecision gets the first two decimal places
    cout << "Discounts Applied: P" << setprecision(2) << fixed << discountAmount << endl; // setprecision gets the first two decimal places. fixed makes sure that the decimal places are always shown, even if they are zeros

    cout << "VAT (12%): P" << setprecision(2) << fixed << vatAmount << endl; // setprecision gets the first two decimal places
    cout << "Amount Due: P" << setprecision(2) << fixed << finalAmountDue << endl; // final amount due after discount and VAT. setprecision gets the first two decimal places

    // setprecision gets the first two decimal places
    // fixed makes sure that the decimal places are always shown, even if they are zeros
    // because if we don't use fixed, it will show as, for example, 123.5 instead of 123.50

    cout << "\nProceed to purchase? (1 = Yes, 0 = No): ";
    cin >> confirmation;

    if(handleInputError()) return false; // handle invalid inputs

    int change;
    string paymentMethod;
    switch(confirmation){
        case 1: { // proceed to payment
            int referenceID;
            stringstream namesStream, quantitiesStream;
            for(int cart_count = 0; cart_count < cart.size(); cart_count++){ // iterate through the cart, and save it thorugh a stringstream for easier concatenation. we will use this for two purposes later, but, with this, we can get the product names and quantities in a single string, separated by semicolons
                namesStream << cart[cart_count][0]; // append the product name to the stream
                quantitiesStream << cart[cart_count][2]; // append the product quantity to the stream
                if(cart_count < cart.size() - 1){ // append semicolon everywhere except the last item
                    namesStream << ";";
                    quantitiesStream << ";";
                }
            }

            cout << "Enter the payment method (Cash/GCash): ";
            cin >> paymentMethod;

            if(paymentMethod != "Cash" && paymentMethod != "GCash"){ // this will check if the inputs are both NOT Cash or GCASH
                // if we were to use OR (||), it would always be true since the first condition would always be true for one of the options
                cout << "Invalid payment method. Please enter Cash or GCash only.\n";
                Sleep(1200);
                return false;
            }

            if(paymentMethod == "GCash"){ // process GCash payment
                paymentMethod = "GCash"; // this will be used for the receipt where it will show the payment method

                cout << "Enter the money received from the customer: ";
                cin >> userMoney;
                if(handleInputError()) return false; // handle invalid inputs
                change = userMoney - finalAmountDue;

                cout << "Enter the reference id: ";
                cin >> referenceID;
                if(handleInputError()) return false; // handle invalid inputs

                // power outtage might happen here after the user had paid, so we need to have a backup that will log the transaction even if the program crashes
                // in this manner, if the power outtage happens right after the user had paid, we can still recover the transaction from the backup file
                // start of code for backup redundancy
                ofstream backupFile("database/transactions/gcash_backup.csv", ios::app); // open in append mode
                if (!backupFile) {
                    cerr << "Error opening backup file for writing." << endl;
                    return false;
                }
                backupFile << "ProdNames,ProdQty,Amt,DcAmt,Tax,TotalAmt,UserMoney,Change,PmMethod,RefID,Date,Time,Cashier\n" 
                            << namesStream.str() << ","
                            << quantitiesStream.str() << ","
                            << setprecision(2) << fixed << rawPrice << ","
                            << setprecision(2) << fixed << discountAmount << ","
                            << setprecision(2) << fixed << vatAmount << ","
                            << setprecision(2) << fixed << finalAmountDue << ","
                            << setprecision(2) << fixed << userMoney << ","
                            << setprecision(2) << fixed << change << ","
                            << "GCash" << ","
                            << referenceID << ","
                            << date << ","
                            << time << ","
                            << username << "\n";
                            
                backupFile.close();
                // end of code for backup redundancy

                const string productsDatabase = "database/products.csv"; // path to products database, read only
                ifstream fileIn(productsDatabase); // open the products database to read the current quantities
                if(!fileIn.is_open()){
                    cout << "Failed to open file\n";
                    Sleep(1200);
                    return false;
                }

                string fileContent, line;
                while(getline(fileIn, line)){
                    stringstream ss(line); // helps us read each line by separating with commas
                    string productName, productQuantity, token;
                    getline(ss, token, ',');
                    getline(ss, productName, ','); // get the product name
                    getline(ss, token, ','); // skip the sub-category
                    getline(ss, token, ','); // skip the category
                    getline(ss, productQuantity, ','); // get the quantity

                    // since there are multiple products, we need to loop through the vector of product names
                    for(int cart_size = 0; cart_size < cart.size(); cart_size++){ // iterate through the cart
                        if(productName == cart[cart_size][0]){
                            int updatedQuantity = stoi(productQuantity) - stoi(cart[cart_size][2]); // subtract the quantity purchased | stoi means string to integer
                            deductPurchasedQuantities(productsDatabase, productName, username, cart[cart_size][2]);
                        }
                    }
                }

                // remove if not needed
                remove("database/transactions/gcash_backup.csv");
                
                // save the transaction to the main database
                saveTransaction(namesStream.str(), quantitiesStream.str(), rawPrice, finalAmountDue, change, username, "GCash", referenceID); // save the transaction to the database, specifically at the gcash_cashierTransactions.csv
            } else if(paymentMethod == "Cash"){ // process Cash payment
                paymentMethod = "Cash"; // this will be used for the receipt where it will show the payment method

                cout << "Enter the customer's money: ";
                cin >> userMoney;

                if(handleInputError()) return false; // handle invalid inputs

                change = userMoney - finalAmountDue; // calculate the change because this is a cash payment

                // check if the user has sufficient money
                if(userMoney < finalAmountDue){
                    int confirmation;
                    cout << "Insufficient money. Try again? (1 = Yes, 0 = No): ";
                    cin >> confirmation;

                    if(handleInputError()) return false; // handle invalid inputs

                    switch(confirmation){
                        case 1: // try again
                            return processTransaction(username);
                        case 0: // cancel transaction and clear the cart
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
                    // power outtage might happen here after the user had paid, so we need to have a backup that will log the transaction even if the program crashes
                    // in this manner, if the power outtage happens right after the user had paid, we can still recover the transaction from the backup file

                    // start of code for backup redundancy
                    ofstream backupFile("database/transactions/cash_backup.csv", ios::app); // open in append mode
                    if (!backupFile) {
                        cerr << "Error opening backup file for writing." << endl;
                        return false;
                    }
                    backupFile << "ProdNames,ProdQty,Amt,DcAmt,Tax,TotalAmt,UserMoney,Change,PaymentMethod,Date,Time,Cashier\n" 
                                << namesStream.str() << ","
                                << quantitiesStream.str() << ","
                                << setprecision(2) << fixed << rawPrice << ","
                                << setprecision(2) << fixed << discountAmount << ","
                                << setprecision(2) << fixed << vatAmount << ","
                                << setprecision(2) << fixed << finalAmountDue << ","
                                << setprecision(2) << fixed << userMoney << ","
                                << setprecision(2) << fixed << change << ","
                                << "Cash" << ","
                                << date << ","
                                << time << ","
                                << username << "\n";
                                
                    backupFile.close();
                    // end of code for backup redundancy

                    const string productsDatabase = "database/products.csv"; // path to products database, read only
                    ifstream fileIn(productsDatabase); // open the products database to read the current quantities
                    if(!fileIn.is_open()){
                        cout << "Failed to open file\n";
                        Sleep(1200);
                        return false;
                    }
                    
                    string fileContent, line;
                    while(getline(fileIn, line)){
                        stringstream ss(line); // helps us read each line by separating with commas
                        string productName, productQuantity, token;
                        getline(ss, token, ',');
                        getline(ss, productName, ','); // get the product name
                        getline(ss, token, ','); // skip the sub-category
                        getline(ss, token, ','); // skip the category
                        getline(ss, productQuantity, ','); // get the quantity

                        // since there are multiple products, we need to loop through the vector of product names
                        for(int cart_size = 0; cart_size < cart.size(); cart_size++){ // iterate through the cart
                            if(productName == cart[cart_size][0]){
                                int updatedQuantity = stoi(productQuantity) - stoi(cart[cart_size][2]); // subtract the quantity purchased | stoi means string to integer
                                deductPurchasedQuantities(productsDatabase, productName, username, cart[cart_size][2]);
                            }
                        }
                    }
                    
                    // save the transaction to the main database
                    saveTransaction(namesStream.str(), quantitiesStream.str(), rawPrice, finalAmountDue, change, username, "Cash");

                    // remove the backup file after saving the transaction to the main database
                    remove("database/transactions/cash_backup.csv");

                    cout << "Change: " << change << endl;
                }
            } 
            system("pause");
            break;
        }
        case 0:
            int choice;
            cout << "\nDo you want to cancel the transaction? (1 = Yes, 0 = No): ";
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

    system("cls");
    cout << "---------------------------------" << endl;
    cout << "P.O.S (Cashier)" << endl ;
    cout << "---------------------------------" << endl;

    cout << "Threads and Charms" << endl;
    cout << date << " " << time << endl; 
    cout << "\n";

    cout << left << setw(35) << "Product Name" << setw(10) << "Qty" << "Price" << endl; // setw is used to set the width of the column, left is used to align the text to the left
    cout << "--------------------------------------------------" << endl;

    for(const auto& item : cart){ // for each item in the cart. auto means that the compiler will automatically detect the type of the variable
        cout << left << setw(35) << item[0] // print with left alignment and width of 35. item[0] is the product name
                << setw(10) << item[2]  // print with width of 10. item[2] is the quantity
                << "P" << item[3] << endl; // item[3] is the price
    }
    cout << "\n\n";

    //setprecision gets the first two decimal places
    //fixed makes sure that the decimal places are always shown, even if they are zeros
    cout << "Money tendered: P" << setprecision(2) << fixed << userMoney << endl;
    cout << "Less: Discounts: P" << setprecision(2) << fixed << discountAmount << endl;

    cout << "Total before VAT: P" << setprecision(2) << fixed << discountedTotal << endl;
    cout << "VAT (12%): P" << setprecision(2) << fixed << vatAmount << endl;
    cout << "Amount Due: P" << setprecision(2) << fixed << finalAmountDue << endl;
    cout << "Payment Method: " << paymentMethod << endl;
    cout << "Change: P" << setprecision(2) << fixed << change << endl;

    cout << "\n";

    // clear the cart after printing the receipt
    cart.clear();
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
        if (row.size() > 3 && row[3] == subCategory) { // if row size is greater than 3 to avoid out of range error and if the sub-category matches the desired sub-category
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
        if (stoi(row[0]) == selectedId) { // row[0] is the product ID
            productName = row[1]; // row[1] is the product name
            productCategory = row[2]; // row[2] is the product category
            productQuantity = stoi(row[4]); // row[4] is the product quantity
            productPrice = stoi(row[5]); // row[5] is the product price

            if (productQuantity == 0){ // if the product is out of stock
                cout << "Product is out of stock!\n";
                system("pause");
                return false;
            } else { // just to make sure that the product is found, since this is a loop. we don't want unnecessary iterations
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
    } else if(quantityToPurchase < 0){ // if the quantity is negative
        cout << "Quantity cannot be negative. Please enter a valid integer." << endl;
        Sleep(1200);
        return false;
    }

    // ask if the user wants to add more products to the cart
    cout << "Product added successfully. Do you want to add more? (1 = Yes, 0 = No): ";
    int addMore;
    cin >> addMore;

    if(handleInputError()) return false; // handle invalid inputs

    // save or append the previous product
    cart.push_back({productName, productCategory, to_string(quantityToPurchase), to_string(productPrice)});

    switch(addMore){
        case 1: // User wants to add more products
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

    return processTransaction(username); // proceed to process the transaction (this is where if 0 is selected, it will proceed to payment)
};