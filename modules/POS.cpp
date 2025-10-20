#include "POS.h"
#include "utilities.h" // For showHeader, etc.
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

bool PointOfSale::login(string username, string password, string& outRole) {
    const string userAccounts = "database/userAccounts.csv";
    // open the userAccounts database
    ifstream file(userAccounts);
    
    // check if the file exists
    if (!file.is_open()) {
        cerr << "Could not open file " << userAccounts << endl;
        return false;
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        // finds the position of each value after the comma
        int pos1 = line.find(','); // first comma
        int pos2 = line.find(',', pos1 + 1); // second comma
        int pos3 = line.find(',', pos2 + 1); // third comma

        // substrings the previous values, and gets the username, password, and role
        // 1 is given to remove the comma
        string csvUsername = line.substr(pos1 + 1, pos2 - pos1 - 1); // username
        // subtract the position of the second comma to the first comma to get the length of the username
        string csvPassword = line.substr(pos2 + 1, pos3 - pos2 - 1); // password
        // subtract the position of the third comma to the second comma to get the length of the role
        string csvRole = line.substr(pos3 + 1); // role
        // no length is given since it will get the rest of the string since there are no more commas

        // .find == find the position
        // .substr == using the position of the .find, remove the commas, and get the value between them

        if (csvUsername == username && csvPassword == password) {
            outRole = csvRole;
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void PointOfSale::adminMenu(string username) {
    const string productsDatabase = "database/products.csv";
    const string usersDatabase = "database/userAccounts.csv";

    // initalize a loop, where it contains the menu Inventory, Monitoring, and Logout
    while (true) {
        system("cls");
        string menu[] = {"Utilities", "Monitoring", "Logout"};
        int adminSelection = showMenu("P.O.S. (Admin)", menu);
        system("cls");

        switch (adminSelection) {
            case 1: {  // Inventory submenu
                // intialize another loop that contains the menu of the admin menu
                while (true) {
                    system("cls");
                    string inventoryMenu[] = {
                        "Add products",
                        "Add an account",
                        "View all products",
                        "View all accounts",
                        "Update account or product",
                        "Delete a product",
                        "Delete an account",
                        "Go back"
                    };
                    int inventoryInput = showMenu("Admin utilities", inventoryMenu);
                    system("cls");

                    switch (inventoryInput) {
                        case 1: // add product
                            showHeader("Add a product");
                            admin.addProduct(productsDatabase, username);
                            break;
                        case 2: // add account
                            showHeader("Add an account");
                            admin.addUser(usersDatabase, username);
                            break;
                        case 3: // view products
                            showHeader("View all products");
                            // since the terminal would not clear if it expects an input to the user
                            // and we aim to let the inventory stay for a little while until the user wants to go back
                            admin.readProducts(productsDatabase);

                            system("pause");
                            break;
                        case 4: // view all accounts
                            showHeader("View all accounts");
                            admin.getAllAccounts(usersDatabase);

                            system("pause");
                            break;
                        case 5: { // update
                            // initialize a loop where it contains the menu Product, Account, and Go back
                            while(true){
                                system("cls");
                                string updateMenu[] = {"Product", "Account", "Go back"};
                                int updateInput = showMenu("Update", updateMenu);
                                system("cls");

                                if(updateInput == 1){                  
                                    // initialize a loop to update the information of a product                          
                                    while(true){
                                        string updateProductMenu[] = {"Product name", "Product Sub-Category", "Product quantity", "Product price", "Go back"};
                                        int updateProductInput = showMenu("Update product", updateProductMenu);

                                        if(updateProductInput == 5){
                                            // this will end the loop, and will return to the previous loop
                                            break;
                                        }

                                        switch(updateProductInput){
                                            case 1:  // update product name
                                                admin.updateProductFields("product", productsDatabase, "name", username);
                                                break;
                                            case 2:  // update product sub-category
                                                admin.updateProductFields("product", productsDatabase, "subCategory", username);
                                                break;
                                            case 3:  // update product quantity
                                                admin.updateProductFields("product", productsDatabase, "quantity", username);
                                                break;
                                            case 4:  // update product price
                                                admin.updateProductFields("product", productsDatabase, "price", username);
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
                                        string updateAccountMenu[] = {"Username", "Password", "Role", "Go back"};
                                        int updateAccountInput = showMenu("Update account", updateAccountMenu);

                                        if(updateAccountInput == 4){
                                            // this will end the loop, and will return to the previous loop
                                            break;
                                        }

                                        switch(updateAccountInput){
                                            case 1:  // update username
                                                admin.updateProductFields("account", usersDatabase, "username", username);
                                                break;
                                            case 2:  // update password
                                                admin.updateProductFields("account", usersDatabase, "password", username);
                                                break;
                                            case 3:  // update role
                                                admin.updateProductFields("account", usersDatabase, "role", username);
                                                break;
                                            default:
                                                cout << "Invalid selection";
                                                Sleep(1200);
                                                break;
                                        }
                                    }
                                } else if(updateInput == 3){ // go back
                                    // this will go back to the admin menu
                                    break;
                                } else {
                                    cout << "Invalid selection";
                                    Sleep(1200);
                                }
                            }
                            break;
                        }
                        case 6: // delete product
                            showHeader("Delete product");
                            admin.deleteInformation("products", productsDatabase, username);
                            break;
                        case 7: // delete account
                            showHeader("Delete account");
                            admin.deleteInformation("accounts", usersDatabase, username);
                            break;
                        case 8:
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
                while(true){
                    system("cls");
                    string monitoringMenu[] = {"View all products", "View today's sales", "View total sales", "Go back"};
                    int monitoringSelection = showMenu("Monitoring", monitoringMenu);
                    system("cls");

                    switch(monitoringSelection){
                        case 1:
                            showHeader("View all products");
                            admin.readProducts(productsDatabase);

                            system("pause");
                            break;
                        case 2:
                            showHeader("View today's sales");
                            admin.getTodaysSales("database/transactions/cashierTransactions.csv");

                            system("pause");
                            break;
                        case 3:
                            showHeader("View total sales");
                            admin.getTotalSales("database/transactions/cashierTransactions.csv");

                            system("pause");
                            break;
                        case 4:
                            // go back to the previous menu
                            goto endMonitoringLoop; // it will break the current loop and go to the endMonitoringLoop label, because it is empty, it will just resort to the loop above it
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                }
                endMonitoringLoop: ;
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
    const string productsDatabase = "database/products.csv";
    const string usersDatabase = "database/userAccounts.csv";

    // initalize a loop, where it contains the menu Inventory, Monitoring, and Logout
    while (true) {
        system("cls");
        string menu[] = {"Utilities", "Monitoring", "Logout"};
        int adminSelection = showMenu("P.O.S. (Manager)", menu);
        system("cls");

        switch (adminSelection) {
            case 1: {  // Inventory submenu
                // intialize another loop that contains the menu of the admin menu
                while (true) {
                    system("cls"); // initial clearing for this loop
                    string inventoryMenu[] = {
                        "Add products",
                        "View all products",
                        "Update product",
                        "Delete a product",
                        "Go back"
                    };
                    int inventoryInput = showMenu("Admin utilities", inventoryMenu);
                    system("cls"); // if an error arise, it will reach this part to clear the screen again

                    switch (inventoryInput) {
                        case 1: // add product
                            showHeader("Add a product");
                            admin.addProduct(productsDatabase, username);
                            break;
                        case 2: // view products
                            showHeader("View all products");
                            // since the terminal would not clear if it expects an input to the user
                            // and we aim to let the inventory stay for a little while until the user wants to go back
                            admin.readProducts(productsDatabase);

                            system("pause");
                            break;
                        case 3: { // update
                            // initialize a loop to update the information of a product                          
                            while(true){
                                string updateProductMenu[] = {"Product name", "Product Sub-Category", "Product quantity", "Product price", "Go back"};
                                int updateProductInput = showMenu("Update product", updateProductMenu);

                                if(updateProductInput == 5){
                                    // this will end the loop, and will return to the previous loop
                                    break;
                                }

                                switch(updateProductInput){
                                    case 1:  // update product name
                                        admin.updateProductFields("product", productsDatabase, "name", username);
                                        break;
                                    case 2:  // update product sub-category
                                        admin.updateProductFields("product", productsDatabase, "subCategory", username);
                                        break;
                                    case 3:  // update product quantity
                                        admin.updateProductFields("product", productsDatabase, "quantity", username);
                                        break;
                                    case 4:  // update product price
                                        admin.updateProductFields("product", productsDatabase, "price", username);
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
                while(true){
                    system("cls");
                    string monitoringMenu[] = {"View all products", "View today's sales", "View total sales", "Go back"};
                    int monitoringSelection = showMenu("Monitoring", monitoringMenu);
                    system("cls");

                    switch(monitoringSelection){
                        case 1:
                            showHeader("View all products");
                            admin.readProducts(productsDatabase);

                            system("pause");
                            break;
                        case 2:
                            showHeader("View today's sales");
                            admin.getTodaysSales("database/transactions/cashierTransactions.csv");

                            system("pause");
                            break;
                        case 3:
                            showHeader("View total sales");
                            admin.getTotalSales("database/transactions/cashierTransactions.csv");

                            system("pause");
                            break;
                        case 4:
                            // go back to the previous menu
                            goto endMonitoringLoop; // it will break the current loop and go to the endMonitoringLoop label, because it is empty, it will just resort to the loop above it
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                }
                endMonitoringLoop: ;
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
                bool transactionMade = false; // Declare before the switch
                while(true){
                    system("cls");
                    string topsSubCategory[] = {"T-Shirts", "Polo Shirts", "Jackets", "Go back"};
                    int topsSelection = showMenu("Tops", topsSubCategory);
                    system("cls");

                    if(topsSelection == 4){
                        break; // Go back to the main cashier menu
                    }

                    switch(topsSelection){
                        case 1: 
                            showHeader("T-Shirts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "T_Shirts", username);
                            break;
                            
                        case 2:
                            showHeader("Polo Shirts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Polo_Shirts", username);
                            break;

                        case 3:
                            showHeader("Jackets");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Jackets", username);
                            break;
                        default:
                            cout << "Invalid selection\n";
                            Sleep(1200);
                            break;
                    }
                    // If a transaction was completed, break out of the sub-category loop
                    // because if a transaction was made, we want to go back to the main cashier menu
                    // otherwise after the transaction, it will still be in the sub-category menu
                    if(transactionMade) break;
                }
                break;
            }
            case 2: { // Bottoms
                bool transactionMade = false; // Declare before the switch
                while(true){
                    system("cls");
                    string bottomsSubCategory[] = {"Jeans", "Shorts", "Skirts", "Go back"};
                    int bottomsSelection = showMenu("Bottoms", bottomsSubCategory);
                    system("cls");

                    if(bottomsSelection == 4){
                        break; // Go back to the main cashier menu
                    }

                    switch(bottomsSelection){
                        case 1: 
                            showHeader("Jeans");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Jeans", username);
                            break;
                            
                        case 2:
                            showHeader("Shorts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Shorts", username);
                            break;

                        case 3:
                            showHeader("Skirts");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Skirts", username);
                            break;
                        default:
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
                bool transactionMade = false; // Declare before the switch
                while(true){
                    system("cls");
                    string accessoriesSubCategory[] = {"Bags", "Headware", "Wallets", "Go back"};
                    int accessoriesSelection = showMenu("Accessories", accessoriesSubCategory);
                    system("cls");
                    
                    if(accessoriesSelection == 4){
                        break; // Go back to the main cashier menu
                    }

                    switch(accessoriesSelection){
                        case 1: 
                            showHeader("Bags");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Bags", username);
                            break;
                            
                        case 2:
                            showHeader("Headware");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Headware", username);
                            break;

                        case 3:
                            showHeader("Wallets");
                            transactionMade = cashier.readProductsBySubcategory(productsDatabase, "Wallets", username);
                            break;
                        default:
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
                // Assuming viewCart is also updated to return a bool
                bool transactionMade = cashier.viewCart(username);
                break;
            }
            case 5:
                return; // Logout
            default:
                cout << "Invalid option\n";
                Sleep(1000);
                break;
        }
    }
}
// ...existing code...