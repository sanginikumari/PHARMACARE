#ifndef SALE_MANAGER_H
#define SALE_MANAGER_H

#include "sale.h"
#include <string>

using namespace std;

class SaleManager
{
private:
    // Array of sales
    Sale sales[500];

    int count;
    int nextSaleID;

public:
    // Constructor
    SaleManager();

    // Add a new sale
    bool addSale(
        int billNumber,
        string date,
        double amount
    );

    // Add a sale loaded from saved data
    bool addLoadedSale(Sale sale);

    // Search sale by ID
    Sale* searchSale(int saleID);

    // Display all sales
    void displayAllSales();

    // Calculate total income
    double calculateTotalIncome();

    // Calculate income for a particular date
    double calculateDailyIncome(string date);

    // Count sales for a particular date
    int countDailySales(string date);

    // Get total number of sales
    int getCount();

    // Get sale at array index
    Sale* getSaleAt(int index);

    // Get next sale ID
    int getNextSaleID();

    // Update next sale ID after loading data
    void updateNextSaleID();
};

#endif