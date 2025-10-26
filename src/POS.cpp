#include "./include/POS.h"
#include "./include/Utilities.h" // For showHeader, etc.

// include all the packages that we only need IN THIS FILE
#include <fstream> // for functions ifstream, ofstream, and getline
#include <sstream> // for stringstream
#include <iostream> // for cout and cin

using namespace std;

// Define all possible menu states for each user role
// this is what we call an enumeration (enum)
// it is used to define a group of constants (cannot be changed)
// enum class acts similarly to class. They can only be accessed through the scope resolution operator (::)
// e.g., AdminState::MainMenu
// so that there will be no conflict if there are other enums with the same names in other parts of the program
enum class AdminState { MainMenu, Inventory, Add, View, ViewLogs, Update, UpdateProduct, UpdateAccount, Delete, DeleteProduct, DeleteAccount, Monitoring, Exit };
enum class ManagerState { MainMenu, Inventory, View, ViewLogs, Update, UpdateProduct, Monitoring, Exit };
enum class CashierState { MainMenu, Tops, Bottoms, Accessories, ViewCart, Exit };

// switch accepts numbers such as int, short, long, char, and enum types
// enum, behind the scenes, are treated as integers

// here are all member functions of PointOfSale class 
// member functions refers to functions defined inside a class
// the double colon (::) is the scope resolution operator, which tells us that the function belongs to the class PointOfSale

