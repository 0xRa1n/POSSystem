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
#include <vector> // for vector

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
    string productName, productSubCategory, productCategory;
    int quantity, userCategoryInput, userSubCategoryInput;
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
    cout << "1. Tops\n2. Bottoms\n3. Accessories\n4. Cancel\n";
    cout << "Enter the product category: ";
    cin >> userCategoryInput;

    if(handleInputError()) return; // handle invalid inputs
    if (userCategoryInput == 4) return;

    // if productCategory input from user is neither tops, bottoms, or accessories
    if(userCategoryInput != 1 && userCategoryInput != 2 && userCategoryInput != 3){
        cout << "Invalid category. Please enter from the list above.\n";
        Sleep(1200);
        return;
    } 

    switch(userCategoryInput){
        case 1: productCategory = "Tops"; break;
        case 2: productCategory = "Bottoms"; break;
        case 3: productCategory = "Accessories"; break;
        default: break; // this line will never be reached due to the previous validation
    }
    cout << "\n"; 

    // ask the user if what product sub-category is the product
    cout << "1. T-Shirts\n2. Polo Shirts\n3. Jackets\n4. Jeans\n5. Shorts\n6. Skirts\n7. Bags\n8. Headwear\n9. Wallets\n10. Cancel\n";
    cout << "Enter the product sub-category: "; // no specific sub-categories since they are many
    cin >> userSubCategoryInput;

    if(handleInputError()) return; // handle invalid inputs
    if(userSubCategoryInput == 10) return;

    switch(userSubCategoryInput){
        case 1: productSubCategory = "T_Shirts"; break;
        case 2: productSubCategory = "Polo_Shirts"; break;
        case 3: productSubCategory = "Jackets"; break;
        case 4: productSubCategory = "Jeans"; break;
        case 5: productSubCategory = "Shorts"; break;
        case 6: productSubCategory = "Skirts"; break;
        case 7: productSubCategory = "Bags"; break;
        case 8: productSubCategory = "Headwear"; break;
        case 9: productSubCategory = "Wallets"; break;
        default: break; // invalid input, so just return. additionally, invalid input is already handled above
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
    double priceInput;
    cout << "Enter the price (0 = Cancel): ";
    cin >> priceInput;

    if(handleInputError()) return; // handle invalid inputs
    if (priceInput == 0) return;
    if(priceInput < 0){ // price cannot be negative
        cout << "Price cannot be negative. Please enter a valid amount." << endl;
        Sleep(1200);
        return;
    }

    price = priceInput;

    // get the last product id from the database
    int newId = getLastId(database) +  1;

    // open the database file, and add the new product
    fstream fout; // create a file stream object with the variable fout, so that we can write to the file
    fout.open(database, ios::app); // this tells the program that the cursor should be placed at the end of the file, avoiding overwritting existing data

    fout << newId << ","
        << productCategory << ","
        << productSubCategory << ","
        << productName << ","
        << quantity << ","
        << fixed << setprecision(2) << price << "\n";
    fout.close(); // close the file

    saveLogs("products", "ADD", productName, username, "New_Product_Added"); // log the addition of product to productsLogs.csv
    cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
    Sleep(1200); // wait for 1.2 seconds before going back to the menu
}

