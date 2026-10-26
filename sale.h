#ifndef SALE_H
#define SALE_H

#include "bill.h"

class Sale : public Transaction
{
private:
    int saleID;
    int billNumber;
    string date;
    double amount;

public:
    Sale();

    Sale(
        int saleID,
        int billNumber,
        string date,
        double amount
    );

    int getSaleID() const;
    int getBillNumber() const;
    string getDate() const;
    double getAmount() const;

    void setSaleID(int id);
    void setBillNumber(int billNo);
    void setDate(string saleDate);
    void setAmount(double saleAmount);

    void display() const override;
};

#endif