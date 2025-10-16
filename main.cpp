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
                POS.adminMenu(usernameInput);
            } else if(role == "Manager"){
                POS.managerMenu(usernameInput);
            } else if (role == "Cashier"){
                POS.cashierMenu(usernameInput);
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