#include "../include/salemanager.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Constructor
SaleManager::SaleManager()
{
    count = 0;
    nextSaleID = 1;
}

// Add a new sale
bool SaleManager::addSale(
    int billNumber,
    string date,
    double amount)
{
    if (count >= 500)
    {
        return false;
    }

    if (billNumber <= 0 ||
        amount < 0)
    {
        return false;
    }

    Sale newSale(
        nextSaleID,
        billNumber,
        date,
        amount
    );

    sales[count] = newSale;
    count++;

    nextSaleID++;

    return true;
}

// Add a sale loaded from saved data
bool SaleManager::addLoadedSale(Sale sale)
{
    if (count >= 500)
    {
        return false;
    }

    // Avoid duplicate sale IDs
    if (searchSale(sale.getSaleID()) != nullptr)
    {
        return false;
    }

    sales[count] = sale;
    count++;

    return true;
}

// Search sale by ID
Sale* SaleManager::searchSale(int saleID)
{
    for (int i = 0; i < count; i++)
    {
        if (sales[i].getSaleID() == saleID)
        {
            return &sales[i];
        }
    }

    return nullptr;
}

// Display all sales
void SaleManager::displayAllSales()
{
    if (count == 0)
    {
        cout << "No sales available." << endl;
        return;
    }

    cout << "\n";
    cout << "========================================"
         << endl;
    cout << "              SALES HISTORY             "
         << endl;
    cout << "========================================"
         << endl;

    cout << left
         << setw(10) << "Sale ID"
         << setw(12) << "Bill No."
         << setw(15) << "Date"
         << setw(15) << "Amount"
         << endl;

    cout << "----------------------------------------"
         << endl;

    for (int i = 0; i < count; i++)
    {
        cout << left
             << setw(10)
             << sales[i].getSaleID()

             << setw(12)
             << sales[i].getBillNumber()

             << setw(15)
             << sales[i].getDate()

             << setw(15)
             << fixed
             << setprecision(2)
             << sales[i].getAmount()

             << endl;
    }

    cout << "========================================"
         << endl;
}

// Calculate total income
double SaleManager::calculateTotalIncome()
{
    double total = 0.0;

    for (int i = 0; i < count; i++)
    {
        total =
            total + sales[i].getAmount();
    }

    return total;
}

// Calculate income for a particular date
double SaleManager::calculateDailyIncome(
    string date)
{
    double total = 0.0;

    for (int i = 0; i < count; i++)
    {
        if (sales[i].getDate() == date)
        {
            total =
                total + sales[i].getAmount();
        }
    }

    return total;
}

// Count sales for a particular date
int SaleManager::countDailySales(
    string date)
{
    int dailySales = 0;

    for (int i = 0; i < count; i++)
    {
        if (sales[i].getDate() == date)
        {
            dailySales++;
        }
    }

    return dailySales;
}

// Get total number of sales
int SaleManager::getCount()
{
    return count;
}

// Get sale at array index
Sale* SaleManager::getSaleAt(int index)
{
    if (index < 0 || index >= count)
    {
        return nullptr;
    }

    return &sales[index];
}

// Get next sale ID
int SaleManager::getNextSaleID()
{
    return nextSaleID;
}

// Update next sale ID after loading saved data
void SaleManager::updateNextSaleID()
{
    int highestSaleID = 0;

    for (int i = 0; i < count; i++)
    {
        if (sales[i].getSaleID()
            > highestSaleID)
        {
            highestSaleID =
                sales[i].getSaleID();
        }
    }

    nextSaleID = highestSaleID + 1;
}