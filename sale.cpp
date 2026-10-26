#include "../include/sale.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Default constructor
Sale::Sale()
{
    saleID = 0;
    billNumber = 0;
    date = "";
    amount = 0.0;
}

// Parameterized constructor
Sale::Sale(
    int id,
    int billNo,
    string saleDate,
    double saleAmount)
{
    saleID = id;
    billNumber = billNo;
    date = saleDate;
    amount = saleAmount;
}

// Display sale information
void Sale::display() const
{
    cout << "Sale ID     : " << saleID << endl;
    cout << "Bill Number : " << billNumber << endl;
    cout << "Date        : " << date << endl;
    cout << "Amount      : Rs. "
         << fixed
         << setprecision(2)
         << amount
         << endl;
}

// Get amount
double Sale::getAmount() const
{
    return amount;
}

// Get sale ID
int Sale::getSaleID() const
{
    return saleID;
}

// Get bill number
int Sale::getBillNumber() const
{
    return billNumber;
}

// Get date
string Sale::getDate() const
{
    return date;
}

// Set sale ID
void Sale::setSaleID(int id)
{
    saleID = id;
}

// Set bill number
void Sale::setBillNumber(int billNo)
{
    billNumber = billNo;
}

// Set date
void Sale::setDate(string saleDate)
{
    date = saleDate;
}

// Set amount
void Sale::setAmount(double saleAmount)
{
    amount = saleAmount;
}