#pragma once // Ensure the header is only included once

// #include <iostream>
// #include <cctype> // for toupper() and tolower()
// #include <sstream> // for stringstream
#include <windows.h> // for Sleep() and system("cls")
// #include <bits/stdc++.h> // this includes all standard libraries
#include <string>
#include <iostream>
#include <limits>
using namespace std;

void showHeader(string headerTitle);
bool handleInputError();
int getLastId(string filename);
bool isAlreadyInCsv(string filename, string productNameToCheck);

template<size_t N>
// the template size_t N is used so that the function will know the size of the array passed to it
// and where will it end
// for example: string updateAccountMenu[] = {"Username", "Password", "Role", "Go back"};
// the size of this array is 4, so N will be 4. and will print 4 options

// inline tells the compiler that it's alright to be used in multiple files without causing redefinition errors
inline int showMenu(string headerTitle, string (&menu)[N]) {  // pass array by reference to retain size information
    // the (&menu) in the string (&menu)[N] is used to pass the array by reference
    // meaning that the array passed here will not be a copy of the original array, and it will retain the SIZE
    showHeader(headerTitle);

    // print menu
    int i = 1;
    // in here, the N is not explicitly used, but, the variable menu uses the size of the array passed to it to know where to end
    // its just the same as (size_t t = 0; t < N; t++)
    for (string menuOptions : menu) { // iterate through each option in the menu array
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