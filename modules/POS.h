#pragma once // Ensure the header is only included once

#include <string>
#include "POSAdmin.h"
#include "POSCashier.h"

class PointOfSale {
private:
    POSAdmin admin;
    POSCashier cashier;

public:
    bool login(string username, string password, string& outRole);
    void adminMenu(string username);
    void managerMenu(string username);
    void cashierMenu(string username);
};