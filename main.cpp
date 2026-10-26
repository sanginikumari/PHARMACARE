#include <iostream>

#include "include/medicinemanager.h"
#include "include/customermanager.h"
#include "include/billmanager.h"
#include "include/salemanager.h"
#include "include/datamanager.h"
#include "include/GUI.h"

using namespace std;

int main()
{
    // Create managers
    MedicineManager medicineManager;
    CustomerManager customerManager;
    BillManager billManager(
        &medicineManager,
        &customerManager
    );
    SaleManager saleManager;

    // Create data manager
    DataManager dataManager;

    // Load saved data
    dataManager.loadAll(
        medicineManager,
        customerManager,
        billManager,
        saleManager
    );

    // Create GUI
    GUI gui(
        &medicineManager,
        &customerManager,
        &billManager,
        &saleManager,
        &dataManager
    );

    // Start GUI
    if (!gui.start())
    {
        cout << "Failed to start PharmaCare GUI."
             << endl;

        return 1;
    }

    return 0;
}