#include <iostream>
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

    // CRUD-Related Functions
    // ampersand is added so that it will make sure that the filename argument would not be modified
    void addProduct(const string& database, const string& productName, int quantity, int price) {
        if (isAlreadyInCsv(database, productName)) {
            cout << "Product '" << productName << "' is already in the CSV.\n";
        } else {
            int lastId = getLastId(database);
            int newId = lastId + 1;

            fstream fout;
            fout.open(database, ios::out | ios::app);

            fout << newId << ","
                << productName << ","
                << quantity << ","
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

    void deleteInformation(const string& filename, const string& toLook){
        ifstream input_file(filename);
        vector<string> rows;
        string line;
        bool found = false;

        // Read each line and filter out the one with the username
        while (getline(input_file, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ',');     // read id (not used)
            getline(ss, token, ',');     // read username
            if(token != toLook){
                rows.push_back(line);
            } else {
                found = true;
            }
        }
        input_file.close();

        if (!found) {
            cout << "Query not found";
            Sleep(1200);
        } else {
            // Write back filtered rows
            ofstream output_file(filename, ios::trunc);
            for (const auto& row : rows) {
                output_file << row << "\n";
            }
            output_file.close();

            cout << "Successfully deleted " << toLook << endl;
            Sleep(1200);
        }
    }
        
    void updateInformation(const string& filename, const string& productName, const string& type, const string& newValue) {
        ifstream fileIn(filename);
        if (!fileIn) {
            cout << "Cannot open file " << filename << endl;
            return;
        }

        string fileContent, line;
        bool found = false;

        while (getline(fileIn, line)) {
            stringstream ss(line);
            string number, name, quantity, price;
            getline(ss, number, ',');
            getline(ss, name, ',');
            getline(ss, quantity, ',');
            getline(ss, price, ',');

            if (name == productName) {
               if(type == "quantity"){
                    quantity = newValue;
               } else if(type == "price"){
                    price = newValue;
               } else if(type == "productName"){
                    name = newValue;
               }
                found = true;
            }

            fileContent += number + "," + name + "," + quantity + "," + price + "\n";
        }
        fileIn.close();

        if (!found) {
            cout << "Error: product '" << productName << "' not found.\n";
        } else {
            ofstream fileOut(filename);
            if (!fileOut) {
                cout << "Cannot write to file " << filename << endl;
            }

            fileOut << fileContent;
            fileOut.close();

            cout << "Updated successfully.\n";
            Sleep(1200);
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
                            "Update quantity or name",
                            "Delete product",
                            "Delete an account",
                            "Go back"
                        };
                        int inventoryInput = showMenu("Admin Utilities", inventoryMenu);
                        system("cls");

                        string deleteUserInput, deleteProductInput;

                        switch (inventoryInput) {
                            case 1: {
                                string productName;
                                int quantity, price;

                                cout << "Enter the product name (type 0 to return): ";
                                cin >> productName;
                                if (productName == "0") break;
                                cout << "Enter the quantity: ";
                                cin >> quantity;
                                cout << "Enter the price: ";
                                cin >> price;

                                POS.admin.addProduct(productsDatabase, productName, quantity, price);
                                break;
                            }

                            case 2: {
                                string username, password, role;
                                
                                cout << "Enter the username you want to add (type 0 to return): ";
                                cin >> username;

                                if (username == "0") break;

                                cout << "Enter the password: ";
                                cin >> password;
                                cout << "What is the role of the user? (Admin/Manager/Cashier): ";
                                cin >> role;

                                POS.admin.addUser(usersDatabase, username, password, role);
                                break;

                            }
                            case 3:
                                // since the terminal would not clear if it expects an input to the user
                                // and we aim to let the inventory stay for a little while until the user wants to go back
                                POS.admin.readProducts(productsDatabase);

                                cout << "\nType any number to go back: ";
                                int dummyInput;
                                cin >> dummyInput;
                                break;

                            case 4: {
                                string updateMenu[] = {"Product", "Account", "Go back"};
                                int updateInput = showMenu("Update", updateMenu);
                                system("cls");

                                if(updateInput == 1){
                                    string updateProductMenu[] = {"Product name", "Product quantity", "Product price", "Go back"};
                                    int updateProductInput = showMenu("Update product", updateProductMenu);

                                    if(updateProductInput == 4){
                                        // so that the user would not have to enter a product name to exit
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
                                } else if (updateInput == 2){
                                    string updateProductMenu[] = {"Username", "Password", "Role"};
                                    int updateProductInput = showMenu("Update account", updateProductMenu);
                                    
                                    Sleep(1200);
                                } else if(updateInput == 3){
                                    break;
                                } else {
                                    cout << "Invalid selection";
                                }

                                break;
                            }

                            case 5:
                                cout << "Enter the product you want to delete (type 0 to return): ";
                                cin >> deleteProductInput;

                                if (deleteProductInput == "0") break;

                                POS.admin.deleteInformation(productsDatabase, deleteProductInput);
                                break;

                            case 6:
                                cout << "Enter the username you want to delete (type 0 to return): ";
                                cin >> deleteUserInput;

                                if (deleteUserInput == "0") break;

                                POS.admin.deleteInformation(usersDatabase, deleteUserInput);
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