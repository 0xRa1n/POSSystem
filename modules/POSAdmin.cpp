#include <iostream>
#include <cctype> // for toupper() and tolower()
#include <sstream> // for stringstream
#include <cstdlib> // for exit()
#include <windows.h> // for Sleep() and system("cls")
#include <bits/stdc++.h> // for this file, here are the functions used along with this package: fstream, vector, string, ctime (time.h), sstream, regex, iomanip (setw and left), limits, stdexcept (invalid_argument and out_of_range)
#include <algorithm> // used for find
#include <regex> // for regex
using namespace std;

// ifstream == reading the file
// offstream == writing to the file

#include "POSAdmin.h"
// include the declaration of POSAdmin class
// and in here we will define all the member functions of POSAdmin

const regex disallowed{R"([^A-Za-z0-9_@#&])"};
// the brackets tells us that its a character set, and the ^ means NOT
// and only A-Z, a-z, 0-9, _ @ # & are allowed
// the R"()" syntax is used to define raw string literals, which allows us to include special characters without needing to escape them

// CREATE
void POSAdmin::addProduct(string database, string username) {
    string productName, productSubCategory;
    int quantity, price;

    // ask the user for the product name (if 0 is entered, it will go back to the menu)
    cout << "Enter the product name (type 0 to go back): ";
    cin >> productName;
    
    // if user changes his mind
    if (productName == "0") return;

    // check for invalid characters
    if(regex_search(productName, disallowed)){
        cout << "Product name cannot contain spaces or commas, or any special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // use the function to check if the entry is already in the database
    if (isAlreadyInCsv(database, productName)) {
        cout << "Product '" << productName << "' is already in the CSV.\n";
        Sleep(1200);
        return;
    } else {
        cout << "Enter the product category (Tops/Bottoms/Accessories, 0 to go back): ";
        string productCategory;
        cin >> productCategory;

        if (productCategory == "0") return;
        if(regex_search(productCategory, disallowed)){
            cout << "Product category cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
            Sleep(1200);
            return;
        }
        productCategory[0] = toupper(productCategory[0]); // capitalize the first letter, if it wasn't capitalized
        vector<string> categories = {"Tops", "Bottoms", "Accessories"}; //vector is used because .find() works better with vectors than arrays (arrays work, but, it will be more complicated)
        auto lookFor = find(categories.begin(), categories.end(), productCategory); // search for the category in the vector categories

        if(lookFor == categories.end()){ // if it reached the end, it means it was not found
            cout << "Invalid category. Please enter Tops, Bottoms, or Accessories only.\n";
            Sleep(1200);
            return;
        }

        // ask the user if what product sub-category is the product
        cout << "Enter the product sub-category (0 to go back): ";
        cin >> productSubCategory;

        if(productSubCategory == "0") return;
        if(regex_search(productSubCategory, disallowed)){
            cout << "Product sub-category cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
            Sleep(1200);
            return;
        }

        // ask the user for the product's quantity and price
        cout << "Enter the quantity (0 to go back): "; // and quantity SHOULD not be zero
        cin >> quantity;

        if(handleInputError()) return; // handle invalid inputs (if cin fails, it will return true)
        if (quantity == 0) return;

        if(quantity < 0){ // quantity cannot be negative
            cout << "Quantity cannot be negative. Please enter a valid integer." << endl;
            Sleep(1200);
            return;
        }

        cout << "Enter the price (0 to go back): "; // price also should NOT be zero
        cin >> price;
        
        if(handleInputError()) return; // handle invalid inputs
        if (price == 0) return;

        if(price < 0){ // price cannot be negative
            cout << "Price cannot be negative. Please enter a valid integer." << endl;
            Sleep(1200);
            return;
        } else {
            // get the last product id from the database
            int newId = getLastId(database) +  1;

            // open the database file, and add the new product
            fstream fout;
            fout.open(database, ios::out | ios::app); // append or output mode

            fout << newId << ","
                << productName << ","
                << productCategory << ","
                << productSubCategory << ","
                << quantity << ","
                << price << "\n";

            fout.close();                    
            saveLogs("products", "ADD", productName, username, "New_Product_Added");
            cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
            Sleep(1200);
        }  
    }
}

void POSAdmin::addUser(string database, string accessingUsername){
    string username, password, role;
                            
    // ask the user for the username (if 0 is entered, it will go back to the menu)
    cout << "Enter the username you want to add (type 0 to go back): ";
    cin >> username;

    // use the function to check if the entry is already in the database
    if(isAlreadyInCsv(database, username)){
        cout << "User is already in the database!";
    }

    // if the user decides to change his mind
    if (username == "0") return;

    if(regex_search(username, disallowed)){
        cout << "Username cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // ask for the password and role
    cout << "Enter the password (0 to go back): ";
    cin >> password;

    if (password == "0") return;

    if(regex_search(password, disallowed)){
        cout << "Password cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    cout << "What is the role of the user? (Admin/Manager/Cashier, 0 to go back): ";
    cin >> role;

    if (role == "0") return;

    role[0] = toupper(role[0]); // capitalize the first letter, if it wasn't capitalized
    vector<string> roles = {"Admin", "Manager", "Cashier"};
    auto lookFor = find(roles.begin(), roles.end(), role); // search for the role in the vector roles

    if(lookFor == roles.end()){
        cout << "Invalid role. Please enter Admin, Manager, or Cashier only.\n";
        Sleep(1200);
        return;
    } else {
        // get the last user id from the dtabase
        int lastUserId = getLastId(database);
        int newUserId = lastUserId + 1;

        fstream fout; // create a file stream object with the variable fout
        fout.open(database, ios::out | ios::app); // output or append mode

        // open the database file, and add the new product
        fout << newUserId << ","
            << username << ","
            << password << ","
            << role << "\n";

        fout.close();
        cout << "Successfully added user " << username << endl;
        saveLogs("accounts", "ADD", username, accessingUsername, "New_User_Added");
    }
    // wait for 1.2 seconds before going back to the menu
    Sleep(1200);
}

void POSAdmin::saveLogs(string type, string operation, string affectedEntry, string adminName, string message) {
    // use different log files for different types
    string database;
    if(type == "accounts"){
        database = "database/logs/adminUserLogs.csv";
    } else if(type == "products"){
        database = "database/logs/productsLogs.csv";
    } else {
        cout << "Invalid log type specified." << endl;
        Sleep(1200);
        return;
    }

    // get the current date and time
    time_t timestamp = time(nullptr); // we do not need to assign a timestamp, we need the current time only, so nullptr is used

    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *

    // dereference means that we get the value that the pointer is pointing to, because if it wasn't, it will return the address only (e.g. 0x123ff456)

    // we used ampersand in the timestamp to get the address pointer of the timestamp variable, otherwise, it will return an error since it expects a pointer, not an integer

    char date[50];
    char time[50];

    // char are used because ctime functions expect char arrays (and is coded in C to accept character arrays)

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

    // open the transactions.csv file in append mode
    ofstream fout(database, ios::app); // fout is an instance of ofstream, used to write to files
    if (!fout) {
        cerr << "Error opening database for writing." << endl;
        return;
    }

    // write the transaction details to the file
    fout << operation << ","
        << affectedEntry << ","
        << date << ","
        << time << ","
        << adminName << ","
        << message << endl;

    fout.close(); // close the file
    // if not closed, it may lead to data loss or corruption if the program ends unexpectedly
}

// READ
void POSAdmin::readProducts(string database) {
    // Open the file
    ifstream file(database);

    // check if file exists
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        return;
    }

    // Read all rows first
    vector<vector<string>> rows; // 2D vector to hold rows and columns || the output should looks like this: {{"ID", "ProductName", "SubCategory", "Quantity", "Price"}, {"1", "Product1", "SubCat1", "10", "100"}, ...}
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row; 
        while (getline(ss, cell, ',')) { // split by comma
            row.push_back(cell); // add each cell to the row || row will look like this: {"ID", "ProductName", "SubCategory", "Quantity", "Price"}
        }
        rows.push_back(row); // add the row to the list of rows || rows will look like this: {{"ID", "ProductName", "SubCategory", "Quantity", "Price"}, {"1", "Product1", "SubCat1", "10", "100"}, ...
    }
    file.close(); // close the file

    // stop if the rows are empty
    if (rows.empty()){
        cout << "No products found in the database.\n";
        return;
    }

    // Find max width of each column
    size_t cols = 0;
    for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement
    vector<size_t> widths(cols, 0); // using the max column size from the variable cols, use it to initialize widths with 0
    // initially: widths is {0, 0, 0, 0, 0} for 5 columns

    // after we get the maximum number, we will update the widths vector
    // this will make sure that each value will not overlap with each other

    for (auto &r : rows) { 
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement
    }

    // Add a little padding for readability
    for (auto &w : widths) w += 2;

    // Print
    for (auto &r : rows) {
        for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }
}

void POSAdmin::readBackupTransactions(string database){
    // Open the file
    ifstream file(database);

    // check if file exists
    if (!file.is_open()) {
        cout << "File could not be opened.\n";
        return;
    }

    // Read all rows first
    vector<vector<string>> rows; // 2D vector to hold rows and columns || the output should looks like this: {{"ID", "Username", "Password", "Role"}, {"1", "User1", "Pass1", "Admin"}, ...}
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row; 
        while (getline(ss, cell, ',')) { // split by comma
            row.push_back(cell); // add each cell to the row || row will look like this: {"ID", "Username", "Password", "Role"}
        }
        rows.push_back(row); // add the row to the list of rows || rows will look like this: {{"ID", "Username", "Password", "Role"}, {"1", "User1", "Pass1", "Admin"}, ...
    }
    file.close(); // close the file

    // stop if the rows are empty
    if (rows.empty()){
        cout << "No backup transactions found in the database.\n";
        return;
    }

    // Find max width of each column
    size_t cols = 0;
    for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement
    vector<size_t> widths(cols, 0); // using the max column size from the variable cols, use it to initialize widths with 0
    // initially: widths is {0, 0, 0, 0} for 4 columns

    // after we get the maximum number, we will update the widths vector
    // this will make sure that each value will not overlap with each other

    for (auto &r : rows) { 
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement
    }
    // Add a little padding for readability
    for (auto &w : widths) w += 2;
    // Print
    for (auto &r : rows) {
        for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }
}

