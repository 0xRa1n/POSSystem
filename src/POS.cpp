#include "./include/POS.h"
#include "./include/utilities.h" // For showHeader, etc.

// include all the packages that we only need IN THIS FILE
#include <fstream> // for functions ifstream, ofstream, and getline
#include <sstream> // for stringstream
#include <iostream> // for cout and cin

using namespace std;

// here are all member functions of PointOfSale class 
// member functions refers to functions defined inside a class
// the double colon (::) is the scope resolution operator, which tells us that the function belongs to the class PointOfSale

bool PointOfSale::login(string username, string password, string& outRole) { // the ampersand means that outRole is passed by reference (meaning that it can modify the original variable)
    const string userAccounts = "database/userAccounts.csv"; // path to the user accounts database, this is a read-only variable
    ifstream file(userAccounts); // open the file in read mode || file is a variable
    
    if (!file.is_open()) { // if the file could not be opened (due to missing file, permission error, etc.)
        cerr << "Could not open file " << userAccounts << endl;
        return false; // login failed
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) { // this reads each line of the file, until the very end (where it will return as false, since there are no more lines to read)
        stringstream readFromLine(line);
        // stringstream is a function that helps us read a line easily
        // and since we need to get the role of the user, we need to read all the columns
        // and by using stringstream, we can read each entry for each column piece by piece
        // and store it on a variable

        string file_id, file_username, file_password, file_role;

        getline(readFromLine, file_id, ','); // read until the first comma
        getline(readFromLine, file_username, ','); // read until the second comma, note that it ignores the previous comma and only gets the value after the second comma
        getline(readFromLine, file_password, ',');
        getline(readFromLine, file_role);

        if (file_username == username && file_password == password) {
            outRole = file_role; // set the outRole to the role found in the database (this will be used to determine the menu to show)
            file.close(); // close the file
            return true;
        }
    }

    file.close(); // close the file even if it's not found
    return false; // login failed
}

