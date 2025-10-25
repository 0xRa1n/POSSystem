
#include "./include/utilities.h"

// include all the packages that we only need IN THIS FILE
#include <iostream>  // For cout, cin, streamsize
#include <string>    // For the string type
#include <limits>    // For numeric_limits
#include <fstream> // for fstream, ifstream
#include <sstream> // for stringstream

void showHeader(string headerTitle){
    system("cls");
    cout << "---------------------------------" << endl;
    cout << headerTitle << endl ;
    cout << "---------------------------------" << endl;
}

bool handleInputError(){
    if(cin.fail()){
        cin.clear(); // clear the fail state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // the numeric_limits basically ignores everything until the next newline character
        cout << "Invalid input. Please try again." << endl;
        Sleep(1000); // pause for 1 second
        return true; // indicate that there was an error
    }
    return false; // no error
}

int getLastId(string filename) {
    ifstream fin(filename);
    string line;
    int lastId = 0;

    while (getline(fin, line)) {
        if (line.empty()) continue; // skip empty lines
        stringstream ss(line);
        string idStr;

        if (getline(ss, idStr, ',')) {
            try {
                int id = stoi(idStr);
                if (id > lastId) lastId = id; // update lastId if current id is greater
            // if it fails, do nothing
            } catch (...) {}
        }
    }
    // return the last ID found
    return lastId;
}

bool isAlreadyInCsv(string filename, string productNameToCheck) {
    ifstream fin(filename); // create an input file stream
    string line;

    while (getline(fin, line)) {
        if (line.find(productNameToCheck) != string::npos) { // string::npos would return -1 if not found
            return true;  // Product name found somewhere in the database
        }
    }
    return false;
}