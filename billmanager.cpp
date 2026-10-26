#include "../include/billmanager.h"
#include <iostream>

using namespace std;

// Constructor
BillManager::BillManager(
    MedicineManager* medicineManager,
    CustomerManager* customerManager)
{
    count = 0;
    nextBillNumber = 1001;

    this->medicineManager = medicineManager;
    this->customerManager = customerManager;

    // Initialize bill pointers
    for (int i = 0; i < 100; i++)
    {
        bills[i] = nullptr;
    }
}

// Destructor
BillManager::~BillManager()
{
    for (int i = 0; i < count; i++)
    {
        delete bills[i];
        bills[i] = nullptr;
    }
}

// Create a new bill
Bill* BillManager::createBill(
    int customerID,
    string customerName,
    string date)
{
    if (count >= 100)
    {
        return nullptr;
    }

    // Check that customer exists when a real customer ID is provided.
// Customer ID 0 is allowed temporarily while the New Bill form
// is being filled.
if (customerManager != nullptr &&
    customerID > 0)
{
    if (customerManager->searchCustomer(customerID) == nullptr)
    {
        return nullptr;
    }
}

    Bill* newBill =
        new Bill(
            nextBillNumber,
            customerID,
            customerName,
            date
        );

    bills[count] = newBill;
    count++;

    nextBillNumber++;

    return newBill;
}

// Add medicine to a bill
bool BillManager::addMedicineToBill(
    int billNumber,
    int medicineID,
    int quantity)
{
    if (quantity <= 0)
    {
        return false;
    }

    Bill* bill = searchBill(billNumber);

    if (bill == nullptr)
    {
        return false;
    }

    Medicine* medicine =
        medicineManager->searchMedicine(medicineID);

    if (medicine == nullptr)
    {
        return false;
    }

    // Reduce stock only if enough quantity exists
    if (!medicineManager->reduceStock(
            medicineID,
            quantity))
    {
        return false;
    }

    // Add the medicine to the bill linked list
    if (!bill->addItem(
            *medicine,
            quantity))
    {
        // Restore stock if adding item fails
        medicineManager->increaseStock(
            medicineID,
            quantity
        );

        return false;
    }

    return true;
}

// Search bill by bill number
Bill* BillManager::searchBill(int billNumber)
{
    for (int i = 0; i < count; i++)
    {
        if (bills[i] != nullptr &&
            bills[i]->getBillNumber() == billNumber)
        {
            return bills[i];
        }
    }

    return nullptr;
}

// Display all bills
void BillManager::displayAllBills()
{
    if (count == 0)
    {
        cout << "No bills available." << endl;
        return;
    }

    for (int i = 0; i < count; i++)
    {
        if (bills[i] != nullptr)
        {
            cout << "\n========== BILL "
                 << i + 1
                 << " =========="
                 << endl;

            bills[i]->displayBill();
        }
    }
}

// Get total number of bills
int BillManager::getCount()
{
    return count;
}

// Get bill at array index
Bill* BillManager::getBillAt(int index)
{
    if (index < 0 || index >= count)
    {
        return nullptr;
    }

    return bills[index];
}

// Get next bill number
int BillManager::getNextBillNumber()
{
    return nextBillNumber;
}

// Add an already loaded bill
bool BillManager::addLoadedBill(Bill* bill)
{
    if (bill == nullptr)
    {
        return false;
    }

    if (count >= 100)
    {
        return false;
    }

    // Do not allow duplicate bill numbers
    if (searchBill(bill->getBillNumber()) != nullptr)
    {
        return false;
    }

    bills[count] = bill;
    count++;

    return true;
}

// Update next bill number after loading saved data
void BillManager::updateNextBillNumber()
{
    int highestBillNumber = 1000;

    for (int i = 0; i < count; i++)
    {
        if (bills[i] != nullptr)
        {
            if (bills[i]->getBillNumber()
                > highestBillNumber)
            {
                highestBillNumber =
                    bills[i]->getBillNumber();
            }
        }
    }

    nextBillNumber = highestBillNumber + 1;
}