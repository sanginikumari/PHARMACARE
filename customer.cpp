#include "../include/customer.h"
#include <iostream>

using namespace std;

// Default constructor
Customer::Customer()
{
    customerID = 0;
    name = "";
    phone = "";
    address = "";
}

// Parameterized constructor
Customer::Customer(
    int id,
    string n,
    string p,
    string a)
{
    customerID = id;
    name = n;
    phone = p;
    address = a;
}

// Display customer details
void Customer::display()
{
    cout << "Customer ID : " << customerID << endl;
    cout << "Name        : " << name << endl;
    cout << "Phone       : " << phone << endl;
    cout << "Address     : " << address << endl;
}

// Getter functions
int Customer::getCustomerID()
{
    return customerID;
}

string Customer::getName()
{
    return name;
}

string Customer::getPhone()
{
    return phone;
}

string Customer::getAddress()
{
    return address;
}

// Setter functions
void Customer::setName(string n)
{
    name = n;
}

void Customer::setPhone(string p)
{
    phone = p;
}

void Customer::setAddress(string a)
{
    address = a;
}