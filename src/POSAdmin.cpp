#include "./include/POSAdmin.h"

// include all the packages that we only need IN THIS FILE
#include <iostream>
#include <cctype> // for toupper() and tolower()
#include <sstream> // for stringstream
#include <cstdlib> // for exit()
#include <algorithm> // used for find
#include <regex> // for regex
#include <fstream> // for fstream, ifstream
#include <iomanip> // for setw and left

// ifstream == reading the file
// offstream == writing to the file

// include the declaration of POSAdmin class
// and in here we will define all the member functions of POSAdmin

const regex disallowed{R"([^A-Za-z0-9_@#&])"}; // const so that it cannot be changed
// the brackets tells us that its a character set, and the ^ means NOT
// and only A-Z, a-z, 0-9, _ @ # & are allowed
// the R"()" syntax is used to define raw string literals, which allows us to include special characters without needing to escape them

// here are all member functions of POSAdmin class
// the double colon (::) is the scope resolution operator, which tells us that the function belongs to the class POSAdmin

// CREATE
void POSAdmin::addProduct(string database, string username) {
    string productName, productSubCategory;
    int quantity;
    double price;

    // ask the user for the product name (if 0 is entered, it will go back to the menu)
    cout << "Enter the product name (0 = Cancel): ";
    cin >> productName;
    
    // if user changes his mind
    if (productName == "0") return;

    // check for invalid characters
    if(regex_search(productName, disallowed)){
        cout << "Product name cannot contain spaces or commas, or any special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // use the function isAlreadyInCsv to check if the entry is already in the database (will return true if found)
    if (isAlreadyInCsv(database, productName)) {
        cout << "Product '" << productName << "' is already in the CSV.\n";
        Sleep(1200);
        return;
    }

    // ask the user for the product category
    cout << "Enter the product category (Tops/Bottoms/Accessories, 0 = Cancel): ";
    string productCategory;
    cin >> productCategory;

    if (productCategory == "0") return;
    if(regex_search(productCategory, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Product category cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // if productCategory input from user is neither tops, bottoms, or accessories
    if(productCategory != "Tops" && productCategory != "Bottoms" && productCategory != "Accessories"){
        cout << "Invalid category. Please enter Tops, Bottoms, or Accessories only.\n";
        Sleep(1200);
        return;
    } 

    // ask the user if what product sub-category is the product
    cout << "Enter the product sub-category (0 = Cancel): "; // no specific sub-categories since they are many
    cin >> productSubCategory;

    if(productSubCategory == "0") return;
    if(regex_search(productSubCategory, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Product sub-category cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // ask the user for the product's quantity and price
    cout << "Enter the quantity (0 = Cancel): "; // and quantity SHOULD not be zero
    cin >> quantity;

    if(handleInputError()) return; // handle invalid inputs (if cin fails, it will return true)
    if (quantity == 0) return;

    if(quantity < 0){ // quantity cannot be negative
        cout << "Quantity cannot be negative. Please enter a valid integer." << endl;
        Sleep(1200);
        return;
    }

    // since the input can have a decimal, we should use getline (because if we enter a decimal point, cin will think its the end of input. and it will trigger the invalid input error)
    string priceInput;
    cout << "Enter the price (0 = Cancel): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, priceInput);

    // we used cin.ignore() along with getline to get a value that has a decimal point
    // this is because cin will stop reading the input at the first whitespace, so if the user enters a decimal point, it will stop reading there
    // cin.ignore() will ignore the newline character left in the input buffer by the previous cin
    // input buffer means the data that is left in the input stream after a previous input operation

    if (priceInput == "0") return;

    // try parsing the price and converting it to double
    try {
        price = stod(priceInput);
        if(price < 0){ // price cannot be negative
            cout << "Price cannot be negative. Please enter a valid number." << endl;
            Sleep(1200);
            return;
        }
    } catch (...) { // catch any exceptions
        cout << "Invalid input. Please enter a valid number." << endl;
        Sleep(1200);
        return;
    }

    // get the last product id from the database
    int newId = getLastId(database) +  1;

    // open the database file, and add the new product
    fstream fout; // create a file stream object with the variable fout, so that we can write to the file
    fout.open(database, ios::app); // this tells the program that the cursor should be placed at the end of the file, avoiding overwritting existing data

    fout << newId << ","
        << productName << ","
        << productCategory << ","
        << productSubCategory << ","
        << quantity << ","
        << fixed << setprecision(2) << price << "\n";

    fout.close(); // close the file

    saveLogs("products", "ADD", productName, username, "New_Product_Added"); // log the addition of product to productsLogs.csv
    cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
    Sleep(1200); // wait for 1.2 seconds before going back to the menu
}

void POSAdmin::addUser(string database, string accessingUsername){
    string username, password, role;
                            
    // ask the user for the username (if 0 is entered, it will go back to the menu)
    cout << "Enter the username you want to add (0 = Cancel): ";
    cin >> username;

    // use the function to check if the entry is already in the database
    if(isAlreadyInCsv(database, username)){
        cout << "User is already in the database!";
        Sleep(1200);
        return;
    }

    // if the user decides to change his mind
    if (username == "0") return;

    if(regex_search(username, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Username cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // ask for the password and role
    cout << "Enter the password (0 = Cancel): ";
    cin >> password;

    if (password == "0") return;

    if(regex_search(password, disallowed)){
        cout << "Password cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    cout << "What is the role of the user? (Admin/Manager/Cashier, 0 = Cancel): ";
    cin >> role;

    if (role == "0") return; // if the user changes his mind

    if(regex_search(role, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Role cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    if(role != "Admin" && role != "Manager" && role != "Cashier"){
        cout << "Invalid role. Please enter Admin, Manager, or Cashier only.\n";
        Sleep(1200);
        return;
    } 

    // get the last user id from the dtabase
    int lastUserId = getLastId(database);
    int newUserId = lastUserId + 1;

    fstream fout; // create a file stream object with the variable fout, so that we can write to the file
    fout.open(database, ios::app); // output or append mode

    // open the database file, and add the new product
    fout << newUserId << ","
        << username << ","
        << password << ","
        << role << "\n";

    fout.close(); // close the file
    cout << "Successfully added user " << username << endl;
    saveLogs("accounts", "ADD", username, accessingUsername, "New_User_Added"); // log the addition of user to adminUserLogs.csv

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
        cout << "Error opening database for writing." << endl;
        return;
    }

    // write the transaction details to the file (according to the header format in the CSV file)
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
        stringstream ss(line); // lets us read each line
        string cell; // temporary variable to hold each cell value
        vector<string> row; // temporary vector to hold each row
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

    // Find the maximum number of columns
    size_t cols = 0;
    for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement

    vector<size_t> widths(cols, 0); // using the max column size from the variable cols, use it to initialize widths with 0
    // if there are 6 columns in the csv, then cols = 6 and it will initialize widths to {0, 0, 0, 0, 0, 0}

    // after we get the maximum number, we will update the widths vector
    // this will make sure that each value will not overlap with each other

    for (auto &r : rows) { 
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement

            // it does this for each value in the widths vector
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
            
            // it does this for each value in the widths vector
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
            
            // it does this for each value in the widths vector
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

void POSAdmin::getDailySales(){
    // location of the gcash and cash transactions
    string cashDatabase = "database/transactions/cash_cashierTransactions.csv";
    string gcashDatabase = "database/transactions/gcash_cashierTransactions.csv";

    // read sales from both databases
    ifstream cashFile(cashDatabase);
    ifstream gcashFile(gcashDatabase);

    if(!cashFile.is_open() || !gcashFile.is_open()){
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    // get the current date
    time_t timestamp = time(NULL); // set the time as null so that it will return the current timestamp
    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *, and we used ampersand inside the argument to get the address pointer of the timestamp variable, as it is expecting an address
    char date[50]; // since it points to a char array
    strftime(date, 50, "%m_%d_%y", &datetime); // format the date as mm_dd_yy. 50 means the maximum length (size) of the char array
    string currentDate(date); // we need to convert it from char array to string for easier comparison later
    // if you would notice, in the POSCashier, we used the same format for the date when saving transactions
    // we do not have to do this conversion in POSCashier.cpp because we are not comparing dates there
    // because, when we compare, we check for the address of the char array, which is not correct, so we convert it to string here

    // now, read the csv files and only get the totalAmt column (column index 5)
    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        stringstream ss(line);
        string transactionDate, cashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, cashSalesAmount, ','); // get the amount (including tax)
        getline(ss, token, ','); // skip the change
        getline(ss, transactionDate, ','); // get the date
        
        if(transactionDate == currentDate){ // compare the transaction date with the current date
            totalCashSales += stod(cashSalesAmount); // convert to integer and add to total cash sales
            cashTransactions++;
        }
    }

    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        stringstream ss(line);
        string transactionDate, gcashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, gcashSalesAmount, ','); // get the amount (including tax)
        // the values will all be replaced, and when it comes to the index five, it will get the amount and save to the variable
        getline(ss, token, ','); // skip the change
        getline(ss, token, ','); // skip the refId
        getline(ss, transactionDate, ','); // get the date

        if(transactionDate == currentDate){ // compare the transaction date with the current date
            totalGcashSales += stod(gcashSalesAmount); // convert to integer and add to total gcash sales
            gcashTransactions++;
        }
    }
    cashFile.close();
    gcashFile.close();

    int totalSales = totalCashSales + totalGcashSales;
    double averageDailyCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0; // if cashTransaction is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageDailyGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    cout << "Sales from Cash: P" << setprecision(2) << fixed << totalCashSales << endl;
    cout << "Sales from GCash: P" << setprecision(2) << fixed << totalGcashSales << "\n\n";

    cout << "Average Cash Sale: P" << setprecision(2) << fixed << averageDailyCashSales << endl;
    cout << "Average GCash Sale: P" << setprecision(2) << fixed << averageDailyGcashSales << "\n\n";

    cout << "Total sales today: P" << setprecision(2) << fixed << totalSales << endl;
    cout << "---------------------------------\n";
}

void POSAdmin::getMonthlySales(){
    // location of the gcash and cash transactions
    string cashDatabase = "database/transactions/cash_cashierTransactions.csv";
    string gcashDatabase = "database/transactions/gcash_cashierTransactions.csv";

    // read sales from both databases
    ifstream cashFile(cashDatabase);
    ifstream gcashFile(gcashDatabase);

    if(!cashFile.is_open() || !gcashFile.is_open()){ // check if both files are opened successfully
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    // get the current date
    time_t timestamp = time(NULL); // set the time as null so that it will return the current timestamp
    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *, and we used ampersand inside the argument to get the address pointer of the timestamp variable, as it is expecting an address
    char monthChar[10], yearChar[10]; // since it points to a char array
    strftime(monthChar, 10, "%m", &datetime); // format the date as mm
    strftime(yearChar, 10, "%y", &datetime); // format the date as yy
    string currentMonth(monthChar); // we need to convert it from char array to string for easier comparison later
    string currentYear(yearChar); // we need to convert it from char array to string for easier comparison later

    // if you would notice, in the POSCashier, we used the same format for the date when saving transactions
    // we do not have to do this conversion in POSCashier.cpp because we are not comparing dates there
    // because when we compare, we check for the address of the char array, which is not correct, so we convert it to string here

    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        stringstream ss(line);
        string transactionDate, cashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, cashSalesAmount, ','); // get the amount (including tax)
        getline(ss, token, ','); // skip the change
        getline(ss, transactionDate, ','); // get the date
        
        string transactionMonth = transactionDate.substr(0,2); // get the first two characters of the transaction date (which is the month)
        string transactionYear = transactionDate.substr(6,2); // get the last two characters of the transaction date (which is the year)
        
        if(currentMonth == transactionMonth && currentYear == transactionYear){
            totalCashSales += stod(cashSalesAmount); // convert to integer and add to total cash sales
            cashTransactions++;
        }
    }

    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        stringstream ss(line);
        string transactionDate, gcashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, gcashSalesAmount, ','); // get the amount (including tax)
        // the values will all be replaced, and when it comes to the index five, it will get the amount and save to the variable
        getline(ss, token, ','); // skip the change
        getline(ss, token, ','); // skip the refId
        getline(ss, transactionDate, ','); // get the date

        string transactionMonth = transactionDate.substr(0,2); // get the first two characters of the transaction date (which is the month)
        string transactionYear = transactionDate.substr(6,2); // get the last two characters of the transaction date (which is the year)

        if(currentMonth == transactionMonth && currentYear == transactionYear){
            totalGcashSales += stod(gcashSalesAmount); // convert to integer and add to total gcash sales
            gcashTransactions++;
        }
    }
    cashFile.close();
    gcashFile.close();

    int totalSales = totalCashSales + totalGcashSales;
    double averageMonthlyCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0; // if cashTransaction is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageMonthlyGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    cout << "Sales from Cash: P" << setprecision(2) << fixed << totalCashSales << endl;
    cout << "Sales from GCash: P" << setprecision(2) << fixed << totalGcashSales << "\n\n";

    cout << "Average Cash Sale: P" << setprecision(2) << fixed << averageMonthlyCashSales << endl;
    cout << "Average GCash Sale: P" << setprecision(2) << fixed << averageMonthlyGcashSales << "\n\n";

    cout << "Total sales this month: P" << setprecision(2) << fixed << totalSales << endl;
    cout << "---------------------------------\n";
}

void POSAdmin::getYearlySales(){
    // location of the gcash and cash transactions
    string cashDatabase = "database/transactions/cash_cashierTransactions.csv";
    string gcashDatabase = "database/transactions/gcash_cashierTransactions.csv";

    // read sales from both databases
    ifstream cashFile(cashDatabase);
    ifstream gcashFile(gcashDatabase);

    if(!cashFile.is_open() || !gcashFile.is_open()){ // check if both files are opened successfully
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    // get the current date
    time_t timestamp = time(NULL); // set the time as null so that it will return the current timestamp
    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *, and we used ampersand inside the argument to get the address pointer of the timestamp variable, as it is expecting an address
    char monthChar[10], yearChar[10]; // since it points to a char array
    strftime(yearChar, 10, "%y", &datetime); // format the date as yy
    string currentYear(yearChar); // we need to convert it from char array to string for easier comparison later

    // if you would notice, in the POSCashier, we used the same format for the date when saving transactions
    // we do not have to do this conversion in POSCashier.cpp because we are not comparing dates there
    // because when we compare, we check for the address of the char array, which is not correct, so we convert it to string here

    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        stringstream ss(line);
        string transactionDate, cashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, cashSalesAmount, ','); // get the amount (including tax)
        getline(ss, token, ','); // skip the change
        getline(ss, transactionDate, ','); // get the date
        
        string transactionYear = transactionDate.substr(6,2); // get the last two characters of the transaction date (which is the year)

        if(transactionYear == currentYear){
            totalCashSales += stod(cashSalesAmount); // convert to integer and add to total cash sales
            cashTransactions++;
        }
    }

    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        stringstream ss(line);
        string transactionDate, gcashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, gcashSalesAmount, ','); // get the amount (including tax)
        // the values will all be replaced, and when it comes to the index five, it will get the amount and save to the variable
        getline(ss, token, ','); // skip the change
        getline(ss, token, ','); // skip the refId
        getline(ss, transactionDate, ','); // get the date

        string transactionYear = transactionDate.substr(6,2); // get the last two characters of the transaction date (which is the year)

        if(transactionYear == currentYear){
            totalGcashSales += stod(gcashSalesAmount); // convert to integer and add to total gcash sales
            gcashTransactions++;
        }
    }
    cashFile.close();
    gcashFile.close();

    int totalSales = totalCashSales + totalGcashSales;
    double averageYearlyCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0; // if cashTransaction is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageYearlyGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    cout << "Sales from Cash: P" << setprecision(2) << fixed << totalCashSales << endl;
    cout << "Sales from GCash: P" << setprecision(2) << fixed << totalGcashSales << "\n\n";

    cout << "Average Cash Sale: P" << setprecision(2) << fixed << averageYearlyCashSales << endl;
    cout << "Average GCash Sale: P" << setprecision(2) << fixed << averageYearlyGcashSales << "\n\n";

    cout << "Total sales this year: P" << setprecision(2) << fixed << totalSales << endl;
    cout << "---------------------------------\n";
}

void POSAdmin::getTotalSales(){
    // location of the gcash and cash transactions
    string cashDatabase = "database/transactions/cash_cashierTransactions.csv";
    string gcashDatabase = "database/transactions/gcash_cashierTransactions.csv";

    // read sales from both databases
    ifstream cashFile(cashDatabase);
    ifstream gcashFile(gcashDatabase);

    if(!cashFile.is_open() || !gcashFile.is_open()){
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    // get the current date
    time_t timestamp = time(NULL); // set the time as null so that it will return the current timestamp
    struct tm datetime = *localtime(&timestamp); // pointer localtime returns a pointer to struct tm, so we dereference it using *, and we used ampersand inside the argument to get the address pointer of the timestamp variable, as it is expecting an address
    char date[50]; // since it points to a char array
    strftime(date, 50, "%m_%d_%y", &datetime); // format the date as mm_dd_yy. 50 means the maximum length (size) of the char array
    string currentDate(date); // we need to convert it from char array to string for easier comparison later
    // if you would notice, in the POSCashier, we used the same format for the date when saving transactions
    // we do not have to do this conversion in POSCashier.cpp because we are not comparing dates there
    // because, when we compare, we check for the address of the char array, which is not correct, so we convert it to string here

    // now, read the csv files and only get the totalAmt column (column index 5)
    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        stringstream ss(line);
        string transactionDate, cashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, cashSalesAmount, ','); // get the amount (including tax)
                
        totalCashSales += stod(cashSalesAmount); // convert to integer and add to total cash sales
        cashTransactions++;
    }

    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        stringstream ss(line);
        string transactionDate, gcashSalesAmount, token;
        // skip the first five tokens
        getline(ss, token, ','); // skip id
        getline(ss, token, ','); // skip product names
        getline(ss, token, ','); // skip product quantities
        getline(ss, token, ','); // skip product amt (not including tax)
        getline(ss, token, ','); // skip the tax amount
        getline(ss, gcashSalesAmount, ','); // get the amount (including tax)

        totalGcashSales += stod(gcashSalesAmount); // convert to integer and add to total gcash sales
        gcashTransactions++;
    }
    cashFile.close();
    gcashFile.close();

    int totalSales = totalCashSales + totalGcashSales;
    double averageTotalCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0; // if cashTransaction is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageTotalGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    cout << "Total Sales from Cash: P" << setprecision(2) << fixed << totalCashSales << endl;
    cout << "Total Sales from GCash: P" << setprecision(2) << fixed << totalGcashSales << "\n\n";

    cout << "Average Sales from Cash: P" << setprecision(2) << fixed << averageTotalCashSales << endl;
    cout << "Average Sales from GCash: P" << setprecision(2) << fixed << averageTotalCashSales << "\n\n";

    cout << "Total Sales: P" << setprecision(2) << fixed << totalSales << endl;
    cout << "---------------------------------\n";
}

