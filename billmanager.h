#ifndef BILL_MANAGER_H
#define BILL_MANAGER_H

#include <bill.h>
#include <medicinemanager.h>
#include <customermanager.h>

class BillManager
{
private:
    // Dynamic array of Bill pointers
    // This avoids shallow-copy problems caused by Bill's linked list.
    Bill* bills[100];

    int count;
    int nextBillNumber;

    MedicineManager* medicineManager;
    CustomerManager* customerManager;

public:
    // Constructor
    BillManager(
        MedicineManager* medicineManager,
        CustomerManager* customerManager
    );

    // Destructor
    ~BillManager();

    // Create a new bill
    Bill* createBill(
        int customerID,
        string customerName,
        string date
    );

    // Add medicine to a bill
    bool addMedicineToBill(
        int billNumber,
        int medicineID,
        int quantity
    );

    // Find bill by bill number
    Bill* searchBill(int billNumber);

    // Display all bills
    void displayAllBills();

    // Get number of bills
    int getCount();

    // Get bill at array index
    Bill* getBillAt(int index);

    // Get next bill number
    int getNextBillNumber();

    // Add an already loaded bill from file
    bool addLoadedBill(Bill* bill);

    // Update next bill number after loading saved data
    void updateNextBillNumber();
};

#endif