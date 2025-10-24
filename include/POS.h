#pragma once // Ensure the header is only included once

#include <string>
#include "POSAdmin.h"
#include "POSCashier.h"

class PointOfSale {
private:
    // the class POSAdmin and POSCashier can only be used inside the class PointOfSale
    // all the functions of POSAdmin and POSCashier can only be accessed through PointOfSale
    POSAdmin admin;
    POSCashier cashier;

public:
    // member functions of PointOfSale
    // this can be used outside the class PointOfSale
    bool login(string username, string password, string& outRole);
    void adminMenu(string username);
    void managerMenu(string username);
    void cashierMenu(string username);
};