bool PointOfSale::login(string username, string password, string& outRole) { // the ampersand means that outRole is passed by reference (meaning that it can modify the original variable)
    const string userAccounts = "database/userAccounts.csv"; // path to the user accounts database, this is a read-only variable
    ifstream file(userAccounts); // open the file in read mode || file is a variable
    
    if (!file.is_open()) { // if the file could not be opened (due to missing file, permission error, etc.)
        cout << "Could not open file " << userAccounts << endl;
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
    // path to the databases (const so that they cannot be changed)
    const string productsDatabase = "database/products.csv";
    const string usersDatabase = "database/userAccounts.csv";

    AdminState currentState = AdminState::MainMenu; // this is the current state (meaning that this can change where the user want to select) || we tell the compiler that the currentState is MainMenu that is inside of the enum class AdminState
    AdminState previousState = AdminState::MainMenu; // this will store the previous menu that the user was in (for going back) || we tell the compiler that the currentState is MainMenu that is inside of the enum class AdminState

    while (currentState != AdminState::Exit) { // this checks if the currentState is not Exit, if it is Exit, the loop will end
        // if the currentState is not AdminState::Exit, continue the loop
        system("cls");
        switch (currentState) {
            case AdminState::MainMenu: { // this will be the main menu for the admin
                string menu[] = {"Inventory", "Monitoring", "Logout"};
                int selection = showMenu("P.O.S. (Admin)", menu);
                switch (selection) {
                    case 1: 
                        currentState = AdminState::Inventory; // change the currentState to Inventory
                        break;
                    case 2: 
                        currentState = AdminState::Monitoring; // change the currentState to Monitoring
                        break;
                    case 3: 
                        currentState = AdminState::Exit; // change the currentState to Exit to logout
                        break;
                    default: 
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                } 
                break; // after the user had selected, it will break this case
            }
            case AdminState::Inventory: { // after the previous case had been broken, it will go here if the user selected Inventory (if the user exits the first case, it will only end that case and the loop will continue to the next where it will check the currentState again)
                string menu[] = {"Add Product or Account", "View", "Update Product or Account", "Delete Product or Account", "Go back"};
                int selection = showMenu("Admin Inventory", menu);
                previousState = AdminState::MainMenu; // we indicate that the previous state is MainMenu for going back, and if this case breaks, the loop will rerun and go to the AdminState::MainMenu case

                switch (selection) {
                    case 1: currentState = AdminState::Add; // call the addProduct function from POSAdmin class
                        break;
                    case 2: currentState = AdminState::View; // below this case, there will be a case named AdminState::View. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the AdminState::View case to execute
                        break;
                    case 3: currentState = AdminState::Update; // below this case, there will be a case named AdminState::Update. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the AdminState::UpdateProduct case to execute
                        break;
                    case 4: currentState = AdminState::Delete; // below this case, there will be a case named AdminState::Delete. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the AdminState::Delete case to execute
                        break;
                    case 5: currentState = previousState;  // go back to the previous state (which is MainMenu). Then, break this case. It will find for the case named AdminState::MainMenu to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case AdminState::Add: { // Add Product or Account
                string menu[] = {"Add Product", "Add Account", "Go back"};
                int selection = showMenu("Add Product or Account", menu);
                previousState = AdminState::Inventory; // set previous state to Inventory for going back

                switch (selection) {
                    case 1: showHeader("Add product"); admin.addProduct(productsDatabase, username); // call the addProduct function from POSAdmin class
                        break;
                    case 2: showHeader("Add account"); admin.addUser(usersDatabase, username); // call the addUser function from POSAdmin class
                        break;
                    case 3: currentState = previousState;  // go back to the previous state (which is Inventory). Then, break this case. It will find for the case named AdminState::Inventory to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case AdminState::View: {
                string menu[] = {"View all accounts", "View all logs", "Backup transaction data (Cash)", "Backup transaction data (GCash)", "Refund logs", "Go back"};
                int selection = showMenu("View", menu);
                previousState = AdminState::Inventory; // set previous state to Inventory for going back

                switch (selection) {
                    case 1: showHeader("View all accounts"); admin.getAllAccounts(usersDatabase); // call the getAllAccounts function from POSAdmin class 
                        system("pause"); 
                        break;
                    case 2: currentState = AdminState::ViewLogs; // below this case, there will be a case named AdminState::ViewLogs. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the AdminState::ViewLogs case to execute
                        break;
                    case 3: showHeader("Backup transaction data (Cash)"); admin.readBackupTransactions("database/transactions/cash_backup.csv"); // call the readBackupTransactions function from POSAdmin class
                        system("pause"); 
                        break;
                    case 4: showHeader("Backup transaction data (GCash)"); admin.readBackupTransactions("database/transactions/gcash_backup.csv"); // call the readBackupTransactions function from POSAdmin class
                        system("pause"); 
                        break;
                    case 5: showHeader("View refund logs"); admin.readRefundLogs(); // call the readRefundLogs function from POSAdmin class
                        system("pause"); 
                        break;
                    case 6: currentState = previousState;  // go back to the previous state (which is Inventory). Then, break this case. It will find for the case named AdminState::Inventory to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case AdminState::ViewLogs: {
                string menu[] = {"Account logs", "Product logs", "Cashier transactions (Cash)", "Cashier transactions (GCash)", "Go back"};
                int selection = showMenu("View All Logs", menu);
                previousState = AdminState::View; // set previous state to View for going back

                switch(selection) {
                    case 1: showHeader("Account logs"); admin.getAllLogs("accounts"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 2: showHeader("Product logs"); admin.getAllLogs("products"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 3: showHeader("Cashier transactions (Cash)"); admin.getAllLogs("cashier_cash"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 4: showHeader("Cashier transactions (GCash)"); admin.getAllLogs("cashier_gcash"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 5: currentState = previousState;  // go back to the previous state (which is View). Then, break this case. It will find for the case named AdminState::View to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case AdminState::Update: {
                string menu[] = {"Update Product", "Update Account", "Go back"};
                int selection = showMenu("Update Product or Account", menu);
                previousState = AdminState::Inventory; // set previous state to Inventory for going back

                switch(selection) {
                    case 1: currentState = AdminState::UpdateProduct;
                        break;
                    case 2: currentState = AdminState::UpdateAccount;
                        break;
                    case 3: currentState = previousState;
                        break;
                    default:
                        cout << "Invalid selection.\n";
                        Sleep(1200);
                        break;
                }
                break;
            }
            case AdminState::UpdateProduct: {
                showHeader("Update products");
                string category[] = {"Tops", "Bottoms", "Accessories", "Go back"};
                int selection = showMenu("Select category to update products", category);
                previousState = AdminState::Inventory; // set previous state to Inventory for going back

                switch(selection) {
                    case 1:
                        system("cls");
                        showHeader("Update Tops");
                        admin.readProductsByCategory(productsDatabase, "Tops", username);
                        system("pause");
                        break;
                    case 2:
                        system("cls");
                        showHeader("Update Bottoms");
                        admin.readProductsByCategory(productsDatabase, "Bottoms", username);
                        system("pause");
                        break;
                    case 3:
                        system("cls");
                        showHeader("Update Accessories");
                        admin.readProductsByCategory(productsDatabase, "Accessories", username);
                        system("pause");
                        break;
                    case 4:
                        currentState = previousState;  // go back to the previous state (which is Inventory). Then, break this case. It will find for the case named AdminState::Inventory to execute
                        break;
                    default:
                        cout << "Invalid selection.\n";
                        Sleep(1200);
                        break;
                }
                break;
            }
            case AdminState::UpdateAccount:
                showHeader("Update accounts");
                admin.readAccounts(usersDatabase, username); // call the readAccounts function from POSAdmin class

                system("pause");
                currentState = previousState;  // this will use the value from this code enum class AdminState:Update (which is Inventory) to go back. This is where this case came from, so it would not be changed.

                break;
            case AdminState::Delete: {
                string menu[] = {"Delete Product", "Delete Account", "Go back"};
                int selection = showMenu("Delete Product or Account", menu);
                previousState = AdminState::Inventory; // set previous state to Inventory for going back

                switch (selection) {
                    case 1: showHeader("Delete product"); admin.deleteInformation("products", productsDatabase, username); // call the deleteInformation function from POSAdmin class
                        break;
                    case 2: showHeader("Delete account"); admin.deleteInformation("accounts", usersDatabase, username); // call the deleteInformation function from POSAdmin class
                        break;
                    case 3: currentState = previousState;  // go back to the previous state (which is Inventory). Then, break this case. It will find for the case named AdminState::Inventory to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case AdminState::Monitoring: { // case for monitoring sales and products
                string menu[] = {"View all products", "View sales", "Go back"};
                int selection = showMenu("Monitoring", menu);
                previousState = AdminState::MainMenu;

                switch(selection){
                    case 1: showHeader("View all products"); admin.readProducts(productsDatabase);  // call the readProducts function from POSAdmin class
                        system("pause"); 
                        break;
                    case 2: // view sales
                        showHeader("View sales");
                        cout << "Daily Sales:\n"; admin.getDailySales();
                        cout << "\nMonthly Sales:\n"; admin.getMonthlySales();
                        cout << "\nYearly Sales:\n"; admin.getYearlySales();
                        cout << "\nOverall Sales:\n"; admin.getTotalSales();
                        cout << "\n"; system("pause");
                        break;
                    case 3: currentState = previousState; // go back to the previous state (which is MainMenu). Then, break this case. It will find for the case named AdminState::MainMenu to execute
                        break;
                    default: 
                        cout << "Invalid option\n"; // for invalid input
                        Sleep(1200); 
                        break;
                }
                break;
            }
        }
    }
}

void PointOfSale::managerMenu(string username) {
    // path to the databases (const so that they cannot be changed)
    const string productsDatabase = "database/products.csv";
    const string usersDatabase = "database/userAccounts.csv";

    ManagerState currentState = ManagerState::MainMenu; // this is the current state (meaning that this can change where the user want to select)
    ManagerState previousState = ManagerState::MainMenu; // this will store the previous menu that the user was in (for going back)

    while (currentState != ManagerState::Exit) {
        system("cls");
        switch (currentState) {
            case ManagerState::MainMenu: {
                string menu[] = {"Inventory", "Monitoring", "Logout"};
                int selection = showMenu("P.O.S. (Manager)", menu);

                switch (selection) {
                    case 1: currentState = ManagerState::Inventory; // below this case, there will be a case named ManagerState::Inventory. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the ManagerState::Inventory case to execute
                        break;
                    case 2: currentState = ManagerState::Monitoring; // below this case, there will be a case named ManagerState::Monitoring. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the ManagerState::Monitoring case to execute
                        break;
                    case 3: currentState = ManagerState::Exit; // change the currentState to Exit to logout
                        break;
                    default: 
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case ManagerState::Inventory: { // case for manager Inventory
                string menu[] = {"Add products", "View...", "Update...", "Delete a product", "Go back"};
                int selection = showMenu("Manager Inventory", menu);
                previousState = ManagerState::MainMenu;

                switch (selection) {
                    case 1: showHeader("Add a product"); admin.addProduct(productsDatabase, username); // call the addProduct function from POSAdmin class
                        break;
                    case 2: currentState = ManagerState::View; // below this case, there will be a case named ManagerState::View. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the ManagerState::View case to execute
                        break;
                    case 3: currentState = ManagerState::Update; // below this case, there will be a case named ManagerState::Update. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the ManagerState::Update case to execute
                        break;
                    case 4: showHeader("Delete product"); admin.deleteInformation("products", productsDatabase, username); // call the deleteInformation function from POSAdmin class
                        break;
                    case 5: currentState = previousState; // go back to the previous state (which is MainMenu). Then, break this case. It will find for the case named ManagerState::MainMenu to execute
                        break;
                    default: 
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case ManagerState::View: {
                string menu[] = {"View all products", "View all accounts", "View all logs", "Backup transaction data (Cash)", "Backup transaction data (GCash)", "Refund logs", "Go back"};
                int selection = showMenu("View", menu);
                previousState = ManagerState::Inventory; // set previous state to Inventory for going back

                switch (selection) {
                    case 1: showHeader("View all products"); admin.readProducts(productsDatabase); // call the readProducts function from POSAdmin class
                        system("pause"); 
                        break;
                    case 2: showHeader("View all accounts"); admin.getAllAccounts(usersDatabase); // call the getAllAccounts function from POSAdmin class
                        system("pause"); 
                        break;
                    case 3: currentState = ManagerState::ViewLogs; // below this case, there will be a case named ManagerState::ViewLogs. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the ManagerState::ViewLogs case to execute
                        break;
                    case 4: showHeader("Backup transaction data (Cash)"); admin.readBackupTransactions("database/transactions/cash_backup.csv"); // call the readBackupTransactions function from POSAdmin class
                        system("pause");
                        break;
                    case 5: showHeader("Backup transaction data (GCash)"); admin.readBackupTransactions("database/transactions/gcash_backup.csv"); // call the readBackupTransactions function from POSAdmin class
                        system("pause");
                        break;
                    case 6: showHeader("View refund logs"); admin.readRefundLogs(); // call the readRefundLogs function from POSAdmin class
                        system("pause");
                        break;
                    case 7: currentState = previousState; break;
                    default: 
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case ManagerState::ViewLogs: {
                string menu[] = {"Account logs", "Product logs", "Cashier transactions (Cash)", "Cashier transactions (GCash)", "Go back"};
                int selection = showMenu("View All Logs", menu);
                previousState = ManagerState::View; // set previous state to View for going back

                switch(selection) {
                    case 1: showHeader("Account logs"); admin.getAllLogs("accounts"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 2: showHeader("Product logs"); admin.getAllLogs("products"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 3: showHeader("Cashier transactions (Cash)"); admin.getAllLogs("cashier_cash"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 4: showHeader("Cashier transactions (GCash)"); admin.getAllLogs("cashier_gcash"); // call the getAllLogs function from POSAdmin class
                        break;
                    case 5: currentState = previousState; // go back to the previous state (which is View). Then, break this case. It will find for the case named ManagerState::View to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case ManagerState::Update: {
                string menu[] = {"Product", "Process Refunds", "Go back"};
                int selection = showMenu("Update", menu);
                previousState = ManagerState::Inventory;

                switch(selection) {
                    case 1: currentState = ManagerState::UpdateProduct; // below this case, there will be a case named ManagerState::UpdateProduct. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the ManagerState::UpdateProduct case to execute
                        break;
                    case 2: showHeader("Process refunds"); admin.processRefunds(username); // call the processRefunds function from POSAdmin class
                        break;
                    case 3: currentState = previousState; // go back to the previous state (which is Inventory). Then, break this case. It will find for the case named ManagerState::Inventory to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid selection.\n"; 
                        Sleep(1200); 
                        break;
                }
                break;
            }
            case ManagerState::UpdateProduct: {
                showHeader("Update products");
                string category[] = {"Tops", "Bottoms", "Accessories", "Go back"};
                int selection = showMenu("Select category to update products", category);
                previousState = ManagerState::Inventory; // set previous state to Inventory for going back

                switch(selection) {
                    case 1:
                        system("cls");
                        showHeader("Update Tops");
                        admin.readProductsByCategory(productsDatabase, "Tops", username);
                        system("pause");
                        break;
                    case 2:
                        system("cls");
                        showHeader("Update Bottoms");
                        admin.readProductsByCategory(productsDatabase, "Bottoms", username);
                        system("pause");
                        break;
                    case 3:
                        system("cls");
                        showHeader("Update Accessories");
                        admin.readProductsByCategory(productsDatabase, "Accessories", username);
                        system("pause");
                        break;
                    case 4: currentState = previousState;  // go back to the previous state (which is Inventory). Then, break this case. It will find for the case named AdminState::Inventory to execute
                        break;
                    default:
                        cout << "Invalid selection.\n";
                        Sleep(1200);
                        break;
                }
                break;
            }
            case ManagerState::Monitoring: {
                string menu[] = {"View all products", "View sales", "Go back"};
                int selection = showMenu("Monitoring", menu);
                previousState = ManagerState::MainMenu;
                 switch(selection){
                    case 1: showHeader("View all products"); admin.readProducts(productsDatabase); // call the readProducts function from POSAdmin class
                        system("pause"); 
                        break;
                    case 2: // view sales
                        showHeader("View sales");
                        cout << "Daily Sales:\n"; admin.getDailySales();
                        cout << "\nMonthly Sales:\n"; admin.getMonthlySales();
                        cout << "\nYearly Sales:\n"; admin.getYearlySales();
                        cout << "\nOverall Sales:\n"; admin.getTotalSales();
                        cout << "\n"; system("pause");
                        break;
                    case 3: 
                        currentState = previousState; // go back to the previous state (which is MainMenu). Then, break this case. It will find for the case named ManagerState::MainMenu to execute
                        break;
                    default: // for invalid input
                        cout << "Invalid option\n";
                        Sleep(1200); 
                        break;
                }
                break;
            }
        }
    }
}

void PointOfSale::cashierMenu(string username) {
    const string productsDatabase = "database/products.csv";
    CashierState currentState = CashierState::MainMenu; // this is the current state (meaning that this can change where the user want to select)

    while (currentState != CashierState::Exit) {
        system("cls");
        bool transactionMade = false; // this will be used to determine if there was a transaction made

        switch (currentState) {
            case CashierState::MainMenu: {
                string menu[] = {"Tops", "Bottoms", "Accessories", "View Cart", "Logout"};
                int selection = showMenu("P.O.S. (Cashier)", menu);
                switch (selection) {
                    case 1: currentState = CashierState::Tops; // below this case, there will be a case named CashierState::Tops. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the CashierState::Tops case to execute
                        break;
                    case 2: currentState = CashierState::Bottoms; // below this case, there will be a case named CashierState::Bottoms. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the CashierState::Bottoms case to execute
                        break;
                    case 3: currentState = CashierState::Accessories; // below this case, there will be a case named CashierState::Accessories. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the CashierState::Accessories case to execute
                        break;
                    case 4: currentState = CashierState::ViewCart; // below this case, there will be a case named CashierState::ViewCart. If this was selected, it will break this case. Then, the loop will rerun this whole switch and find the CashierState::ViewCart case to execute
                        break;
                    case 5: currentState = CashierState::Exit; // change the currentState to Exit to logout
                        break;
                    default: // for invalid input
                        cout << "Invalid option\n"; 
                        Sleep(1000); 
                        break;
                }
                break;
            }
            case CashierState::Tops: { // state for tops
                string menu[] = {"T-Shirts", "Polo Shirts", "Jackets", "Go back"};
                int selection = showMenu("Tops", menu);
                switch (selection) {
                    case 1: showHeader("T-Shirts"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "T_Shirts", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 2: showHeader("Polo Shirts"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Polo_Shirts", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 3: showHeader("Jackets"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Jackets", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 4: currentState = CashierState::MainMenu; // go back to main menu
                        break;
                    default: // invalid input
                        cout << "Invalid selection\n"; 
                        Sleep(1200); 
                        break;
                }
                if (transactionMade) currentState = CashierState::MainMenu; // Go back to main menu after a sale
                break;
            }
            case CashierState::Bottoms: { // state for bottoms
                string menu[] = {"Jeans", "Shorts", "Skirts", "Go back"};
                int selection = showMenu("Bottoms", menu);

                switch (selection) {
                    case 1: showHeader("Jeans"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Jeans", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 2: showHeader("Shorts"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Shorts", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 3: showHeader("Skirts"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Skirts", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 4: currentState = CashierState::MainMenu; // go back to main menu
                        break;
                    default: // invalid input
                        cout << "Invalid option\n"; 
                        Sleep(1200); 
                        break;
                }
                if (transactionMade) currentState = CashierState::MainMenu; // Go back to main menu after a sale
                break;
            }
            case CashierState::Accessories: {
                string menu[] = {"Bags", "Headware", "Wallets", "Go back"};
                int selection = showMenu("Accessories", menu);
                switch (selection) {
                    case 1: showHeader("Bags"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Bags", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 2: showHeader("Headware"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Headware", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 3: showHeader("Wallets"); transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Wallets", username); // call the readProductsBySubcategory function from POSCashier class
                        break;
                    case 4: currentState = CashierState::MainMenu; // go back to main menu
                        break;
                    default: // invalid input
                        cout << "Invalid selection\n"; 
                        Sleep(1200); 
                        break;
                }
                if (transactionMade) currentState = CashierState::MainMenu; // Go back to main menu after a sale
                break;
            }
            case CashierState::ViewCart: { // state for viewing cart
                showHeader("View Cart");
                cashier.viewCart(username);
                currentState = CashierState::MainMenu; // Always return to main menu after viewing/processing cart
                break;
            }
        }
    }
}