void POSAdmin::addUser(string database, string accessingUsername){
    string username, password, role;
    int roleInput;
                            
    // ask the user for the username (if 0 is entered, it will go back to the menu)
    cout << "Enter the username you want to add (0 = Cancel): ";
    cin >> username;

    // if the user decides to change his mind
    if (username == "0") return;

    // use the function to check if the entry is already in the database || we use if and if not else if, because we want both conditions to be checked regardless (if it was elseif, the second condition would not be checked if the first condition was true)
    if(isAlreadyInCsv(database, username)){
        cout << "User is already in the database!";
        Sleep(1200);
        return;
    }
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

    cout << "What is the role of the user? (1 = Admin, 2 = Manager, 3 = Cashier, 0 = Cancel): ";
    cin >> roleInput;
    if(handleInputError()) return; // handle invalid inputs
    if (roleInput == 0) return;

    switch(roleInput){
        case 1: role = "Admin"; break;
        case 2: role = "Manager"; break;
        case 3: role = "Cashier"; break;
        default:
            cout << "Invalid role. Please enter Admin, Manager, or Cashier only.\n";
            Sleep(1200);
            return;
    }

    // get the last user id from the dtabase
    int newUserId = getLastId(database) + 1;

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

void POSAdmin::saveRefundLogs(int transactionID, string productNames, int productQuantities, double amountRefunded, string transactionPaymentMethod, string username){
    const string database = "database/logs/refundLogs.csv";
    ofstream fout(database, ios::app);
    if (!fout) {
        cout << "Error opening database for writing." << endl;
        return;
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

    // write the refund details to the file (according to the header format in the CSV file)
    fout << transactionID << ","
        << productNames << ","
        << productQuantities << ","
        << amountRefunded << ","
        << transactionPaymentMethod << ","
        << date << ","
        << time << ","
        << username << endl;

    fout.close(); // close the file
    // if not closed, it may lead to data loss or corruption if the program ends unexpectedly
}

// READ
void POSAdmin::getDailySales(){
    // location of the gcash and cash transactions
    string cashDatabase = "database/transactions/cash_cashierTransactions.csv";
    string gcashDatabase = "database/transactions/gcash_cashierTransactions.csv";

    // read sales from both databases
    ifstream cashFile(cashDatabase); // read the contents of the cash transactions file
    ifstream gcashFile(gcashDatabase); // read the contents of the gcash transactions file

    if(!cashFile.is_open() || !gcashFile.is_open()){ // check if one of the files cannot open
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    // get the current date
    time_t timestamp = time(NULL);
    struct tm datetime = *localtime(&timestamp); // get the value of the pointer returned by localtime
    char date[50];
    strftime(date, 50, "%m_%d_%y", &datetime); // format the date as mm_dd_yy
    string currentDate(date);

    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    // cash
    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        if (line.empty()) continue; // Skip empty lines

        stringstream ss(line); // helps to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        // the expected output of this are: {id, product names, product quantities, product amt (not including tax), tax amount, total amount (including tax), discount, money tendered, change, refunded amount, date}
        // in short, vector cols would be the representation of a row in the csv file, where each index corresponds to a column

        // Check if the row has enough columns to be valid
        // Cash CSV has about 12 columns. We need at least 11 (up to the date).
        if (cols.size() > 10) { // every row, get the date column
            string transactionDate = cols[10];
            if (transactionDate == currentDate) { // after the transaction date matches the current day, get the salesAmount and refundedAmount on that row
                double salesAmount = stod(cols[5]); // Total Amount is at index 5
                double refundedAmount = stod(cols[9]); // Refunded Amount is at index 9
                totalCashSales += salesAmount - refundedAmount; // calculate net sales after refund
                cashTransactions++; // we will need this to calculate the average cash sales
            }
        }
    } // this will run until the very last row (or line) of the csv file

    // gcash
    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        if (line.empty()) continue;

        stringstream ss(line); // helps to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }

        // GCash CSV has an extra 'refId' column. We need at least 12.
        if (cols.size() > 11) { // every row, get the date column
            string transactionDate = cols[11];
            if (transactionDate == currentDate) { // after the transaction date matches the current day, get the salesAmount and refundedAmount on that row
                double salesAmount = stod(cols[5]); // Total Amount is at index 5
                double refundedAmount = stod(cols[10]); // Refunded Amount is at index 10
                totalGcashSales += salesAmount - refundedAmount; // calculate net sales after refund
                gcashTransactions++; // we will need this to calculate the average gcash sales
            }
        }
    } // this will run until the very last row (or line) of the csv file
    cashFile.close();
    gcashFile.close();

    double totalSales = totalCashSales + totalGcashSales; // total sales from both payment methods
    double averageDailyCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0;  // if cashTransactions is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageDailyGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    // setprecision(2) is used to set the number of decimal places to 2, and fixed is used to ensure that the decimal places are always shown (even if they are zeros, and also avoid rounding up)
    cout << setprecision(2) << fixed << fixed;
    cout << "Sales from Cash: P" << totalCashSales << endl;
    cout << "Sales from GCash: P" << totalGcashSales << "\n\n";

    cout << "Average Cash Sale: P" << averageDailyCashSales << endl;
    cout << "Average GCash Sale: P" << averageDailyGcashSales << "\n\n";

    cout << "Total sales today: P" << totalSales << endl;
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
    time_t timestamp = time(NULL);
    struct tm datetime = *localtime(&timestamp); // get the value of the pointer returned by localtime
    char monthChar[10], yearChar[10]; 
    strftime(monthChar, 10, "%m", &datetime); // get current month in mm format
    strftime(yearChar, 10, "%y", &datetime); // get current year in yy format
    string currentMonth(monthChar); // get current month as string (for comparison purposes)
    string currentYear(yearChar); // get current year as string (for comparison purposes)

    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    // cash
    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        if (line.empty()) continue; // Skip empty lines
        
        stringstream ss(line); // used to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        
        // Check if the row has enough columns and a valid date
        if(cols.size() > 10 && cols[10].length() >= 8) { // every row, get the date column
            string transactionDate = cols[10]; // after getting the date, extract month and year
            string transactionMonth = transactionDate.substr(0,2);
            string transactionYear = transactionDate.substr(6,2);
            
            if(currentMonth == transactionMonth && currentYear == transactionYear){ // if both month and year matches current month and year, get salesAmount and refundedAmount
                double salesAmount = 0.0;
                double refundedAmount = 0.0;
                salesAmount = stod(cols[5]); // Total Amount
                refundedAmount = stod(cols[9]); // Refunded Amount
                
                totalCashSales += salesAmount - refundedAmount; // calculate net sales after refund
                cashTransactions++; // we will need this to calculate the average cash sales
            }
        }
    } // this will run until the very last row (or line) of the csv file

    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        if (line.empty()) continue; // Skip empty lines
        
        stringstream ss(line); // helps to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        
        // Check if the row has enough columns and a valid date
        if(cols.size() > 11 && cols[11].length() >= 8) {// every row, get the date column
            string transactionDate = cols[11]; // after getting the date, extract month and year
            string transactionMonth = transactionDate.substr(0,2);
            string transactionYear = transactionDate.substr(6,2);
            
            if(currentMonth == transactionMonth && currentYear == transactionYear){ // if both month and year matches current month and year, get salesAmount and refundedAmount
                double salesAmount = 0.0;
                double refundedAmount = 0.0;
                if (!cols[5].empty()) salesAmount = stod(cols[5]); // Total Amount
                if (!cols[10].empty()) refundedAmount = stod(cols[10]); // Refunded Amount
                
                totalGcashSales += salesAmount - refundedAmount; // calculate net sales after refund
                gcashTransactions++; // we will need this to calculate the average gcash sales
            }
        }
    }
    cashFile.close();
    gcashFile.close();

    double totalSales = totalCashSales + totalGcashSales; // total sales from both payment methods
    double averageMonthlyCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0; // if cashTransactions is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageMonthlyGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    // setprecision(2) is used to set the number of decimal places to 2, and fixed is used to ensure that the decimal places are always shown (even if they are zeros, and also avoid rounding up)
    cout << setprecision(2) << fixed;
    cout << "Sales from Cash: P" << totalCashSales << endl;
    cout << "Sales from GCash: P" << totalGcashSales << "\n\n";

    cout << "Average Cash Sale: P" << averageMonthlyCashSales << endl;
    cout << "Average GCash Sale: P" << averageMonthlyGcashSales << "\n\n";

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

    if(!cashFile.is_open() || !gcashFile.is_open()){ // check if one of the files cannot be opened
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    // get the current date
    time_t timestamp = time(NULL);
    struct tm datetime = *localtime(&timestamp); // get the value of the pointer returned by localtime
    char yearChar[10]; // to hold the year string
    strftime(yearChar, 10, "%y", &datetime); // get current year in yy format
    string currentYear(yearChar); // get current year as string (for comparison purposes)

    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        if (line.empty()) continue; // Skip empty lines
        
        stringstream ss(line); // used to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        
        // Check if the row has enough columns and a valid date
        if(cols.size() > 10 && cols[10].length() >= 8) { // every row, get the date column
            string transactionDate = cols[10]; // after getting the date, extract year
            string transactionYear = transactionDate.substr(6,2); // get the year part
            
            if(currentYear == transactionYear){
                double salesAmount = 0.0;
                double refundedAmount = 0.0;
                salesAmount = stod(cols[5]); // Total Amount
                refundedAmount = stod(cols[9]); // Refunded Amount
                
                totalCashSales += salesAmount - refundedAmount; // calculate net sales after refund
                cashTransactions++; //we will need this to calculate the average cash sales
            }
        }
    } // this will run until the very last row (or line) of the csv file

    //gcash
    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        if (line.empty()) continue; // Skip empty lines
        
        stringstream ss(line); // used to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        
        // Check if the row has enough columns and a valid date
        if(cols.size() > 11 && cols[11].length() >= 8) { // every row, get the date column
            string transactionDate = cols[11]; // after getting the date, extract year
            string transactionYear = transactionDate.substr(6,2);
            
            if(currentYear == transactionYear){ // if year matches current year, get salesAmount and refundedAmount
                double salesAmount = 0.0; 
                double refundedAmount = 0.0;
                salesAmount = stod(cols[5]); // Total Amount
                refundedAmount = stod(cols[10]); // Refunded Amount
                
                totalGcashSales += salesAmount - refundedAmount; // calculate net sales after refund
                gcashTransactions++; // we will need this to calculate the average gcash sales
            }
        }
    } // this will run until the very last row (or line) of the csv file
    cashFile.close();
    gcashFile.close();

    double totalSales = totalCashSales + totalGcashSales;
    double averageYearlyCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0;
    double averageYearlyGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0;

    // setprecision(2) is used to set the number of decimal places to 2, and fixed is used to ensure that the decimal places are always shown (even if they are zeros, and also avoid rounding up)
    cout << setprecision(2) << fixed;
    cout << "Sales from Cash: P" << totalCashSales << endl;
    cout << "Sales from GCash: P" << totalGcashSales << "\n\n";

    cout << "Average Cash Sale: P" << averageYearlyCashSales << endl;
    cout << "Average GCash Sale: P" << averageYearlyGcashSales << "\n\n";

    cout << "Total sales this year: P" << totalSales << endl;
    cout << "---------------------------------\n";
}

