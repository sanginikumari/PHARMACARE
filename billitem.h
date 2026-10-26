#ifndef BILL_ITEM_H
#define BILL_ITEM_H

#include "medicine.h"

class BillItem
{
private:
    Medicine medicine;
    int quantity;
    double amount;

public:
    // Constructor
    BillItem(Medicine m, int q);

    // Getter functions
    Medicine getMedicine();
    int getQuantity();
    double getAmount();

    // Linked List pointer
    BillItem* next;
};

#endif