void POSAdmin::getAllAccounts(string database) {
    // Open the file
    ifstream file(database);

    // check if file exists
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        return;
    }

    // Read all rows first
    vector<vector<string>> rows; // 2D vector to hold rows and columns || the output should looks like this: {{"ID", "Username", "Password", "Role"}, {"1", "User1", "Pass1", "Admin"}, ...}
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row; 
        while (getline(ss, cell, ',')) { // split by comma
            row.push_back(cell); // add each cell to the row || row will look like this: {"ID", "Username", "Password", "Role"}
        }
        rows.push_back(row); // add the row to the list of rows || rows will look like this: {{"ID", "Username", "Password", "Role"}, {"1", "User1", "Pass1", "Admin"}, ...
    }
    file.close(); // close the file

    // stop if the rows are empty
    if (rows.empty()){
        cout << "No accounts found in the database.\n";
        return;
    }

    // Find max width of each column
    size_t cols = 0;
    for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement
    vector<size_t> widths(cols, 0); // using the max column size from the variable cols, use it to initialize widths with 0
    // initially: widths is {0, 0, 0, 0} for 4 columns

    // after we get the maximum number, we will update the widths vector
    // this will make sure that each value will not overlap with each other

    for (auto &r : rows) { 
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement
    }
    // Add a little padding for readability
    for (auto &w : widths) w += 2;
    // Print
    for (auto &r : rows) {
        for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }
}