void POSAdmin::getTotalSales(){
    // location of the gcash and cash transactions
    string cashDatabase = "database/transactions/cash_cashierTransactions.csv";
    string gcashDatabase = "database/transactions/gcash_cashierTransactions.csv";

    // read sales from both databases
    ifstream cashFile(cashDatabase);
    ifstream gcashFile(gcashDatabase);

    if(!cashFile.is_open() || !gcashFile.is_open()){ // check if one of the files cannot be opened
        cout << "Failed to open one or both transaction files.\n";
        return;
    }

    string line;
    double totalCashSales = 0.0;
    double totalGcashSales = 0.0;
    int gcashTransactions = 0, cashTransactions = 0;

    //cash
    getline(cashFile, line); // skip the header
    while(getline(cashFile, line)){
        if (line.empty()) continue; // Skip empty lines
        
        stringstream ss(line); // used to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        
        // Check if the row has enough columns to be valid
        if(cols.size() > 9) { // Need at least 10 columns for sales and refund amounts
            double salesAmount = 0.0;
            double refundedAmount = 0.0;
            salesAmount = stod(cols[5]); // Total Amount is at index 5
            refundedAmount = stod(cols[9]); // Refunded Amount is at index 9
            
            totalCashSales += salesAmount - refundedAmount; // calculate net sales after refund
            cashTransactions++; // we will need this to calculate the average cash sales
        }
    } // this will run until the very last row (or line) of the csv file

    // gcash
    getline(gcashFile, line); // skip the header
    while(getline(gcashFile, line)){
        if (line.empty()) continue; // Skip empty lines
        
        stringstream ss(line); // used to split the line by comma
        vector<string> cols; // to store the value of each column
        string cell; // to temporarily hold each cell value
        while(getline(ss, cell, ',')) {
            cols.push_back(cell); // add each cell value to the cols vector
        }
        
        // Check if the row has enough columns to be valid
        if(cols.size() > 10) { // GCash has an extra refId, so we need at least 11 columns
            double salesAmount = 0.0;
            double refundedAmount = 0.0;
            salesAmount = stod(cols[5]); // Total Amount is at index 5
            refundedAmount = stod(cols[10]); // Refunded Amount is at index 10
            
            totalGcashSales += salesAmount - refundedAmount; // calculate net sales after refund
            gcashTransactions++; // we will need this to calculate the average gcash sales
        }
    }// this will run until the very last row (or line) of the csv file
    cashFile.close();
    gcashFile.close();

    double totalSales = totalCashSales + totalGcashSales; // total sales from both payment methods
    double averageTotalCashSales = cashTransactions > 0 ? totalCashSales / cashTransactions : 0; // if cashTransactions is greater than 0, divide totalCashSales by cashTransactions, else return 0
    double averageTotalGcashSales = gcashTransactions > 0 ? totalGcashSales / gcashTransactions : 0; // if gcashTransactions is greater than 0, divide totalGcashSales by gcashTransactions, else return 0

    // setprecision(2) is used to set the number of decimal places to 2, and fixed is used to ensure that the decimal places are always shown (even if they are zeros, and also avoid rounding up)
    cout << setprecision(2) << fixed;
    cout << "Total Sales from Cash: P" << totalCashSales << endl;
    cout << "Total Sales from GCash: P" << totalGcashSales << "\n\n";

    cout << "Average Sales from Cash: P" << averageTotalCashSales << endl;
    cout << "Average Sales from GCash: P" << averageTotalGcashSales << "\n\n";

    cout << "Total Sales: P" << totalSales << endl;
    cout << "---------------------------------\n";
}