void POSAdmin::getAllLogs(string type){
    string database;
    // use different log files for different types
    if(type == "accounts"){
        database = "database/logs/adminUserLogs.csv";
    } else if(type == "products"){
        database = "database/logs/productsLogs.csv";
    } else if(type == "cashier_cash"){
        database = "database/transactions/cash_cashierTransactions.csv";
    } else if(type == "cashier_gcash"){
        database = "database/transactions/gcash_cashierTransactions.csv";
    } else {
        cout << "Invalid log type specified." << endl;
        Sleep(1200);
        return;
    }

    // Open the file as ifstream, so that we can read it
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
    for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    vector<size_t> widths(cols, 0); // using the max column size from the variable cols, use it to initialize widths with 0
    // if there are 6 columns in the csv, then cols = 6 and it will initialize widths to {0, 0, 0, 0, 0, 0}

    // after we get the maximum number, we will update the widths vector
    // this will make sure that each value will not overlap with each other

    for (auto &r : rows) { // for each row
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement

            // it does this for each value in the widths vector
    }

    // Add a little padding for readability
    for (auto &w : widths) w += 2;

    // Print
    for (auto &r : rows) {
        for (size_t c = 0; c < r.size(); ++c) {  // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }
    system("pause");
}

// UPDATE
void POSAdmin::updateProduct(string database, string type, string username){
    string query, newValue, reason;

    ifstream readFile(database); // open the file as an ifstream since we are reading from it
    if(!readFile.is_open()){
        cout << "Failed to open file\n";
        return;
    }

    cout << "What is the product name you want to update? (0 = Cancel): ";
    cin >> query;

    // check if product name exists in the csv
    if(!isAlreadyInCsv(database, query)){
        cout << "Product name '" << query << "' does not exist in the CSV.\n";
        Sleep(1200);
        return;
    }

    if (query == "0") return;
    if(regex_search(query, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Product name cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    cout << "Enter the new value (0 = Cancel): ";
    cin >> newValue;

    if(newValue == "0") return; // input from cin is always string

    // error handling for different fields in a product
    if(query == "productName" || query == "productSubCategory" || query == "productCategory"){ // only these two fields are strings
        if(regex_search(newValue, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
            cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
            Sleep(1200);
            return;
        } 
        if(query == "name" && isAlreadyInCsv(database, newValue)){ // check if the user wants to update a product name to another product name that already exists
            cout << "Product name '" << newValue << "' is already in the CSV.\n"; // since duplicate product names are not allowed
            Sleep(1200);
            return;
        }
    } else if(query == "price" || query == "quantity"){ // only these two fields are integers
        // make sure that the new input is a valid integer
        // note that the function handleInputError() will only work for INTEGERS entered via cin
        // and this function not only takes integers
        try {
            int value = stod(newValue); // convert string to integer
            if(value < 0){ // price and quantity cannot be negative
                cout << "The " + query + " cannot be negative.\n";
                Sleep(1200);
                return;
            }
        } catch (invalid_argument&) { // if the input cannot be converted to integer (or invalid input)
            cout << "Invalid input, please enter a valid integer for " + query + ".\n";
            Sleep(1200);
            return;
        } catch (out_of_range&) { // if the input is too large or too small for an integer
            cout << "The number entered is out of range for " + query + ".\n";
            Sleep(1200);
            return;
        }
    } 

    cout << "What is the reason for the update? (0 = Cancel): ";
    cin.ignore(); // ignore any inputs from the last cin (input)
    getline(cin, reason); // get the whole line including the spaces, because if we use cin, it will end at the space (which we don't want)
    if(reason == "0") return;
    if(regex_search(reason, disallowed)){
        cout << "Reason cannot contain commas or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    string fileContent, line;
    bool found = false; // to check if the query was found

    while (getline(readFile, line)) {
        stringstream ss(line); // create a string stream from the line
        string indexOne, indexTwo, indexThree, indexFour, indexFive, indexSix;
        // get a copy of each entries
        getline(ss, indexOne, ','); // read the id (not used) and save to variable indexOne
        getline(ss, indexTwo, ','); // read the second entry (username or product name) and save to variable indexTwo
        getline(ss, indexThree, ','); // read the product category or password and save to variable indexThree
        getline(ss, indexFour, ','); // read the product subcategory or role and save to variable indexFour
        getline(ss, indexFive, ','); // get the product quantity and save to variable indexFive
        getline(ss, indexSix, ','); // get the product price and save to variable indexSix

        // if-elseif has been used since switch case does not support strings in C++
        // modify the entry if it matches the query
        if (indexTwo == query) {
            // update the corresponding field
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
            found = true;
        }
        // write back to the file content
        // create a copy of each line, then, modify the index that the user wants to change
        fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "," + indexFive + "," + indexSix + "\n";
        // we cannot use the variable line here since it still holds the old value, and this function's purpose is to update the value
    }
    readFile.close(); // close the file

    if (!found) { // just a safety precaution, the above isAlreadyInCsv() should have caught this already
        return; // query not found, will exit the function
    } 

    ofstream fileOut(database); // open the file as an ofstream since we are writing to it
    if (!fileOut) { // if the file cannot be written
        cout << "Cannot write to file." << endl;
        Sleep(1200);
        return;
    }

    fileOut << fileContent; // write the updated content back to the file
    fileOut.close();

    // we will log EVERY action made by the manager or admin
    if(type == "productName"){ // if the user updated a product name
        saveLogs("products", "UPDATE", query + "_to_" + newValue, username, reason);
    } else {
        if(type == "productPrice"){ // if the user updated a product price
            saveLogs("products", "UPDATE", query + "_Price_to_" + newValue, username, reason);
        } else if(type == "productQuantity"){ // if the user updated a product quantity
            saveLogs("products", "UPDATE", query + "_Quantity_to_" + newValue, username, reason);
        } else if(type == "productSubCategory"){ // if the user updated a product sub-category
            saveLogs("products", "UPDATE", query + "_SubCategory_to_" + newValue, username, reason);
        } else if(type == "productCategory"){ // if the user updated a product category
            saveLogs("products", "UPDATE", query + "_Category_to_" + newValue, username, reason);
        } else { // for any other type of update
            saveLogs("products", "UPDATE", query + "_to_" + newValue, username, reason);
        }
    }
    cout << "Successfully updated product " << query << " with " << newValue << " as a new " << type << endl;

    Sleep(1200);
    return;
}

void POSAdmin::updateAccount(string database, string type, string username){
    string query, newValue, reason;
    ifstream readFile(database); // open the file as an ifstream since we are reading from it

    if(!readFile.is_open()){ // if file cannot be opened
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }

    cout << "What is the account username you want to update? (0 = Cancel): ";
    cin >> query;
    if(!isAlreadyInCsv(database, query)){ // check if account username exists in the csv
        cout << "Account username '" << query << "' does not exist in the CSV.\n";
        Sleep(1200);
        return;
    }

    cout << "What is the new value you want to set? ";
    cin >> newValue;
    
    if(regex_search(newValue, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    cout << "What is the reason for the update? ";
    cin.ignore(); // ignore previous user inputs
    getline(cin, reason); // get the whole line including spaces 
    if(regex_search(reason, disallowed)){
        cout << "Reason cannot contain commas or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    string fileContent, line;
    bool found = false;

    while (getline(readFile, line)) {
        stringstream ss(line); // use stringstream to read the line
        string indexOne, indexTwo, indexThree, indexFour, indexFive, indexSix;
        // get a copy of each entries
        getline(ss, indexOne, ','); // read the id (not used) and save to variable indexOne
        getline(ss, indexTwo, ','); // read the account username and save to variable indexTwo
        getline(ss, indexThree, ','); // read the account password and save to variable indexThree
        getline(ss, indexFour, ','); // read the account role and save to variable indexFour

        // if-elseif has been used since switch case does not support strings in C++
        // modify the entry if it matches the query
        if (indexTwo == query) {
            // update the corresponding field
            if(type == "accountUsername"){
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
        fileContent += indexOne + "," + indexTwo + "," + indexThree + "," + indexFour + "\n";
        // we cannot use the variable line here since it still holds the old value, and this function's purpose is to update the value
    }
    readFile.close(); // close the file

    if (!found) {
        return; // query not found, will exit the function
    }
    ofstream fileOut(database);
    if (!fileOut) {
        cout << "Cannot write to file " << database << endl;
        Sleep(1200);
        return;
    }

    fileOut << fileContent;
    fileOut.close();

    // we will log EVERY action made by the admin
    if(type == "accountPassword"){ // if the user updated a password
        saveLogs("accounts", "UPDATE", query + "_PW_to_" + newValue, username, reason);
    } else { // for any other type of update
        saveLogs("accounts", "UPDATE", query + "_to_" + newValue, username, reason);
    }
    cout << "Successfully updated account " << query << " with " << newValue << " as a new " << type << endl;
    
    Sleep(1200);
    return;
}

void POSAdmin::updateDiscounts(string username) {
    string category;
    ifstream discountFile("database/discounts.csv");
    if(!discountFile.is_open()){
        cout << "Failed to open discounts database.\n";
        return;
    }
    cout << "Enter the category (Tops/Bottoms/Accessories) to update discount (0 = Cancel): ";
    cin >> category;

    if(category == "0") return;
    if(regex_search(category, disallowed)){
        cout << "Category cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    } 
    if(!isAlreadyInCsv("database/discounts.csv", category)){
        cout << "Category '" << category << "' does not exist in the discounts database.\n";
        Sleep(1200);
        return;
    } 
    if(category != "Tops" && category != "Bottoms" && category != "Accessories"){
        cout << "Invalid category. Please enter Tops, Bottoms, or Accessories.\n";
        Sleep(1200);
        return;
    }

    cout << "Enter the new discount percentage (0-100, 0 = Cancel): ";
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
        } 
    
        fileContent += line + "\n"; // keep the line as is
    }
    discountFile.close(); // close the file

    // write back to the file
    ofstream discountFileOut("database/discounts.csv"); // open the file as ofstream since we are writing to it
    if(!discountFileOut.is_open()){ // if file cannot be opened
        cout << "Failed to open discounts database for writing.\n";
        return;
    }

    discountFileOut << fileContent; // write the updated content back to the file
    discountFileOut.close(); // close the file

    cout << "Successfully updated discount for " << category << " to " << discountPercentage << "%.\n";
    saveLogs("products", "UPDATE", category + "_Discount_to_" + to_string(discountPercentage), username, "Discount_Updated");
    Sleep(1200);
}

// DELETE
void POSAdmin::deleteInformation(string type, string filename, string username){
    string deleteProductInput;

    cout << "Enter the entry name you want to delete (0 = Cancel): ";
    cin >> deleteProductInput;

    if(regex_search(deleteProductInput, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
        cout << "Entry name cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    if (deleteProductInput == "0") return;

    ifstream input_file(filename); // open the file as an ifstream since we are reading from it
    if (!input_file.is_open()) {
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }

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
    }
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
    Sleep(1200);
}