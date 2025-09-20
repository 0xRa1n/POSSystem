#include <iostream>
#include <string>
#include <sstream>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

#include "POSAdmin.h"

template<size_t N>
int showMenu(string title, string (&menu)[N]) {      
    cout << "---------------------------------" << endl;
    cout << title << endl ;
    cout << "---------------------------------" << endl;

    int selection;
    // count all the elements in the array
    int size = sizeof(menu) / sizeof(menu[0]);
    // the counter should start at 1
    int i = 1;
    
    for(string menuOptions : menu){
        cout << "[" << i++ << "] " << menuOptions << endl;
    }      

    cout << "---------------------------------" << endl;
    cout << "Option: ";
    cin >> selection;

    return selection;
}

class PointOfSale {
    public:
    // POSAdmin is called from the functions/POSAdmin.h
    POSAdmin admin;

    bool login(const string& username, const string& password, string& outRole) {
        const string userAccounts = "database/userAccounts.csv";
        ifstream file(userAccounts);
        if (!file.is_open()) {
            cerr << "Could not open file " << userAccounts << endl;
            return false;
        }

        string line;
        getline(file, line); // skip header

        while (getline(file, line)) {
            // finds the position of each value after the comma
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            // substrings the previous values, and gets the username, password, and role
            string csvUsername = line.substr(pos1 + 1, pos2 - pos1 - 1);
            string csvPassword = line.substr(pos2 + 1, pos3 - pos2 - 1);
            string csvRole = line.substr(pos3 + 1);

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
            string menu[] = {"Inventory", "Monitoring", "Logout"};
            int adminSelection = showMenu("Admin", menu);
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
                            "Update quantity or name",
                            "Delete product",
                            "Delete an account",
                            "Go back"
                        };
                        int inventoryInput = showMenu("Admin Utilities", inventoryMenu);
                        system("cls");

                        string deleteUserInput, deleteProductInput;

                        switch (inventoryInput) {
                            case 1: 
                                POS.admin.addProduct(productsDatabase);
                                break;
                            case 2: 
                                POS.admin.addUser(usersDatabase);
                                break;
                            case 3:
                                // since the terminal would not clear if it expects an input to the user
                                // and we aim to let the inventory stay for a little while until the user wants to go back
                                POS.admin.readProducts(productsDatabase);

                                cout << "\nType any number to go back: ";
                                int dummyInput;
                                cin >> dummyInput;
                                break;
                            case 4: {
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

                                            string originalProductName;

                                            cout << "Enter the product name: ";
                                            cin >> originalProductName;

                                            switch(updateProductInput){
                                                case 1: {
                                                    string newProductName;

                                                    cout << "Enter the new product name: ";
                                                    cin >> newProductName;

                                                    POS.admin.updateInformation(productsDatabase, originalProductName, "productName", newProductName);
                                                    break;
                                                }
                                                case 2: {
                                                    string newQuantity;
            
                                                    cout << "Enter the new quantity: ";
                                                    cin >> newQuantity;

                                                    POS.admin.updateInformation(productsDatabase, originalProductName, "quantity", newQuantity);
                                                    break;
                                                }
                                                case 3: {
                                                    string newPrice;
            
                                                    cout << "Enter the new price: ";
                                                    cin >> newPrice;

                                                    POS.admin.updateInformation(productsDatabase, originalProductName, "price", newPrice);
                                                    break;
                                                }
                                                default:
                                                    cout << "Invalid selection";
                                                    break;
                                            }
                                            break;
                                        }
                                    } else if (updateInput == 2){
                                        string updateProductMenu[] = {"Username", "Password", "Role"};
                                        int updateProductInput = showMenu("Update account", updateProductMenu);
                                        
                                        Sleep(1200);
                                    } else if(updateInput == 3){
                                        // this will go back to the admin menu
                                        break;
                                    } else {
                                        cout << "Invalid selection";
                                    }
                                }
                                break;
                            }
                            case 5:
                                POS.admin.deleteInformation(productsDatabase);
                                break;
                            case 6:
                                POS.admin.deleteInformation(usersDatabase);
                                break;
                            case 7: 
                                // Go back to admin main menu
                                goto endInventoryLoop;
                            default:
                                cout << "Invalid selection. Try again.\n";
                                system("pause");
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
        cout << "Welcome CASHIER\n";
        // Add cashier-related options here
    }
};