#include "../include/billitem.h"

// Constructor
BillItem::BillItem(Medicine m, int q)
{
    medicine = m;
    quantity = q;

    // Calculate amount for this medicine
    amount = medicine.getPrice() * quantity;

    // Next node is initially empty
    next = nullptr;
}

// Get medicine
Medicine BillItem::getMedicine()
{
    return medicine;
}

// Get purchased quantity
int BillItem::getQuantity()
{
    return quantity;
}

// Get total amount for this item
double BillItem::getAmount()
{
    return amount;
}