void POSAdmin::getTotalSales(string database){
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

void POSAdmin::getTodaysSales(string database){
    // we should first read the csv, then get the current date and the date from the csv. Then, starting there, get the amount
    string line;
    int todaysSales = 0;

    // open the file
    ifstream file(database);

    // get the current date
    time_t timestamp = time(NULL);
    struct tm datetime = *localtime(&timestamp);
    char date[50]; // since it points to a char array
    strftime(date, 50, "%m_%d_%y", &datetime);
    string currentDate(date);
    
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
        // skip the first four tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // get the amount
        string amount = token;
        getline(ss, token, ','); // get the date
        string transactionDate = token;

        if(transactionDate == currentDate){
            todaysSales += stoi(amount); // convert to integer and add to today's sales
        }
    }
    file.close();
    cout << "Today's Sales: P" << todaysSales << endl;
}

void POSAdmin::getAllLogs(string type){
    string database;
    if(type == "accounts"){
        database = "database/logs/adminUserLogs.csv";
    } else if(type == "products"){
        database = "database/logs/productsLogs.csv";
    } else if(type == "cashier"){
        database = "database/transactions/cashierTransactions.csv";
    } else {
        cout << "Invalid log type specified." << endl;
        Sleep(1200);
        return;
    }

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
        vector<string> row; // this will hold a single row, example output of this is {"ADD", "Product1", "12_31_23", "10:00:00_AM", "AdminUser"} 
        while (getline(ss, cell, ',')) { // split by comma
            row.push_back(cell); // add each cell to the row
        }
        rows.push_back(row); // add the row to the list of rows
    }
    file.close(); // close the file

    // stop if the rows are empty
    if (rows.empty()){
        cout << "No logs found in the database.\n";
        return;
    }

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
    system("pause");
}

