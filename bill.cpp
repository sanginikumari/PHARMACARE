#include "../include/bill.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Default constructor
Bill::Bill()
{
    billNumber = 0;
    customerID = 0;
    customerName = "";
    date = "";
    totalAmount = 0.0;
    head = nullptr;
}

// Parameterized constructor
Bill::Bill(
    int number,
    int cID,
    string cName,
    string billDate)
{
    billNumber = number;
    customerID = cID;
    customerName = cName;
    date = billDate;
    totalAmount = 0.0;
    head = nullptr;
}

// Destructor
Bill::~Bill()
{
    BillItem* current = head;

    while (current != nullptr)
    {
        BillItem* nextNode = current->next;

        delete current;

        current = nextNode;
    }

    head = nullptr;
}

// Add medicine item to linked list
bool Bill::addItem(
    Medicine medicine,
    int quantity)
{
    if (quantity <= 0)
    {
        return false;
    }

    // Create a new linked-list node
    BillItem* newItem =
        new BillItem(medicine, quantity);

    // If list is empty
    if (head == nullptr)
    {
        head = newItem;
    }
    else
    {
        // Move to the last node
        BillItem* current = head;

        while (current->next != nullptr)
        {
            current = current->next;
        }

        // Add new node at the end
        current->next = newItem;
    }

    calculateTotal();

    return true;
}

// Calculate total bill amount
double Bill::calculateTotal() const
{
    totalAmount = 0.0;

    BillItem* current = head;

    while (current != nullptr)
    {
        totalAmount =
            totalAmount + current->getAmount();

        current = current->next;
    }

    return totalAmount;
}

// Display complete bill
void Bill::displayBill() const
{
    calculateTotal();

    cout << "\n";
    cout << "========================================"
         << endl;

    cout << "              PHARMACARE                "
         << endl;

    cout << "       Pharmacy Store Management        "
         << endl;

    cout << "========================================"
         << endl;

    cout << "Bill Number  : "
         << billNumber
         << endl;

    cout << "Customer ID  : "
         << customerID
         << endl;

    cout << "Customer Name: "
         << customerName
         << endl;

    cout << "Date         : "
         << date
         << endl;

    cout << "----------------------------------------"
         << endl;

    cout << left
         << setw(8) << "ID"
         << setw(20) << "Medicine"
         << setw(10) << "Qty"
         << setw(12) << "Price"
         << setw(12) << "Amount"
         << endl;

    cout << "----------------------------------------"
         << endl;

    BillItem* current = head;

    while (current != nullptr)
    {
        Medicine medicine =
            current->getMedicine();

        cout << left
             << setw(8)
             << medicine.getMedicineID()

             << setw(20)
             << medicine.getName()

             << setw(10)
             << current->getQuantity()

             << setw(12)
             << fixed
             << setprecision(2)
             << medicine.getPrice()

             << setw(12)
             << current->getAmount()

             << endl;

        current = current->next;
    }

    cout << "----------------------------------------"
         << endl;

    cout << right
         << setw(35)
         << "Grand Total: Rs. "

         << fixed
         << setprecision(2)
         << totalAmount

         << endl;

    cout << "========================================"
         << endl;
}

// Override Transaction::display()
void Bill::display() const
{
    displayBill();
}

// Override Transaction::getAmount()
double Bill::getAmount() const
{
    return getTotalAmount();
}

// Get bill number
int Bill::getBillNumber() const
{
    return billNumber;
}

// Get customer ID
int Bill::getCustomerID() const
{
    return customerID;
}

// Get customer name
string Bill::getCustomerName() const
{
    return customerName;
}

// Get bill date
string Bill::getDate() const
{
    return date;
}

// Get total amount
double Bill::getTotalAmount() const
{
    calculateTotal();

    return totalAmount;
}

// Get linked-list head
BillItem* Bill::getHead()
{
    return head;
}