void POSAdmin::readProductsByCategory(string productsDatabase, string category, string username){
    ifstream file(productsDatabase);
            
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }
    cout << "Format: ID, ProductName, Category, SubCategory, Quantity, Price\n" << endl;

    // Read and filter T-Shirts products
    vector<vector<string>> categoryRows;
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
        if (row.size() > 1 && row[1] == category) { // if row size is greater than 1 to avoid out of range error and if the category matches the desired category
            categoryRows.push_back(row); // add the row to the categoryRows
            // subCategoryRows will look like this: {{"ID", "ProductName", "SubCategory", "Quantity", "Price"}, {"1", "Product1", "T-Shirts", "10", "100"}, ...
            // the contents of the variable row will only be added if the sub-category matches
            // this way, we only have the products that belong to the desired sub-category
        }
    }
    file.close();

    if (categoryRows.empty()) {
        cout << "No products available in this category.\n";
        Sleep(1200);
        return;
    }

    // print all the products in the said subcategory
    // Find max width of each column
    size_t cols = 0;
    for (auto &r : categoryRows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement
    vector<size_t> widths(cols, 0); // initialize a vector with widths of 0
    // initially: widths is {0, 0, 0, 0, 0} for 5 columns
    // then the following code will update the widths for each row

    // after we get the maximum number, we will update the widths vector
    for (auto &r : categoryRows) { // using the reference, we can directly access the value
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement
    }

    // Add a little padding for readability
    for (auto &w : widths) w += 2;

    // Print
    for (auto &r : categoryRows) { // using the reference, we can directly access the value
        for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }

    string productName, productCategory;
    int productQuantity;
    double productPrice;

    // Get user input for product ID
    int selectedId;
    cout << "\nEnter product ID to select (0 = Cancel): ";
    cin >> selectedId;

    if (handleInputError()) return; // handle invalid inputs
    if (selectedId == 0) return;

    bool isFound = false;
    for (auto row : categoryRows) {
        if (stoi(row[0]) == selectedId) { // row[0] is the product ID
            productName = row[3];
            productCategory = row[1];
            productQuantity = stoi(row[4]);
            productPrice = stod(row[5]);

            isFound = true;
            break;
        }
    }

    if(!isFound){
        cout << "Product with ID " << selectedId << " not found in this category.\n";
        Sleep(1200);
        return;
    }

    cout << "1. Update Category\n2. Update Sub-Category\n3. Update Product Name\n4. Update Quantity\n5. Update Price\n6. Go Back\nEnter the field you want to edit: ";
    cin >> selectedId;
    cout << "\n";

    if(selectedId == 6) return;

    if (handleInputError()) return; // handle invalid inputs
    switch (selectedId) {
        case 1: {
            string newProductCategory; // for storing the new category
            int categoryOption; // for storing the input
            cout << "1. Tops\n2. Bottoms\n3. Accessories\nEnter the new category: ";
            cin >> categoryOption;

            if(handleInputError()) return; // handle invalid inputs

            switch(categoryOption){
                case 1: newProductCategory = "Tops"; break;
                case 2: newProductCategory = "Bottoms"; break;
                case 3: newProductCategory = "Accessories"; break;
                default: cout << "Invalid category option.\n"; Sleep(1200); break;
            }
            
            updateProduct(productsDatabase, productName, newProductCategory, "productCategory", username);
            break;
        }
        case 2: {
            string newProductSubCategory;
            int subCategoryOption;
            cout << "1. T-Shirts\n2. Polo Shirts\n3. Jackets\n4. Jeans\n5. Shorts\n6. Skirts\n7. Bags\n8. Headwear\n9. Wallets\nEnter the new sub-category: ";
            cin >> subCategoryOption;

            if(handleInputError()) return; // handle invalid inputs

            switch(subCategoryOption){
                case 1: newProductSubCategory = "T_Shirts"; break;
                case 2: newProductSubCategory = "Polo_Shirts"; break;
                case 3: newProductSubCategory = "Jackets"; break;
                case 4: newProductSubCategory = "Jeans"; break;
                case 5: newProductSubCategory = "Shorts"; break;
                case 6: newProductSubCategory = "Skirts"; break;
                case 7: newProductSubCategory = "Bags"; break;
                case 8: newProductSubCategory = "Headwear"; break;
                case 9: newProductSubCategory = "Wallets"; break;
                default: cout << "Invalid sub-category option.\n"; Sleep(1200); break;
            }

            updateProduct(productsDatabase, productName, newProductSubCategory, "productSubCategory", username);
            break;
        }
        case 3: {
            string newProductName;
            cout << "Enter new Product Name: ";
            cin >> newProductName;

            updateProduct(productsDatabase, productName, newProductName, "productName", username);
            break;
        }
        case 4: {
            int newProductQuantity;
            cout << "Enter new Quantity: ";
            cin >> newProductQuantity;
            
            updateProduct(productsDatabase, productName, to_string(newProductQuantity), "productQuantity", username);
            break;
        }
        case 5: {
            double newProductPrice;
            cout << "Enter new Price: ";
            cin >> newProductPrice;

            stringstream priceStream; // this will treat anything inserted into it as a string
            priceStream << fixed << setprecision(2) << newProductPrice; // format the price to 2 decimal places
            string formattedPrice = priceStream.str(); // get the formatted price as a string

            updateProduct(productsDatabase, productName, formattedPrice, "productPrice", username);
            break;
        }
        default:
            cout << "Invalid option selected.\n";
            Sleep(1200);
            return;
    }
};

