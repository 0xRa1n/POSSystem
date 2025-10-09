#include <iostream>
#include <string>
#include <sstream>
#include <conio.h>
#include <cctype>
#include <cstdlib>
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

#include "POSAdmin.h"
#include "POSCashier.h"

void showHeader(string headerTitle){
    system("cls");
    cout << "---------------------------------" << endl;
    cout << headerTitle << endl ;
    cout << "---------------------------------" << endl;
}
// ...existing code...
template<size_t N>
int showMenu(string headerTitle, string (&menu)[N]) {      
    showHeader(headerTitle);

    // print menu
    int i = 1;
    for (string menuOptions : menu) {
        cout << "[" << i++ << "] " << menuOptions << endl;
    }

    cout << "---------------------------------" << endl;
    cout << "Option: ";

    int input;
    cin >> input;
    // in this manner, if the user inputs a non-integer value, it will not crash the program
    // or not use try-catch
    if(cin.fail()){
        cin.clear(); // clear the fail state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // the numeric_limits basically ignores everything until the next newline character
        // in this manner, everything will be cleared until it comes back to the loop
        return -1;
    } else {
        return input;
    }
}
// ...existing code...
class PointOfSale {
    public:
    // POSAdmin is called from the functions/POSAdmin.h
    POSAdmin admin;
    POSCashier cashier;

    bool login(string username, string password, string& outRole) {
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
            size_t pos1 = line.find(','); // first comma
            size_t pos2 = line.find(',', pos1 + 1); // second comma
            size_t pos3 = line.find(',', pos2 + 1); // third comma

            // substrings the previous values, and gets the username, password, and role
            string csvUsername = line.substr(pos1 + 1, pos2 - pos1 - 1); // username
            string csvPassword = line.substr(pos2 + 1, pos3 - pos2 - 1); // password
            string csvRole = line.substr(pos3 + 1); // role

            if (csvUsername == username && csvPassword == password) {
                outRole = csvRole;
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    void adminMenu(PointOfSale& POS) {
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
                            "Update account or product",
                            "Delete a product",
                            "Delete an account",
                            "Go back"
                        };
                        int inventoryInput = showMenu("Admin utilities", inventoryMenu);
                        system("cls");

                        string deleteUserInput, deleteProductInput;

                        switch (inventoryInput) {
                            case 1: // add product
                                showHeader("Add a product");

                                POS.admin.addProduct(productsDatabase);
                                break;
                            case 2: // add account                
                                showHeader("Add an account");              
                                cout << "Add account" << endl;

                                POS.admin.addUser(usersDatabase);
                                break;
                            case 3: // view products
                                showHeader("View all products");
                                // since the terminal would not clear if it expects an input to the user
                                // and we aim to let the inventory stay for a little while until the user wants to go back
                                POS.admin.readProducts(productsDatabase);

                                cout << "\nType any number to go back: ";
                                int dummyInput;
                                cin >> dummyInput;
                                break;
                            case 4: { // update
                                // initialize a loop where it contains the menu Product, Account, and Go back
                                while(true){
                                    system("cls");
                                    string updateMenu[] = {"Product", "Account", "Go back"};
                                    int updateInput = showMenu("Update", updateMenu);
                                    system("cls");

                                    if(updateInput == 1){                  
                                        // initialize a loop to update the information of a product                          
                                        while(true){
                                            string updateProductMenu[] = {"Product name", "Product quantity", "Product price", "Go back"};
                                            int updateProductInput = showMenu("Update product", updateProductMenu);

                                            if(updateProductInput == 4){
                                                // this will end the loop, and will return to the previous loop
                                                break;
                                            }

                                            switch(updateProductInput){
                                                case 1: { // update product name
                                                    POS.admin.updateProductFields("product", productsDatabase, "name");
                                                    break;
                                                }
                                                case 2: { // update product quantity
                                                    POS.admin.updateProductFields("product", productsDatabase, "quantity");
                                                    break;
                                                }
                                                case 3: { // update product price
                                                    POS.admin.updateProductFields("product", productsDatabase, "price");
                                                    break;
                                                }
                                                default:
                                                    cout << "Invalid selection";
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
                                                case 1: { // update username
                                                    POS.admin.updateProductFields("account", usersDatabase, "username");
                                                    break;
                                                }
                                                case 2: { // update password
                                                    POS.admin.updateProductFields("account", usersDatabase, "password");
                                                    break;
                                                }
                                                case 3: { // update role
                                                    POS.admin.updateProductFields("account", usersDatabase, "role");
                                                    break;
                                                }
                                                default:
                                                    cout << "Invalid selection";
                                                    break;
                                            }
                                        }
                                    } else if(updateInput == 3){ // go back
                                        // this will go back to the admin menu
                                        break;
                                    } else {
                                        cout << "Invalid selection";
                                    }
                                }
                                break;
                            }
                            case 5: // delete product
                                showHeader("Delete product");
                                POS.admin.deleteInformation(productsDatabase);
                                break;
                            case 6: // delete account
                                showHeader("Delete account");
                                POS.admin.deleteInformation(usersDatabase);
                                break;
                            case 7: // go back
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
                case 2:
                    POS.admin.readProducts(productsDatabase);
                    cout << "\nType any number to go back: ";
                    int dummyInput;
                    cin >> dummyInput;
                    break;

                case 3:
                    cout << "Goodbye.\n";
                    exit(0);

                default:
                    cout << "Invalid option\n";
                    Sleep(1000);
                    break;
            }
        }
    }

