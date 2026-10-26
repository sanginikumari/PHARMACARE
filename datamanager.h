#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "medicinemanager.h"
#include "customermanager.h"
#include "billmanager.h"
#include "salemanager.h"
#include <string>

using namespace std;

class DataManager
{
private:
    string medicinesFile;
    string customersFile;
    string billsFile;
    string billItemsFile;
    string salesFile;

public:
    // Constructor
    DataManager();

    // Save all application data
    bool saveAll(
        MedicineManager& medicineManager,
        CustomerManager& customerManager,
        BillManager& billManager,
        SaleManager& saleManager
    );

    // Load all application data
    bool loadAll(
        MedicineManager& medicineManager,
        CustomerManager& customerManager,
        BillManager& billManager,
        SaleManager& saleManager
    );

private:
    // Medicine data
    bool saveMedicines(
        MedicineManager& medicineManager
    );

    bool loadMedicines(
        MedicineManager& medicineManager
    );

    // Customer data
    bool saveCustomers(
        CustomerManager& customerManager
    );

    bool loadCustomers(
        CustomerManager& customerManager
    );

    // Bill data
    bool saveBills(
        BillManager& billManager
    );

    bool loadBills(
        BillManager& billManager
    );

    // Bill item data
    bool saveBillItems(
        BillManager& billManager
    );

    bool loadBillItems(
        BillManager& billManager,
        MedicineManager& medicineManager
    );

    // Sales data
    bool saveSales(
        SaleManager& saleManager
    );

    bool loadSales(
        SaleManager& saleManager
    );
};

#endif