void POSAdmin::readAccounts(string userDatabase, string operatorUsername){
    ifstream file(userDatabase);

    if (!file.is_open()) {
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }
    cout << "Format: ID, Username, Password, Role\n" << endl;

    // we reused the function from the utilities because we want to get something, not just read

    // Read and filter accounts
    vector<vector<string>> accountRows;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell); // add each cell to the row
            // row will look like this: {"ID", "ProductName", "SubCategory", "Quantity", "Price"
        }
        accountRows.push_back(row); // add the row to the categoryRows
    }
    file.close();

    if (accountRows.empty()) {
        cout << "No accounts found.\n";
        Sleep(1200);
        return;
    }

    // print all the products in the said subcategory
    // Find max width of each column
    size_t cols = 0;
    for (auto &r : accountRows) cols = max(cols, r.size()); // get the maximum number in each of the vector rows, so that the other parts will not overlap
    // no brackets since its a single controlled statement
    vector<size_t> widths(cols, 0); // initialize a vector with widths of 0
    // initially: widths is {0, 0, 0, 0, 0} for 5 columns
    // then the following code will update the widths for each row

    // after we get the maximum number, we will update the widths vector
    for (auto &r : accountRows) { // using the reference, we can directly access the value
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement
    }

    // Add a little padding for readability
    for (auto &w : widths) w += 2;

    // Print
    for (auto &r : accountRows) { // using the reference, we can directly access the value
        for (size_t c = 0; c < r.size(); ++c) { // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }

    string accountUsername, accountPassword, accountRole;

    // Get user input for product ID
    int selectedId;
    cout << "\nEnter account ID to select (0 = Cancel): ";
    cin >> selectedId;

    if (handleInputError()) return; // handle invalid inputs
    if (selectedId == 0) return;

    bool isFound = false;
    for (size_t i = 1; i < accountRows.size(); ++i) { // for loop is used so that it can skip the header row (we started at 1 to skip header)
        const auto& row = accountRows[i]; // this holds the value for each row (skipping the header)
        if (stoi(row[0]) == selectedId) { // row[0] is the account ID
            accountUsername = row[1]; // row[1] is the account username
            accountPassword = row[2]; // row[2] is the account password
            accountRole = row[3]; // row[3] is the account role
            isFound = true;
            break;
        }
    }
    if(!isFound){
        cout << "Account with ID " << selectedId << " not found in this category.\n";
        Sleep(1200);
        return;
    }

    cout << "1. Update Username\n2. Update Password\n3. Update Role\n4. Go Back\nEnter the field you want to edit: ";
    cin >> selectedId;
    cout << "\n";
    
    if (handleInputError()) return; // handle invalid inputs
    if(selectedId == 4) return;

    switch(selectedId) {
        case 1: {
            string newUsername;
            cout << "Enter new Username: ";
            cin >> ws;
            getline(cin, newUsername);
            if(regex_search(newUsername, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
                cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
                Sleep(1200);
                return;
            }

            updateAccount(userDatabase, accountUsername, newUsername, "accountUsername", operatorUsername);
            break;
        }
        case 2: {
            string newPassword;
            cout << "Enter new Password: ";
            cin >> ws;
            getline(cin, newPassword);
            if(regex_search(newPassword, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
                cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
                Sleep(1200);
                return;
            }

            updateAccount(userDatabase, accountUsername, newPassword, "accountPassword", operatorUsername);
            break;
        }
        case 3: {
            string newRole;
            int roleOption;
            cout << "1. Admin\n2. Cashier\nEnter the new role: ";

            cin >> ws; // used to get rid of any leading whitespace
            getline(cin, newRole);
            if(regex_search(newRole, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
                cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
                Sleep(1200);
                return;
            }

            switch(roleOption){
                case 1: newRole = "Admin"; break;
                case 2: newRole = "Cashier"; break;
                default: cout << "Invalid role option.\n"; Sleep(1200); break;
            }

            updateAccount(userDatabase, accountUsername, newRole, "accountRole", operatorUsername);
            break;
        }
        default:
            cout << "Invalid option selected.\n";
            Sleep(1200);
            return;
    }
};

int POSAdmin::getProductPriceByName(string productsDatabase, string productName){
    ifstream file(productsDatabase);
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        return 0;
    }
    string line;
    getline(file, line); // skip header
    while(getline(file, line)){
        stringstream ss(line);
        string id, category, subCategory, name, quantity, price;
        
        getline(ss, id, ',');
        getline(ss, category, ',');
        getline(ss, subCategory, ',');
        getline(ss, name, ',');
        getline(ss, quantity, ',');
        getline(ss, price, ',');

        if(name == productName){
            file.close();
            return stoi(price);
        }
    }
    file.close();
    return 0;
}

void POSAdmin::readReceipts(string database, int receiptIdChoiceInput, string paymentMethod){
    ifstream file(database);
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        return;
    }
    string line;
    bool isFound = false;
    getline(file, line); // skip header
    while(getline(file, line)){
        stringstream ss(line);
        string id, productNames, productQuantities, productAmt, taxAmt, totalAmt, discount, moneyTendered, changeAmt, refId, refundedAmt, date, time;
        
        getline(ss, id, ',');
        getline(ss, productNames, ',');
        getline(ss, productQuantities, ',');
        getline(ss, productAmt, ',');
        getline(ss, taxAmt, ',');
        getline(ss, totalAmt, ',');
        getline(ss, discount, ',');
        getline(ss, moneyTendered, ',');
        getline(ss, changeAmt, ',');
        if(paymentMethod == "gcash"){
            getline(ss, refId, ','); // only for gcash transactions
        }
        getline(ss, refundedAmt, ',');
        getline(ss, date, ',');
        getline(ss, time, ',');

        if(stoi(id) == receiptIdChoiceInput){
            isFound = true;
            system("cls");
            showHeader("P.O.S");

            cout << "Threads and Charms\n";
            cout << "Transaction Date: " << date << " " << time << "\n";
            cout << "Transaction #: " << id << "\n\n";

            cout << left << setw(35) << "Product Name" << setw(10) << "Qty" << "Price" << endl;
            cout << "--------------------------------------------------" << endl;

            // split the product names and quantities by '|', so that we can print them line by line// split the product names and quantities by '|', so that we can print them line by lin

            // stringstream helps us to treat a string like a stream, so that we can use getline to split it by a delimiter. stream refers to a sequence of data elements that are made available over time. In this case, we are treating the string as a stream of characters that we can read from. (in short, it helps us read the string easily)
            stringstream productNamesStream(productNames);
            stringstream productQuantitiesStream(productQuantities);
            string singleProductName, singleProductQuantity;

            while(getline(productNamesStream, singleProductName, '|') && getline(productQuantitiesStream, singleProductQuantity, '|')){ // iterate for each product and quantity one by one
                cout << left << setw(35) << singleProductName << setw(10) << singleProductQuantity; // print the product name and quantity (because we assigned the getline to store the quantity and product name to singleProductName and singleProductQuantity)
                // to find the exact amount for each product, we need to create a function that will look for the name, get its price, and multiply it by the quantity
                double productPrice = getProductPriceByName("database/products.csv", singleProductName);
                double totalPrice = productPrice * stoi(singleProductQuantity);
                cout << "P" << fixed << setprecision(2) << totalPrice / 1.12 << "\n"; // print the total price for that product
            }
            // this will run until there are no more products or quantities to read (no more pipe character '|')

            cout << "\nMoney tendered: " << "P" << moneyTendered << "\n";
            cout << "Discounts: " << "P" << discount << "\n";
            cout << "Total before VAT: " << "P" << fixed << setprecision(2) << (stod(totalAmt) - stod(taxAmt)) << "\n"; // total before VAT is totalAmt - taxAmt
            cout << "VAT (12%): " << "P" << taxAmt << "\n";
            cout << "Amount Due: " << "P" << totalAmt << "\n";
            cout << "Change: " << "P" << changeAmt << "\n";

            if(paymentMethod == "gcash"){
                cout << "Reference ID: " << refId << "\n";
                cout << "Payment Method: GCash\n";
            } else {
                cout << "Payment Method: Cash\n";
            }

            cout << "Refunded Amount: " << "P" << refundedAmt << "\n\n";
            break;
        }
    }

    if(!isFound){
        cout << "Receipt with ID " << receiptIdChoiceInput << " not found.\n";
        return;
    }

    file.close();
}

// UPDATE
void POSAdmin::updateProduct(string filename, string query, string valueToUpdate, string type, string username){
    string reason;

    ifstream readFile(filename); // open the file as an ifstream since we are reading from it
    if(!readFile.is_open()){
        cout << "Failed to open file\n";
        return;
    }

    cout << "What is the reason for the update? (0 = Cancel): ";
    cin >> ws; // eat up any leading whitespace
    getline(cin, reason); // get the whole line including the spaces, because if we use cin, it will end at the space (which we don't want)
    
    if(reason == "0") return;
    if(regex_search(reason, disallowed)){
        cout << "Reason cannot contain commas or any other special character besides: _ @ # &\n";
        Sleep(1200);
        return;
    }

    // error handling for different fields in a product
    if(type == "productName" || type == "productSubCategory" || type == "productCategory"){ // only these two fields are strings
        if(regex_search(valueToUpdate, disallowed)){ // validate the input for any invalid characters that may interfere with the program's structure
            cout << "Invalid input, it cannot contain spaces or commas, or any other special character besides: _ @ # &\n";
            Sleep(1200);
            return;
        } 
        if(type == "productName" && isAlreadyInCsv(filename, valueToUpdate)){ // check if the user wants to update a product name to another product name that already exists
            cout << "Product name '" << valueToUpdate << "' is already in the CSV.\n"; // since duplicate product names are not allowed
            Sleep(1200);
            return;
        }
    } else if(type == "productPrice" || type == "productQuantity"){ // only these two fields are integers
        try { // make sure that the new input is a valid integer. 
            int value = stod(valueToUpdate); // convert string to integer
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

    string fileContent, line;
    bool found = false; // to check if the query was found

    while (getline(readFile, line)) {
        stringstream ss(line); // create a string stream from the line
        string indexOne, indexTwo, indexThree, indexFour, indexFive, indexSix;
        // get a copy of each entries
        getline(ss, indexOne, ','); // read the product id
        getline(ss, indexTwo, ','); // read the product category
        getline(ss, indexThree, ','); // read the product subcategory
        getline(ss, indexFour, ','); // read the product name
        getline(ss, indexFive, ','); // read the product quantity
        getline(ss, indexSix, ','); // read the product price

        // if-elseif has been used since switch case does not support strings in C++
        // modify the entry if it matches the query
        if (indexFour == query) { // This will check each line if the product name matches the query (note that indexTwo holds the product name, and the variable query holds the product name to search for)
            if(type == "productName"){
                indexFour = valueToUpdate; // update product name
            } else if(type == "productQuantity"){
                indexFive = valueToUpdate; // update product quantity
            } else if(type == "productPrice"){
                indexSix = valueToUpdate; // update product price
            }  else if(type == "productSubCategory"){
                indexThree = valueToUpdate; // update product sub-category
            } else if(type == "productCategory"){
                indexTwo = valueToUpdate; // update product category
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

    ofstream fileOut(filename); // open the file as an ofstream since we are writing to it
    if (!fileOut) { // if the file cannot be written
        cout << "Cannot write to file." << endl;
        Sleep(1200);
        return;
    }

    fileOut << fileContent; // write the updated content back to the file
    fileOut.close();

    //we will log EVERY action made by the manager or admin
    if(type == "productName"){ // if the user updated a product name
        saveLogs("products", "UPDATE", query + "_to_" + valueToUpdate, username, reason);
    } else {
        if(type == "productPrice"){ // if the user updated a product price
            saveLogs("products", "UPDATE", query + "_Price_to_" + valueToUpdate, username, reason);
        } else if(type == "productQuantity"){ // if the user updated a product quantity
            saveLogs("products", "UPDATE", query + "_Quantity_to_" + valueToUpdate, username, reason);
        } else if(type == "productSubCategory"){ // if the user updated a product sub-category
            saveLogs("products", "UPDATE", query + "_SubCategory_to_" + valueToUpdate, username, reason);
        } else if(type == "productCategory"){ // if the user updated a product category
            saveLogs("products", "UPDATE", query + "_Category_to_" + valueToUpdate, username, reason);
        } else { // for any other type of update
            saveLogs("products", "UPDATE", query + "_to_" + valueToUpdate, username, reason);
        }
    }
    cout << "Successfully updated product " << query << " with " << valueToUpdate << " as a new " << type << "\n\n";

    Sleep(1200);
    return;
}

void POSAdmin::updateAccount(string database, string query, string valueToUpdate, string type, string username){
    string reason;
    ifstream readFile(database); // open the file as an ifstream since we are reading from it

    if(!readFile.is_open()){ // if file cannot be opened
        cout << "Failed to open file\n";
        Sleep(1200);
        return;
    }

    cout << "What is the reason for the update? ";
    cin >> ws;
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
                indexTwo = valueToUpdate; // update account username
            } else if(type == "accountPassword"){
                indexThree = valueToUpdate; // update account password
            } else if(type == "accountRole"){
                indexFour = valueToUpdate; // update account role
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
        saveLogs("accounts", "UPDATE", query + "_PW_to_" + valueToUpdate, username, reason);
    } else { // for any other type of update
        saveLogs("accounts", "UPDATE", query + "_to_" + valueToUpdate, username, reason);
    }
    cout << "Successfully updated account " << query << " with " << valueToUpdate << " as a new " << type << endl;
    
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

// ...existing code...
void POSAdmin::processRefunds(string username){
    int transactionPaymentMethodInput, transactionID;
    string transactionPaymentMethod, transactionDatabase, amountToRefundForLog;
    string productsRefundedForLog, productQuantitiesRefundedForLog;
    bool isFound = false;

    cout << "Which payment method was used for the transaction to refund? (1 = Cash, 2 = GCash, 0 = Cancel): ";
    cin >> transactionPaymentMethodInput;

    if(handleInputError() || transactionPaymentMethodInput == 0) return;

    if(transactionPaymentMethodInput == 1){
        transactionDatabase = "database/transactions/cash_cashierTransactions.csv";
        transactionPaymentMethod = "Cash";
    } else if(transactionPaymentMethodInput == 2){
        transactionDatabase = "database/transactions/gcash_cashierTransactions.csv";
        transactionPaymentMethod = "GCash";
    } else {
        cout << "Invalid payment method. Please enter 1 for Cash or 2 for GCash.\n";
        Sleep(1200);
        return;
    }

    cout << "Enter the transaction ID (0 = Cancel): ";
    cin >> transactionID;

    if(handleInputError() || transactionID == 0) return;

    string line;
    string fileContent;
    ifstream transactionFile(transactionDatabase); // open the file as an ifstream since we are reading from it
    if(!transactionFile.is_open()){ // if file cannot be opened
        cout << "Failed to open transaction file.\n";
        Sleep(1200);
        return;
    }

    // 1. Handle the header correctly
    getline(transactionFile, line); 
    fileContent += line + "\n"; // add header to fileContent

    // 2. Loop through the data rows
    while(getline(transactionFile, line)){
        stringstream ss(line);
        string id, productNames, productQuantities, productAmt, taxAmt, totalAmt, discount, moneyTendered, changeAmt, refundedAmt, refId, date, time, cashierName, status;

        // Parse all columns from the current row
        getline(ss, id, ',');
        getline(ss, productNames, ',');
        getline(ss, productQuantities, ',');
        getline(ss, productAmt, ',');
        getline(ss, taxAmt, ',');
        getline(ss, totalAmt, ',');
        getline(ss, discount, ',');
        getline(ss, moneyTendered, ',');
        getline(ss, changeAmt, ',');
        if(transactionPaymentMethod == "GCash"){
            getline(ss, refId, ',');
        }
        getline(ss, refundedAmt, ',');
        getline(ss, date, ',');
        getline(ss, time, ',');
        getline(ss, cashierName, ',');
        getline(ss, status);

        // 3. Check if this is the line to void
        if(!id.empty() && stoi(id) == transactionID){
            if(status == "Voided"){
                cout << "Transaction ID " << transactionID << " has already been voided.\n";
                Sleep(1200);
                transactionFile.close();
                return;
            } 
            isFound = true;
            status = "Voided"; // Update the status for this line
            
            refundedAmt = totalAmt; // now, the refundedAmt for that specific product ID will now become what the user had to pay

            // Capture the details from THIS line for logging later
            amountToRefundForLog = totalAmt; 
            productsRefundedForLog = productNames;
            productQuantitiesRefundedForLog = productQuantities;

            // (The inventory update logic remains the same)
            vector<vector<string>> products;
            stringstream prodNamesStream(productNames); // split product names by '|'
            stringstream prodQuantitiesStream(productQuantities); // split product quantities by '|'
            string prodName, prodQuantity;
            while(getline(prodNamesStream, prodName, '|') && getline(prodQuantitiesStream, prodQuantity, '|')){ // iterate for each product and quantity one by one
                products.push_back({prodName, prodQuantity}); // if there are two products (e.g. Prod1|Prod2) and their quantities (e.g. 2|3), then products will hold {{"Prod1", "2"}, {"Prod2", "3"}} 
            }
            
            string productsFileContent;
            ifstream productsFile("database/products.csv"); // open the products database to update quantities
            if(!productsFile.is_open()){ // if file cannot be opened
                cout << "Failed to open products database for updating quantities.\n";
                Sleep(1200);
                return;
            }

            string productsLine;
            while(getline(productsFile, productsLine)){ // read each line from products database
                stringstream prodSS(productsLine); // create a string stream from the line
                string prodId, prodCategory, prodSubCategory, prodNameInDB, prodQuantityInDB, prodPrice;
                // parse product details
                getline(prodSS, prodId, ',');
                getline(prodSS, prodCategory, ',');
                getline(prodSS, prodSubCategory, ',');
                getline(prodSS, prodNameInDB, ',');
                getline(prodSS, prodQuantityInDB, ',');
                getline(prodSS, prodPrice);

                for(const auto& prod : products){ // for each product in the refunded transaction
                    if(prodNameInDB == prod[0]){ // if product names match (index zero means the product name, and since it is a vector inside a vector, we can use the index 0 if there are many products)
                        // Update the quantity in the database
                        int updatedQuantity = stoi(prodQuantityInDB) + stoi(prod[1]); // get the quantity from the db and the quantity bought
                        prodQuantityInDB = to_string(updatedQuantity); // convert it into string
                        break;
                    }
                }
                // Rebuild the product line
                productsFileContent += prodId + "," + prodCategory + "," + prodSubCategory + "," + prodNameInDB + "," + prodQuantityInDB + "," + prodPrice + "\n";
            }
            productsFile.close();

            ofstream productsFileOut("database/products.csv");
            productsFileOut << productsFileContent;
            productsFileOut.close();
        }

        // 4. Rebuild the line (either original or modified) and add to fileContent
        fileContent += id + "," + productNames + "," + productQuantities + "," + productAmt + "," + taxAmt + "," + totalAmt + "," + discount + "," + moneyTendered + "," + changeAmt + ",";
        if(transactionPaymentMethod == "GCash"){
            fileContent += refId + ",";
        }
        fileContent += refundedAmt + "," + date + "," + time + "," + cashierName + "," + status + "\n";
    }
    transactionFile.close();

    if(!isFound){
        cout << "Transaction ID " << transactionID << " not found in the " << transactionPaymentMethod << " transactions.\n";
        Sleep(1200);
        return;
    }
    
    // 5. Write the complete, modified content back to the file
    ofstream transactionFileOut(transactionDatabase);
    transactionFileOut << fileContent;
    transactionFileOut.close();

    cout << "Transaction ID " << transactionID << " has been successfully voided. Amount to refund: P" << amountToRefundForLog << "\n\n";

    // 6. FIX: Correctly calculate total quantity for logging to prevent crash
    int totalQuantitiesForLog = 0;
    stringstream logQuants(productQuantitiesRefundedForLog);
    string logQuant;
    while(getline(logQuants, logQuant, '|')){
        if(!logQuant.empty()) totalQuantitiesForLog += stoi(logQuant);
    }

    saveRefundLogs(transactionID, productsRefundedForLog, totalQuantitiesForLog, stod(amountToRefundForLog), transactionPaymentMethod, username);
    system("pause");
    return;
}

// DELETE
void POSAdmin::deleteInformation(string type, string filename, string username){    
    string database;
    if(type == "products"){
        database = "database/products.csv";
    } else if(type == "accounts"){
        database = "database/userAccounts.csv";
    } else {
        cout << "Invalid type for deletion.\n";
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
        cout << "No records found in the database.\n";
        Sleep(1200);
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

    string productName;
    int selectedId;
    cout << "\nEnter the product ID you want to delete (0 = Cancel): ";
    cin >> selectedId;

    if(handleInputError()) return; // handle invalid inputs
    if(selectedId == 0) return;

    string idToDelete;
    bool isFound = false;
    for(int count = 1; count < rows.size(); ++count){
        if(rows[count].size() > 0 && stoi(rows[count][0]) == selectedId){ // check if the first column (ID) matches the selectedId
            isFound = true;
            idToDelete = rows[count][0]; // get the line to delete
            if(type == "products"){
                productName = rows[count][3]; // get the product name for logging
            }
            break;
        }
    }

    if (!isFound) {
        cout << "ID " << selectedId << " not found in the database.\n";
        Sleep(1200);
        return;
    }

    // Confirm deletion
    int deleteConfirmation;
    cout << "Are you sure you want to delete the entry with ID " << selectedId << "? (1 = Yes, 0 = No): ";
    cin >> deleteConfirmation;
    if(handleInputError()) return; // handle invalid inputs
    if(deleteConfirmation == 0){
        cout << "Deletion cancelled.\n";
        Sleep(1200);
        return;
    }

    // Rebuild file content without the deleted line
    string fileContent;

    // read the file and skip the product id to delete
    for (const auto& row : rows) {
        // Check if the current row's ID matches the one to delete
        if (row.empty() || row[0] == idToDelete) {
            continue; // Skip this row (either empty or the one to be deleted)
        }

        // Rebuild the line from the row's cells
        for (int count = 0; count < row.size(); ++count) {
            fileContent += row[count]; // get the value for each cell. output of this would be {123, "Tops", "Shirts", "Blue_Shirt", "50", "299"}. each value will be added one by one, with comma (except for the last one)
            if (count < row.size() - 1) {
                fileContent += ","; // Add a comma between cells
            }
        }
        fileContent += "\n"; // Add a newline at the end of the row
    }

    file.close();

    // Write back to the file
    ofstream fileOut(database);
    fileOut << fileContent;
    fileOut.close();

    cout << "Successfully deleted entry with ID " << selectedId << " from the database.\n";
    saveLogs(type, "DELETE", productName + "_ID_" + to_string(selectedId), username, "Deleted_Entry");
    Sleep(1200);
}