#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <bits/stdc++.h>
#include <iomanip> // for setw
using namespace std;

template<size_t N>
int showMenu(string title, string (&menu)[N]) {      
    cout << "---------------------------------" << endl;
    cout << title << endl ;
    cout << "---------------------------------" << endl;

    int selection;
    int size = sizeof(menu) / sizeof(menu[0]); // correct element count
    int i = 1;
    
    for(string menuOptions : menu){
        cout << "[" << i++ << "] " << menuOptions << endl;
    }      

    cout << "---------------------------------" << endl;
    cout << "Option: ";
    cin >> selection;

    return selection;
}

class POSAdmin {
    public:
    // this part is related to adding an a product
    int getLastId(const string& filename) {
        ifstream fin(filename);
        string line;
        int lastId = 0;

        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr;
            if (getline(ss, idStr, ',')) {
                try {
                    int id = stoi(idStr);
                    if (id > lastId) lastId = id;
                } catch (...) {}
            }
        }
        return lastId;
    }

    bool isAlreadyInCsv(const string& filename, const string& productNameToCheck) {
        ifstream fin(filename);
        string line;

        while (getline(fin, line)) {
            if (line.find(productNameToCheck) != string::npos) {
                return true;  // Product name found somewhere in the database
            }
        }
        return false;
    }

    void addProduct(const string& database, const string& productName, int quantity, int price) {
        if (isAlreadyInCsv(database, productName)) {
            cout << "Product '" << productName << "' is already in the CSV.\n";
        } else {
            int lastId = getLastId(database);
            int newId = lastId + 1;

            fstream fout;
            fout.open(database, ios::out | ios::app);

            fout << newId << ", "
                << productName << ", "
                << quantity << ", "
                << price << "\n";

            fout.close();
            cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
        }

        Sleep(1200);
    }

    void addUser(const string& database, const string& username, const string& password, const string& role){
        if(isAlreadyInCsv(database, username)){
            cout << "User is already in the database!";
        } else {
            int lastUserId = getLastId(database);
            int newUserId = lastUserId + 1;

            fstream fout;
            fout.open(database, ios::out | ios::app);

            fout << newUserId << ","
                << username << ","
                << password << ","
                << role << "\n";

            fout.close();
            cout << "Successfully added user " << username << endl;
        }
        Sleep(1200);
    }

    // ampersand is added so that it will make sure that the filename argument would not be modifieds
    void readProducts(const string& database) {
        ifstream file(database);
        if (!file.is_open()) {
            cout << "Failed to open file\n";
            return;
        }

        // Read all rows first
        vector<vector<string>> rows;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            vector<string> row;
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            rows.push_back(row);
        }
        file.close();

        if (rows.empty()) return;

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
    }
};

class PointOfSale {
    public:
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

        while (true) {
            system("cls");
            string menu[] = {"Inventory", "Monitoring", "Logout"};
            int adminSelection = showMenu("Admin", menu);
            system("cls");

            switch (adminSelection) {
                case 1: {  // Inventory submenu
                    while (true) {
                        system("cls");
                        string inventoryMenu[] = {
                            "Add products",
                            "Add an account",
                            "View all products",
                            "Update quantity or name (U)",
                            "Delete product (U)",
                            "Delete an account (U)",
                            "Go back"
                        };
                        int inventoryInput = showMenu("Inventory", inventoryMenu);
                        system("cls");

                        int quantity, price;
                        string productName, username, password, role;

                        switch (inventoryInput) {
                            case 1:
                                cout << "Enter the product name (type 0 to return): ";
                                cin >> productName;
                                if (productName == "0") break;
                                cout << "Enter the quantity: ";
                                cin >> quantity;
                                cout << "Enter the price: ";
                                cin >> price;

                                POS.admin.addProduct(productsDatabase, productName, quantity, price);
                                break;

                            case 2:
                                cout << "Enter the username you want to add: ";
                                cin >> username;
                                cout << "Enter the password: ";
                                cin >> password;
                                cout << "What is the role of the user? (Admin/Manager/Cashier): ";
                                cin >> role;

                                POS.admin.addUser(usersDatabase, username, password, role);
                                break;

                            case 3:
                                // since the terminal would not clear if it expects an input to the user
                                // and we aim to let the inventory stay for a little while until the user wants to go back
                                POS.admin.readProducts(productsDatabase);
                                cout << "\nType any number to go back: ";
                                int dummyInput;
                                cin >> dummyInput;
                                break;

                            case 4:
                                cout << "Feature not implemented yet.\n";
                                system("pause");
                                break;

                            case 5:
                                cout << "Feature not implemented yet.\n";
                                system("pause");
                                break;

                            case 6:
                                cout << "Feature not implemented yet.\n";
                                system("pause");
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
                    cout << "Monitoring selected (feature not implemented).\n";
                    Sleep(1000);
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

int main(){
    // initialize class POS
    PointOfSale POS;

    // initialize a VERY SECURE username and password
    int retries = 0;
    string usernameInput, passwordInput, role;

    system("cls");
    cout << "---------------------------------" << endl;
    cout << "Login" << endl ;
    cout << "---------------------------------" << endl;
    while(true){
        cout << "Enter your username: ";
        cin >> usernameInput;

        cout << "Enter your password: ";
        cin >> passwordInput;
        cout << "\n";

        if (POS.login(usernameInput, passwordInput, role)) {
            // check if the user's account is admin, manager, or cashier
            if(role == "Admin"){
                POS.adminMenu(POS);
            } else if(role == "Manager"){
                POS.managerMenu(POS);
            } else if (role == "Cashier"){
                POS.cashierMenu(POS);
            } else {
                cout << "Invalid role received.";
                break;
            }
        } else {
            if (retries == 2){
                cout << "Maximum attempt reached.";
                break;
            }
            cout << "Invalid credentials" << endl;
            retries++;
        }
    }
}