// UPDATE
int POSAdmin::updateInformation(string filename, string query, string type, string newValue, string username, string reason) {
    ifstream fileIn(filename); // open an input file stream, since we are reading the file first, then, writing it back
    if (!fileIn.is_open()) {
        return 0;
    }

    string fileContent, line;
    bool found = false;

    while (getline(fileIn, line)) {
        stringstream ss(line); // create a string stream from the line
        string indexOne, indexTwo, indexThree, indexFour, indexFive, indexSix;
        // get a copy of each entries
        getline(ss, indexOne, ','); // read the id (not used) and save to variable indexOne
        getline(ss, indexTwo, ','); // read the second entry (username or product name) and save to variable indexTwo
        getline(ss, indexThree, ','); // read the product category or password and save to variable indexThree
        getline(ss, indexFour, ','); // read the product subcategory or role and save to variable indexFour
        getline(ss, indexFive, ','); // get the product quantity and save to variable indexFive
        if(type.find("product") != string::npos){ // if the type contains the word "product"
            getline(ss, indexSix, ','); // get the product price and save to variable indexSix
        }

        // if-elseif has been used since switch case does not support strings in C++
        // modify the entry if it matches the query
        if (indexTwo == query) {
            // update the corresponding field
            // product
            if(type == "productName"){
                indexTwo = newValue; // update product name
            } else if(type == "productQuantity"){
                indexFive = newValue; // update product quantity
            } else if(type == "productPrice"){
                indexSix = newValue; // update product price
            }  else if(type == "productSubCategory"){
                indexFour = newValue; // update product sub-category
            } else if(type == "productCategory"){
                indexThree = newValue; // update product category
            }
            // account
            else if(type == "accountUsername"){
                indexTwo = newValue; // update account username 
            } else if(type == "accountPassword"){
                indexThree = newValue; // update account password
            } else if(type == "accountRole"){
                indexFour = newValue; // update account role
            }
            found = true;
        }
        // write back to the file content
        // create a copy of each line, then, modify the index that the user wants to change
        if(type.find("product") != string::npos){
            fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "," + indexFive + "," + indexSix + "\n";
        } else { // account
            fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "\n";
        }
        // we cannot use the variable line here since it still holds the old value, and this function's purpose is to update the value
    }
    fileIn.close(); // close the file

    if (!found) {
        return 0; // query not found, will exit the function
    } else {
        ofstream fileOut(filename);
        if (!fileOut) {
            cout << "Cannot write to file " << filename << endl;
        }

        fileOut << fileContent;
        fileOut.close();

        // we will log EVERY action made by the manager or admin
        if(type.find("account") != string::npos){ // check if the type contains the word "account"
            // string::npos is a very large number representing "not found"
            // If the substring is not found, it returns the special value string::npos
            if(type == "accountPassword"){
                saveLogs("accounts", "UPDATE", query + "_PW_to_" + newValue, username, reason);
            } else {
                saveLogs("accounts", "UPDATE", query + "_to_" + newValue, username, reason);
            }
        } else if(type.find("product") != string::npos){ // check if the type contains the word "product"
            if(type == "productName"){
                saveLogs("products", "UPDATE", query + "_to_" + newValue, username, reason);
            } else {
                if(type == "productPrice"){
                    saveLogs("products", "UPDATE", query + "_Price_to_" + newValue, username, reason);
                } else if(type == "productQuantity"){
                    saveLogs("products", "UPDATE", query + "_Quantity_to_" + newValue, username, reason);
                } else if(type == "productSubCategory"){
                    saveLogs("products", "UPDATE", query + "_SubCategory_to_" + newValue, username, reason);
                } else {
                    saveLogs("products", "UPDATE", query + "_to_" + newValue, username, reason);
                }
            }
        }
        return 1;
    }
}

