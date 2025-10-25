#pragma once // Ensure the header is only included once

#include "POSAdmin.h" // include the declaration of POSAdmin class, so that it can be used inside PointOfSale
#include "POSCashier.h" // include the declaration of POSCashier class, so that it can be used inside PointOfSale

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

    // under these menus, the admin, manager, and cashier can access their respective functions
    void adminMenu(string username); 
    void managerMenu(string username);
    void cashierMenu(string username);
};