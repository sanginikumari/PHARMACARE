#include "../include/medicine.h"
#include <iostream>

using namespace std;

// Default constructor
Medicine::Medicine()
{
    medicineID = 0;
    name = "";
    category = "";
    price = 0.0;
    quantity = 0;
    expiryDate = "";
}

// Parameterized constructor
Medicine::Medicine(
    int id,
    string n,
    string c,
    double p,
    int q,
    string expiry)
{
    medicineID = id;
    name = n;
    category = c;
    price = p;
    quantity = q;
    expiryDate = expiry;
}

// Display medicine details
void Medicine::display()
{
    cout << "Medicine ID : " << medicineID << endl;
    cout << "Name        : " << name << endl;
    cout << "Category    : " << category << endl;
    cout << "Price       : " << price << endl;
    cout << "Quantity    : " << quantity << endl;
    cout << "Expiry Date : " << expiryDate << endl;
}

// Getter functions
int Medicine::getMedicineID()
{
    return medicineID;
}

string Medicine::getName()
{
    return name;
}

string Medicine::getCategory()
{
    return category;
}

double Medicine::getPrice()
{
    return price;
}

int Medicine::getQuantity()
{
    return quantity;
}

string Medicine::getExpiryDate()
{
    return expiryDate;
}

// Setter functions
void Medicine::setName(string n)
{
    name = n;
}

void Medicine::setCategory(string c)
{
    category = c;
}

void Medicine::setPrice(double p)
{
    price = p;
}

void Medicine::setQuantity(int q)
{
    quantity = q;
}

void Medicine::setExpiryDate(string expiry)
{
    expiryDate = expiry;
}

// Increase stock
void Medicine::increaseQuantity(int amount)
{
    if (amount > 0)
    {
        quantity = quantity + amount;
    }
}

// Decrease stock
bool Medicine::decreaseQuantity(int amount)
{
    if (amount <= 0)
    {
        return false;
    }

    if (amount > quantity)
    {
        return false;
    }

    quantity = quantity - amount;

    return true;
}