void POSAdmin::updateDiscounts(string username) {
    string category;
    ifstream discountFile("database/discounts.csv");
    if(!discountFile.is_open()){
        cout << "Failed to open discounts database.\n";
        return;
    }
    cout << "Enter the category (Tops/Bottoms/Accessories) to update discount (0 to go back): ";
    cin >> category;
    vector<string> categories = {"Tops", "Bottoms", "Accessories"};

    if(category == "0") return;
    if(regex_search(category, disallowed)){
        cout << "Category cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }
    
    category[0] = toupper(category[0]); // capitalize the first letter, if not already
    auto lookFor = find(categories.begin(), categories.end(), category); // search for the category
    if(lookFor == categories.end()){
        cout << "Invalid category. Please enter Tops, Bottoms, or Accessories only.\n";
        Sleep(1200);
        return;
    }

    cout << "Enter the new discount percentage (0-100, 0 to go back): ";
    int discountPercentage;
    cin >> discountPercentage;

    if(handleInputError()) return; // handle invalid inputs
    if(discountPercentage == 0) return;

    if(discountPercentage < 0 || discountPercentage > 100){
        cout << "Discount percentage must be between 0 and 100.\n";
        Sleep(1200);
        return;
    }

    // read the file and update the discount
    string line;
    string fileContent;
    while(getline(discountFile, line)){
        stringstream ss(line); // create a string stream from the line
        // again, stringstream helps us read a string as if it were a stream (like cin)
        // without stringstream, it would be hard to read a string by delimiter (e.g., comma)
        string token;
        string fileCategory, fileDiscountPercentage;
        getline(ss, fileCategory, ','); // read category || delimiter is comma (where it will end the reading)
        getline(ss, fileDiscountPercentage, ','); // read discount percentage

        if(fileCategory == category){
            // update the discount percentage
            fileContent += category + "," + to_string(discountPercentage) + "\n";
        } else {
            fileContent += line + "\n"; // keep the line as is
        }
    }
    discountFile.close();

    // write back to the file
    ofstream discountFileOut("database/discounts.csv");
    if(!discountFileOut.is_open()){
        cout << "Failed to open discounts database for writing.\n";
        return;
    }
    discountFileOut << fileContent;
    discountFileOut.close();

    cout << "Successfully updated discount for " << category << " to " << discountPercentage << "%.\n";
    saveLogs("products", "UPDATE", category + "_Discount_to_" + to_string(discountPercentage), username, "Discount_Updated");
    Sleep(1200);
}

void POSAdmin::updateProductFields(string type, string database, string field, string username){
    system("cls"); // clear the console

    cout << "---------------------------------" << endl;
    cout << "Update " + type + " " + field << endl;
    cout << "---------------------------------" << endl;
    string originalInputName;

    cout << "Enter the " + type + " name (type 0 to go back): ";
    cin >> originalInputName;

    // just somehow if the user decides to change his mind
    if(originalInputName == "0") return;

    if(regex_search(originalInputName, disallowed)){
        cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }
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
    } else {
        cout << "Enter the new " + type + " " + field + " (0 to go back): ";
        cin >> newInputField;

        if (newInputField == "0") return;

        // error handling for different fields
        if(field == "name" || field == "subCategory" || field == "username" || field == "password" || field == "role"){
            if(regex_search(newInputField, disallowed)){
                cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
                Sleep(1200);
                return;
            } else if(field == "name" && isAlreadyInCsv(database, newInputField)){
                cout << "Product name '" << newInputField << "' is already in the CSV.\n";
                Sleep(1200);
                return;
            } else if(field == "username" && isAlreadyInCsv(database, newInputField)){
                cout << "Username '" << newInputField << "' is already in the database!\n";
                Sleep(1200);
                return;
            }
        } else if(field == "price" || field == "quantity"){
            // make sure that the new input is a valid integer
            // note that the function handleInputError() will only work for INTEGERS entered via cin
            try {
                int value = stoi(newInputField);
                if(value < 0){
                    cout << "The " + field + " cannot be negative.\n";
                    Sleep(1200);
                    return;
                }
            } catch (invalid_argument&) {
                cout << "Invalid input, please enter a valid integer for " + field + ".\n";
                Sleep(1200);
                return;
            } catch (out_of_range&) {
                cout << "The number entered is out of range for " + field + ".\n";
                Sleep(1200);
                return;
            }
        } 
        field[0] = toupper(field[0]); // capitalize the first letter of the field, if it wasn't capitalized

        // ask the user for the reason for updating the entry
        cout << "Please provide a reason for updating the " + type + " " + field
                << " (e.g., 'Correcting_Price', 'Updating_Quantity', etc. And make sure to use underscores instead of spaces): ";
        string reason;

        // Clear the input buffer to remove any leftover newline characters from previous 'cin' operations.
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        // cin.ignore already ignores the previous inputs
        // but, we are also telling it to remove to delete a large number of characters until it finds a newline character
        // \n is the delimiter, it means that it will end if it finds the end of the line
        // it considers the key enter as \n

        // Read the entire line, including spaces.
        getline(cin, reason);

        // getline can be used for reading any input stream, including standard input (cin) and file streams (ifstream, ofstream).

        if(reason.empty()){
            cout << "Reason cannot be empty.\n";
            Sleep(1200);
            return;
        } else if(regex_search(reason, disallowed)){
            cout << "Invalid reason, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
            Sleep(1200);
            return;
        }

        if(updateInformation(database, originalInputName, type + field, newInputField, username, reason) == 1){ // since I am only using this function one time inside this function
            cout << "Updated successfully.\n";
        } else {
            cout << "The product " + type + " was not found.\n";
        }
        Sleep(1200);
        return;
    }
}