    void managerMenu(PointOfSale& POS) {
        cout << "Welcome MANAGER\n";
        // Add manager-related options here
    }
    void cashierMenu(PointOfSale& POS) {
        const string productsDatabase = "database/products.csv";
        while(true){
            system("cls");
            string menu[] = {"Tops", "Bottoms", "Accessories", "Exit"};
            int cashierSelection = showMenu("P.O.S. (Cashier)", menu);

            switch (cashierSelection) {
                case 1 : {
                    string topsSubCategory[] = {"T-Shirts", "Polo Shirts", "Jackets", "Go back"};
                    int topsSelection = showMenu("Tops", topsSubCategory);

                    if(topsSelection == 4){
                        // go back to the previous menu
                        break;
                    }

                    switch(topsSelection){
                        case 1: 
                            showHeader("T-Shirts");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "T_Shirts");
                            break;
                            
                        case 2:
                            showHeader("Polo Shirts");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Polo_Shirts");
                            break;

                        case 3:
                            showHeader("Jackets");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Jackets");
                            break;
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                    break;
                }
                case 2: {
                    string bottomsSubCategory[] = {"Jeans", "Shorts", "Skirts", "Go back"};
                    int bottomsSelection = showMenu("Bottoms", bottomsSubCategory);

                    if(bottomsSelection == 4){
                        // go back to the previous menu
                        break;
                    }

                    switch(bottomsSelection){
                        case 1: 
                            showHeader("Jeans");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Jeans");
                            break;
                            
                        case 2:
                            showHeader("Shorts");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Shorts");
                            break;

                        case 3:
                            showHeader("Skirts");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Skirts");
                            break;
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                    break;
                }
                case 3: {
                    string accessoriesSubCategory[] = {"Bags", "Headware", "Wallets", "Go back"};
                    int accessoriesSelection = showMenu("Accessories", accessoriesSubCategory);

                    if(accessoriesSelection == 4){
                        // go back to the previous menu
                        break;
                    }

                    switch(accessoriesSelection){
                        case 1: 
                            showHeader("Bags");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Bags");
                            break;
                            
                        case 2:
                            showHeader("Headware");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Headware");
                            break;

                        case 3:
                            showHeader("Wallets");
                            POS.cashier.readProductsBySubcategory(productsDatabase, "Wallets");
                            break;
                        default:
                            cout << "Invalid option\n";
                            Sleep(1200);
                            break;
                    }
                    break;
                }
                case 4:
                    cout << "Goodbye.\n";
                    exit(0);
                default:
                    cout << "Invalid option\n";
                    Sleep(1000);
                    break;
            }
        }
    }
};