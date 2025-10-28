
#include "./include/utilities.h"

// include all the packages that we only need IN THIS FILE
#include <iostream> 
#include <limits>    // For numeric_limits
#include <fstream> // for fstream, ifstream
#include <sstream> // for stringstream
#include <iomanip> // for setw and left
#include <vector> // for vector

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

    while (getline(fin, line)) { // read each line
        if (line.empty()) continue; // skip empty lines
        stringstream ss(line); // helps us read each line by separating with commas (our delimiter)
        string idStr; // to hold the ID as string

        if (getline(ss, idStr, ',')) { // read until the first comma
            try {
                int id = stoi(idStr); // convert string to integer
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

void readCSV(string database) {
    // Open the file as ifstream, so that we can read it
    ifstream file(database);

    // check if file exists
    if (!file.is_open()) {
        cout << "Failed to open file\n";
        return;
    }

    // Read all rows first
    vector<vector<string>> rows; // 2D vector to hold rows and columns
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row; // this will hold a single row, example output of this is {"ADD", "Product1", "12_31_23", "10:00:00_AM", "AdminUser"} 
        while (getline(ss, cell, ',')) { // split by comma
            row.push_back(cell); // add each cell to the row
        }
        rows.push_back(row); // add the row to the list of rows
    }
    file.close(); // close the file

    // stop if the rows are empty
    if (rows.empty()){
        cout << "No records found in the database.\n";
        return;
    }

    size_t cols = 0; // set the initial column number to 0
    for (auto &r : rows) cols = max(cols, r.size()); // get the maximum number of columns for all rows
    vector<size_t> widths(cols, 0); // using the max column size from the variable cols, use it to initialize widths with 0
    // if there are 6 columns in the csv, then cols = 6 and it will initialize widths to {1, 4, 8, 13, 0, 0}

    // after we get the maximum number, we will update the widths vector
    // this will make sure that each value will not overlap with each other

    for (auto &r : rows) { // for each row
        for (size_t c = 0; c < r.size(); ++c) // for each column in the row
            widths[c] = max(widths[c], r[c].size()); // update max width according to the for loop that determines the maximum number of columns
            // then, it would look like this: {2, 15, 12, 8, 5} for example (it iterates to get the maximum length of each column)
            // there is no curly braces here because it is a single controlled statement

            // it does this for each value in the widths vector
    }

    // Add a little padding for readability
    for (auto &w : widths) w += 2;

    // Print
    for (auto &r : rows) { // for each rows
        for (size_t c = 0; c < r.size(); ++c) {  // for each column in the row
            cout << left << setw(static_cast<int>(widths[c])) << r[c]; // print with padding || static cast is used to convert size_t to int SAFELY
            // additionally, static_cast is used to avoid warnings related to signed/unsigned comparison
        }
        cout << '\n';
    }
}