void PointOfSale::adminMenu(string username) {
    const string productsDatabase = "database/products.csv";
    const string usersDatabase = "database/userAccounts.csv";

    // initalize a loop, where it contains the menu Inventory, Monitoring, and Logout
    while (true) {
        system("cls"); // initial clear of the console
        string menu[] = {"Utilities", "Monitoring", "Logout"}; 
        int adminSelection = showMenu("P.O.S. (Admin)", menu); // this will print the header, and the numbers for selecting options
        system("cls"); // if there are error such as invalid input, it will reach this part to clear the screen again
        // this is same as the following code for the menus

        switch (adminSelection) {
            case 1: {  // Inventory submenu
                // intialize another loop that contains the menu of the admin menu
                while (true) {
                    system("cls"); // initial clear of the console
                    string inventoryMenu[] = {
                        "Add products",
                        "Add an account",
                        "View products, accounts, logs, or backup transaction data",
                        "Update account, product, or discounts",
                        "Delete a product",
                        "Delete an account",
                        "Go back"
                    };
                    int inventoryInput = showMenu("Admin utilities", inventoryMenu); // this will print the header, and the numbers for selecting options
                    system("cls"); // if an error arise, it will reach this part to clear the screen again

                    switch (inventoryInput) {
                        case 1: // add product
                            showHeader("Add a product"); // show header with the title Add a product
                            admin.addProduct(productsDatabase, username); // use the function inside the POSAdmin (we can use this since we are using it inside the PointOfSale class)
                            break; // break so that after the function, it would stop running the function

                        case 2: // add account
                            showHeader("Add an account"); // show header with the title Add an account
                            admin.addUser(usersDatabase, username); // use the function inside the POSAdmin
                            break;

                        case 3: {
                            while(true){ // every menu, it corresponds with a loop. so that if the user decides to go back, we can just end the loop to have him or her go back to the previous menu
                                system("cls"); // initial clearing of the console
                                string viewMenu[] = {"View all products", "View all accounts", "View all logs", "Backup transaction data (Cash)", "Backup transaction data (GCash)", "Go back"}; // automatically sets the options for the user to choose
                                int viewInput = showMenu("View", viewMenu); // show the menu and get the user's input
                                system("cls");

                                if(viewInput == 6) break; // exit view menu

                                switch(viewInput){
                                    case 1:
                                        showHeader("View all products");
                                        admin.readProducts(productsDatabase);
                                        system("pause"); // pause to let the user read the products
                                        break;
                                    case 2:
                                        showHeader("View all accounts");
                                        admin.getAllAccounts(usersDatabase);
                                        system("pause"); // pause to let the user read the accounts
                                        break;
                                    case 3:
                                        while(true){ // every menu, it corresponds with a loop. so that if the user decides to go back, we can just end the loop to have him or her go back to the previous menu
                                            system("cls"); // initial clear
                                            string logsMenu[] = {"Account logs", "Product logs", "Cashier transactions (Cash)", "Cashier transactions (GCash)", "Go back"};
                                            int logsInput = showMenu("View all logs", logsMenu);
                                            system("cls"); // clear again after getting input

                                            if (logsInput == 5) break; // exit logs menu

                                            switch(logsInput){
                                                case 1: // account logs
                                                    showHeader("Account logs");
                                                    admin.getAllLogs("accounts");
                                                    break;
                                                case 2: // product logs
                                                    showHeader("Product logs");
                                                    admin.getAllLogs("products");
                                                    break;
                                                case 3: // cashier logs
                                                    showHeader("Cashier transactions (Cash)");
                                                    admin.getAllLogs("cashier_cash");
                                                    break;
                                                case 4: // cashier logs
                                                    showHeader("Cashier transactions (GCash)");
                                                    admin.getAllLogs("cashier_gcash");
                                                    break;
                                                default:
                                                    cout << "Invalid selection";
                                                    Sleep(1200);
                                                    break;
                                            }
                                        }
                                        break;
                                    case 4: // this function is for those who had a transaction right after a  power outage or system crash. in that way, they can update immediately the database
                                        showHeader("Backup transaction data (Cash)");
                                        admin.readBackupTransactions("database/transactions/cash_backup.csv");
                                        system("pause"); // pause to let the user read the backup transactions
                                        break;
                                    case 5: // this function is for those who had a transaction right after a  power outage or system crash. in that way, they can update immediately the database
                                        showHeader("Backup transaction data (GCash)");
                                        admin.readBackupTransactions("database/transactions/gcash_backup.csv");
                                        system("pause"); // pause to let the user read the backup transactions
                                        break;
                                    default: // if the user entered an invalid input, the showMenu function will return -1. meaning that it was an invalid input.
                                        cout << "Invalid selection";
                                        Sleep(1200); // wait for 1.2s
                                        break; // then exit
                                }
                            }
                            break;
                        }
                        case 4: { // update
                            // initialize a loop where it contains the menu Product, Account, and Go back
                            while(true){
                                system("cls");
                                string updateMenu[] = {"Product", "Account", "Discounts", "Go back"};
                                int updateInput = showMenu("Update", updateMenu);
                                system("cls");

                                if(updateInput == 1){                  
                                    // initialize a loop to update the information of a product                          
                                    while(true){
                                        system("cls");
                                        string updateProductMenu[] = {"Product name", "Product Sub-Category", "Product Category", "Product quantity", "Product price", "Go back"};
                                        int updateProductInput = showMenu("Update product", updateProductMenu);
                                        system("cls");

                                        if(updateProductInput == 6) break; // this will end the loop, and will return to the previous loop

                                        switch(updateProductInput){
                                            case 1:  // update product name
                                                showHeader("Update product name");
                                                admin.updateProduct(productsDatabase, "productName", username);
                                                break;
                                            case 2:  // update product sub-category
                                                showHeader("Update product sub-category");
                                                admin.updateProduct(productsDatabase, "productSubCategory", username);
                                                break;
                                            case 3:  // update product category
                                                showHeader("Update product category");
                                                admin.updateProduct(productsDatabase, "productCategory", username);
                                                break;
                                            case 4:  // update product quantity
                                                showHeader("Update product quantity");
                                                admin.updateProduct(productsDatabase, "productQuantity", username);
                                                break;
                                            case 5:  // update product price
                                                showHeader("Update product price");
                                                admin.updateProduct(productsDatabase, "productPrice", username);
                                                break;
                                            default: // fallback
                                                cout << "Invalid selection";
                                                Sleep(1200);
                                                break;
                                        }
                                    }
                                } else if (updateInput == 2){                                        
                                    // initialize a loop to update the information of a product                          
                                    while(true){
                                        system("cls");
                                        string updateAccountMenu[] = {"Username", "Password", "Role", "Go back"};
                                        int updateAccountInput = showMenu("Update account", updateAccountMenu);
                                        system("cls");

                                        if(updateAccountInput == 4){
                                            // this will end the loop, and will return to the previous loop
                                            break;
                                        }

                                        switch(updateAccountInput){
                                            case 1:  // update username
                                                showHeader("Update account username");
                                                admin.updateAccount(usersDatabase, "accountUsername", username);

                                                break;
                                            case 2:  // update password
                                                showHeader("Update account password");
                                                admin.updateAccount(usersDatabase, "accountPassword", username);

                                                break;
                                            case 3:  // update role
                                                showHeader("Update account role");
                                                admin.updateAccount(usersDatabase, "accountRole", username);

                                                break;
                                            default:
                                                cout << "Invalid selection";
                                                Sleep(1200);
                                                break;
                                        }
                                    }
                                } else if(updateInput == 3){ // update discounts
                                    // initialize a loop to update the discounts
                                    showHeader("Update discounts");

                                    admin.updateDiscounts(username);
                                } else if(updateInput == 4){ // go back
                                    // this will go back to the admin menu
                                    break;
                                } else {
                                    cout << "Invalid selection";
                                    Sleep(1200);
                                }
                            }
                            break;
                        }
                        case 5: // delete product
                            showHeader("Delete product");
                            admin.deleteInformation("products", productsDatabase, username);

                            break;
                        case 6: // delete account
                            showHeader("Delete account");
                            admin.deleteInformation("accounts", usersDatabase, username);

                            break;
                        case 7:
                            // Go back to admin main menu
                            goto endInventoryLoop;
                        default:
                            cout << "Invalid selection.\n";
                            Sleep(1200);
                            break;
                    }
                }
                // this serves as an exit point for the second while loop
                endInventoryLoop: ; // it will execute this, but will do nothing. this is what is called a label
                break; // afterwards, it will break the first while loop's switch case
            }
            case 2: {
                while(true){ // loop for the monitoring menu, if the user decides to go back, it will end this loop
                    system("cls"); // initial clear
                    string monitoringMenu[] = {"View all products", "View sales", "Go back"};
                    int monitoringSelection = showMenu("Monitoring", monitoringMenu); // show the menu (automatically assign the option numbers) and get the user's input
                    system("cls"); // if the are errors such as invalid input, it will reach this part to clear the screen again

                    switch(monitoringSelection){
                        case 1: // view all products
                            showHeader("View all products");
                            admin.readProducts(productsDatabase);

                            system("pause");
                            break;
                        case 2:
                            showHeader("View sales");
                            cout << "Daily Sales:\n";
                            admin.getDailySales();

                            cout << "\nMonthly Sales:\n";
                            admin.getMonthlySales();

                            cout << "\nYearly Sales:\n";
                            admin.getYearlySales();

                            cout << "\nOverall Sales:\n";
                            admin.getTotalSales();

                            cout << "\n";
                            system("pause");
                            break;
                        case 3:
                            // go back to the previous menu
                            goto endMonitoringLoop; 
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                }
                endMonitoringLoop: ; // this is a label, it is still executed but nothing happens, after that, it will execute the break keyword
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid option\n";
                Sleep(1200);
                break;
        }
    }
}

void PointOfSale::managerMenu(string username) {
    // read only, since we don't need to modify these variables
    const string productsDatabase = "database/products.csv";
    const string usersDatabase = "database/userAccounts.csv";

    // initalize a loop, where it contains the menu Inventory, Monitoring, and Logout
    while (true) {
        system("cls"); // initial clear of the console
        string menu[] = {"Utilities", "Monitoring", "Logout"}; // initial loop (main menu for admin)
        int adminSelection = showMenu("P.O.S. (Manager)", menu); // this will print the header, and the numbers for selecting options. also, it will return the user's input
        system("cls"); // for errors such as invalid input, it will reach this part to clear the screen again

        switch (adminSelection) {
            case 1: {  // Inventory submenu
                // intialize another loop that contains the menu of the admin menu
                while (true) {
                    system("cls"); // initial clearing for this loop
                    string inventoryMenu[] = {
                        "Add products",
                        "View products, accounts, logs, or backup transaction data",
                        "Update product",
                        "Delete a product",
                        "Go back"
                    };
                    int inventoryInput = showMenu("Manager utilities", inventoryMenu);
                    system("cls"); // if an error arise, it will reach this part to clear the screen again

                    switch (inventoryInput) {
                        case 1: // add product
                            showHeader("Add a product");
                            admin.addProduct(productsDatabase, username);
                            break;
                        case 2: {
                            while(true){
                                system("cls");
                                string viewMenu[] = {"View all products", "View all accounts", "View all logs", "Backup transaction data (Cash)", "Backup transaction data (GCash)", "Go back"}; // automatically sets the options for the user to choose
                                int viewInput = showMenu("View", viewMenu); // automatically sets the options for the user to choose, and shows the menu. it also gets the user's input
                                system("cls");

                                if(viewInput == 5) break; // exit view menu

                                switch(viewInput){
                                    case 1: // view products
                                        showHeader("View all products");
                                        admin.readProducts(productsDatabase);
                                        system("pause"); // pause to let the user read the products
                                        break;
                                    case 2: // view accounts
                                        showHeader("View all accounts");
                                        admin.getAllAccounts(usersDatabase);
                                        system("pause"); // pause to let the user read the accounts
                                        break;
                                    case 3: // view logs (account, product, or cashier)
                                        while(true){ // every menu, it corresponds with a loop. so that if the user decides to go back, we can just end the loop to have him or her go back to the previous menu
                                            system("cls"); // initial clear
                                            string logsMenu[] = {"Account logs", "Product logs", "Cashier transactions (Cash)", "Cashier transactions (GCash)", "Go back"};
                                            int logsInput = showMenu("View all logs", logsMenu);
                                            system("cls"); // clear again after getting input

                                            if (logsInput == 5) break; // exit logs menu

                                            switch(logsInput){
                                                case 1: // account logs
                                                    showHeader("Account logs");
                                                    admin.getAllLogs("accounts");
                                                    break;
                                                case 2: // product logs
                                                    showHeader("Product logs");
                                                    admin.getAllLogs("products");
                                                    break;
                                                case 3: // cashier logs
                                                    showHeader("Cashier transactions (Cash)");
                                                    admin.getAllLogs("cashier_cash");
                                                    break;
                                                case 4: // cashier logs
                                                    showHeader("Cashier transactions (GCash)");
                                                    admin.getAllLogs("cashier_gcash");
                                                    break;
                                                default:
                                                    cout << "Invalid selection";
                                                    Sleep(1200);
                                                    break;
                                            }
                                        }
                                        break;
                                    case 4: // this function is for those who had a transaction right after a  power outage or system crash. in that way, they can update immediately the database
                                        showHeader("Backup transaction data (Cash)");
                                        admin.readBackupTransactions("database/transactions/cash_backup.csv");
                                        system("pause"); // pause to let the user read the backup transactions
                                        break;
                                    case 5: // this function is for those who had a transaction right after a  power outage or system crash. in that way, they can update immediately the database
                                        showHeader("Backup transaction data (GCash)");
                                        admin.readBackupTransactions("database/transactions/gcash_backup.csv");
                                        system("pause"); // pause to let the user read the backup transactions
                                        break;
                                    default:
                                        cout << "Invalid selection";
                                        Sleep(1200);
                                        break;
                                }
                            }
                            break;
                        }
                        case 3: { // update
                            // initialize a loop to update the information of a product                          
                            while(true){
                                string updateProductMenu[] = {"Product name", "Product sub-category", "Product category", "Product quantity", "Product price", "Go back"};
                                int updateProductInput = showMenu("Update product", updateProductMenu);

                                if(updateProductInput == 6) break; // go to the previous menu (ends this loop)

                                switch(updateProductInput){
                                    case 1:  // update product name
                                        showHeader("Update product name");
                                        admin.updateProduct(productsDatabase, "productName", username);
                                        break;
                                    case 2:  // update product sub-category
                                        showHeader("Update product sub-category");
                                        admin.updateProduct(productsDatabase, "productSubCategory", username);
                                        break;
                                    case 3:  // update product category
                                        showHeader("Update product category");
                                        admin.updateProduct(productsDatabase, "productCategory", username);
                                        break;
                                    case 4:  // update product quantity
                                        showHeader("Update product quantity");
                                        admin.updateProduct(productsDatabase, "productQuantity", username);
                                        break;
                                    case 5:  // update product price
                                        showHeader("Update product price");
                                        admin.updateProduct(productsDatabase, "productPrice", username);
                                        break;
                                    default: // fallback
                                        cout << "Invalid selection";
                                        Sleep(1200);
                                        break;
                                }
                            }
                            break;
                        }
                        case 4: // delete product
                            showHeader("Delete product");
                            admin.deleteInformation("products", productsDatabase, username);
                            break;
                        case 5:
                            // Go back to admin main menu
                            goto endInventoryLoop;
                        default:
                            cout << "Invalid selection.\n";
                            Sleep(1200);
                            break;
                    }
                }
                // this serves as an exit point for the second while loop
                endInventoryLoop: ;
                break;
            }
            case 2: {
                while(true){ // loop for the monitoring menu, if the user decides to go back, it will end this loop
                    system("cls");
                    string monitoringMenu[] = {"View all products", "View today's sales","Go back"};
                    int monitoringSelection = showMenu("Monitoring", monitoringMenu); // show the menu (automatically assign the option numbers) and get the user's input
                    system("cls");

                    switch(monitoringSelection){
                        case 1: // view all products
                            showHeader("View all products");
                            admin.readProducts(productsDatabase);

                            system("pause");
                            break;
                        case 2: // get daily, monthly, yearly, and total sales
                            showHeader("View sales");
                            cout << "Daily Sales:\n";
                            admin.getDailySales();

                            cout << "\nMonthly Sales:\n";
                            admin.getMonthlySales();

                            cout << "\nYearly Sales:\n";
                            admin.getYearlySales();

                            cout << "\nOverall Sales:\n";
                            admin.getTotalSales();

                            cout << "\n";
                            system("pause");
                            break;
                        case 3:
                            // go back to the previous menu
                            goto endMonitoringLoop; // this will jump to the endMonitoringLoop label
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                }
                endMonitoringLoop: ; // this is a label, it is still executed but nothing happens, after that, it will execute the break keyword
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid option\n";
                Sleep(1200);
                break;
        }
    }
}

void PointOfSale::cashierMenu(string username) {
    const string productsDatabase = "database/products.csv";
    while(true){
        system("cls"); // initial clearing, usually starts when logging in as cashier or when going back to this menu
        string menu[] = {"Tops", "Bottoms", "Accessories", "View Cart", "Logout"};
        int cashierSelection = showMenu("P.O.S. (Cashier)", menu);
        system("cls"); // if an error input was received, it will reach this part, so that it will print the error and go back to the menu

        switch (cashierSelection) {
            case 1: { // Tops
                bool transactionMade = false; // the purpose of this boolean here is to exit or end this sub-category loop and go back to the main menu if the transaction is complete (since we don't want to go to the sub-category menu again after a transaction is made)
                while(true){
                    system("cls");
                    string topsSubCategory[] = {"T-Shirts", "Polo Shirts", "Jackets", "Go back"};
                    int topsSelection = showMenu("Tops", topsSubCategory); // show the menu and get the user's input
                    system("cls");

                    if(topsSelection == 4) break; // Go back to the main cashier menu

                    switch(topsSelection){
                        case 1: // t-shirts
                            showHeader("T-Shirts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "T_Shirts", username); // this will return either true or false depending if a transaction was made
                            break;
                        case 2: // polo shirts
                            showHeader("Polo Shirts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Polo_Shirts", username); // this will return either true or false depending if a transaction was made
                            break;
                        case 3:
                            showHeader("Jackets");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Jackets", username); // this will return either true or false depending if a transaction was made
                            break;
                        default: // for invalid input
                            cout << "Invalid selection\n";
                            Sleep(1200);
                            break;
                    }
                    // If a transaction was completed, break out of the sub-category loop
                    // because if a transaction was made, we want to go back to the main cashier menu
                    // otherwise after the transaction, it will still be in the sub-category menu
                    if(transactionMade) break;
                }
                break; // end of case 1 (Tops), also, so that this case will stop and not go to the next case
            }
            case 2: { // Bottoms
                bool transactionMade = false; // the purpose of this boolean here is to exit or end this sub-category loop and go back to the main menu if the transaction is complete (since we don't want to go to the sub-category menu again after a transaction is made)
                while(true){
                    system("cls");
                    string bottomsSubCategory[] = {"Jeans", "Shorts", "Skirts", "Go back"};
                    int bottomsSelection = showMenu("Bottoms", bottomsSubCategory);
                    system("cls");

                    if(bottomsSelection == 4) break; // Go back to the main cashier menu

                    switch(bottomsSelection){
                        case 1: // jeans
                            showHeader("Jeans");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Jeans", username);
                            break;
                        case 2: // shorts
                            showHeader("Shorts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Shorts", username);
                            break;
                        case 3: // skirts
                            showHeader("Skirts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Skirts", username);
                            break;
                        default: // for invalid input
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                    // If a transaction was completed, break out of the sub-category loop
                    if(transactionMade) break;
                }
                break;
            }
            case 3: { // Accessories
                bool transactionMade = false; // the purpose of this boolean here is to exit or end this sub-category loop and go back to the main menu if the transaction is complete (since we don't want to go to the sub-category menu again after a transaction is made)
                while(true){
                    system("cls");
                    string accessoriesSubCategory[] = {"Bags", "Headware", "Wallets", "Go back"};
                    int accessoriesSelection = showMenu("Accessories", accessoriesSubCategory);
                    system("cls");
                    
                    if(accessoriesSelection == 4) break; // Go back to the main cashier menu

                    switch(accessoriesSelection){
                        case 1: // bags
                            showHeader("Bags");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Bags", username);
                            break;
                        case 2: // headware
                            showHeader("Headware");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Headware", username);
                            break;
                        case 3: // wallets
                            showHeader("Wallets");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Wallets", username);
                            break;
                        default: // for invalid input
                            cout << "Invalid selection\n";
                            Sleep(1200);
                            break;
                    }
                    // If a transaction was completed, break out of the sub-category loop
                    if(transactionMade) break;
                }
                break;
            }
            case 4: {
                bool transactionMade = cashier.viewCart(username); // this is boolean because if the user proceeds to checkout, we need to know in the main menu to not break the loop
                // and if the user cancels the transaction and he went with the view cart, it will just return to the main menu
                break;
            }
            case 5:
                return; // Logout
            default: // for invalid input
                cout << "Invalid option\n";
                Sleep(1000);
                break;
        }
    }
}
