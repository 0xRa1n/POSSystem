#include <iostream>
#include "./include/POS.h"
#include "./include/utilities.h"
using namespace std;

int main(){
    // the contents of the include folder are called header files. Header files are used to declare the structure of classes and functions that can be used in multiple source files.
    // they are all a declaration of functions and classes that are defined in the src folder.
    // this is done to improve code organization and reusability.

    // initialize variables
    PointOfSale POS; // since we need to seperate the functions for each role, we need to create a class named PointOfSale that will handle the login and menu navigation
    int retry = 0;
    string usernameInput, passwordInput, role; // the role will be used to determine which menu to show || it will be empty, but, after passing it throught the login function, it will contain the role of the user

    while(true){
        system("cls");
        showHeader("Threads and Charms");
        cout << "Enter your username: ";
        cin >> usernameInput;

        cout << "Enter your password: ";
        cin >> passwordInput;
        cout << "\n";

        if (POS.login(usernameInput, passwordInput, role)) {
            cout << "Login successful! Welcome, " << usernameInput << " (" << role << ").\n";
            Sleep(1200);
            
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
            if (retry == 2){ // maximum of 3 attempts
                cout << "Maximum attempt reached.";
                break;
            }
            cout << "Invalid credentials" << endl;
            Sleep(1200);
            retry++;
        }
    }
}