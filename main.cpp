#include <iostream>
#include "functions/POS.h"
using namespace std;

int main(){
    // initialize variables
    PointOfSale POS;
    int retries = 0;
    string usernameInput, passwordInput, role;

    system("cls");
    showHeader("Threads and Charms");
    while(true){
        cout << "Enter your username: ";
        cin >> usernameInput;

        cout << "Enter your password: ";
        cin >> passwordInput;
        cout << "\n";

        if (POS.login(usernameInput, passwordInput, role)) {
            // check if the user's account is admin, manager, or cashier
            if(role == "Admin"){
                POS.adminMenu(POS, usernameInput);
            } else if(role == "Manager"){
                POS.managerMenu(POS, usernameInput);
            } else if (role == "Cashier"){
                POS.cashierMenu(POS, usernameInput);
            } else {
                cout << role;
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