// DELETE
void POSAdmin::deleteInformation(string type, string filename, string username){
    string deleteProductInput;

    cout << "Enter the entry name you want to delete (type 0 to go back): ";
    cin >> deleteProductInput;

    if(regex_search(deleteProductInput, disallowed)){
        cout << "Entry name cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

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
        getline(ss, token, ',');     // read the second entry
        if(token != deleteProductInput){
            rows.push_back(line); // keep the line if it doesn't match
        } else {
            found = true;
        }
        // at this point, the vector rows now holds the value except the line that matches the query
    }
    input_file.close();

    if (!found) {
        cout << "Query not found";
    } else {
        // Write back filtered rows
        ofstream output_file(filename, ios::trunc); // open in truncate mode
        // it avoids the line that has the query, and it rewrites the csv without it
        for (const auto& row : rows) { // write each remaining row
            output_file << row << "\n"; // now, write the contents of the variable rows back to the file
        } 
        output_file.close();

        if(type == "accounts"){ // save to the account logs
            saveLogs("accounts", "DELETE", deleteProductInput, username, "Account_Deleted");
        } else if(type == "products"){ // save to the product logs
            saveLogs("products", "DELETE", deleteProductInput, username, "Product_Deleted");
        }

        cout << "Successfully deleted " << deleteProductInput << endl;
    }
    Sleep(1200);
}