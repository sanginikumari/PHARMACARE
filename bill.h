#ifndef BILL_H
#define BILL_H

#include <string>
#include "billitem.h"

using namespace std;

class Transaction
{
public:
    virtual void display() const = 0;
    virtual double getAmount() const = 0;

    virtual ~Transaction()
    {
    }
};

class Bill : public Transaction
{
private:
    int billNumber;
    int customerID;
    string customerName;
    string date;

    mutable double totalAmount;

    BillItem* head;

public:
    Bill();

    Bill(
        int number,
        int cID,
        string cName,
        string billDate
    );

    ~Bill();

    bool addItem(
        Medicine medicine,
        int quantity
    );

    double calculateTotal() const;

    void displayBill() const;

    void display() const override;

    double getAmount() const override;

    int getBillNumber() const;
    int getCustomerID() const;
    string getCustomerName() const;
    string getDate() const;
    double getTotalAmount() const;

    BillItem* getHead();
};

#endif