#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <conio.h>
using namespace std;

string admin_username = "admin";
string admin_password = "rain";

string cashier_username = "cashier";
string cashier_password = "rain";

class POSAdmin {
    public:
    int getLastProductId(const string& filename) {
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

    bool isProductInCsv(const string& filename, const string& productNameToCheck) {
        ifstream fin(filename);
        string line;

        while (getline(fin, line)) {
            if (line.find(productNameToCheck) != string::npos) {
                return true;  // Product name found somewhere in the database
            }
        }
        return false;
    }

    void addProduct(string productName, int quantity, int price){
        const string filename = "database.csv";

        if (isProductInCsv(filename, productName)) {
            cout << "Product '" << productName << "' is already in the CSV.\n";
            return;
        } else {
            int lastId = getLastProductId(filename);
            int newId = lastId + 1;

            fstream fout;
            fout.open(filename, ios::out | ios::app);

            fout << newId << ", "
                << productName << ", "
                << quantity << ", "
                << price << "\n";

            fout.close();
            cout << "Successfully added product '" << productName << "' with price " << price << ".\n";
        }
    }
};

class PointOfSale {
    public:
    POSAdmin admin;
};

void adminMenu(PointOfSale& POS) {
    while (true) {
        int adminSelection;
        cout << "1. Inventory\n2. Monitoring\nSelect (1/2): ";
        cin >> adminSelection;

        if (adminSelection == 1) {
            while (true) {  // loop for the inventory submenu
                int inventoryInput;
                cout << "1. Add products\n2. Refresh all available products\n3. Update quantity or name\n4. Delete product\n5. Go back\nSelect (1/2/3/4/5): ";
                cin >> inventoryInput;

                if (inventoryInput == 1) {
                    int quantity, price;
                    string productName;

                    cout << "Enter the product name: ";
                    cin >> productName;
                    cout << "Enter the quantity: ";
                    cin >> quantity;
                    cout << "Enter the price: ";
                    cin >> price;

                    POS.admin.addProduct(productName, quantity, price);
                    // this code will return to the menu above
                    continue;
                } else if (inventoryInput == 3){

                } else if (inventoryInput == 5) {
                    // Go back to main admin menu
                    break;
                } else {
                    cout << "Option not implemented or invalid. Please try again.\n";
                }
            }
        } else if (adminSelection == 2) {
            cout << "Monitoring selected (feature not implemented).\n";
        } else {
            cout << "Invalid option, please select 1 or 2.\n";
        }
    }
}

void cashierMenu() {
    cout << "Welcome CASHIER\n";
    // Add cashier-related options here
}


int main(){
    // initialize class POS
    PointOfSale POS;

    // initialize a VERY SECURE username and password
    int ch, retries = 0;
    string usernameInput, passwordInput;

    for(;;){
        cout << "Enter your username: ";
        cin >> usernameInput;

        cout << "Enter your password: ";
        cin >> passwordInput;

        // determine if the user is an ADMIN or CASHIER
        if (usernameInput == admin_username && passwordInput == admin_password) {
            adminMenu(POS);
        } else if (usernameInput == cashier_username && passwordInput == cashier_password) {
            cashierMenu();
            break;
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