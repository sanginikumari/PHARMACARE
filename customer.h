#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

using namespace std;

class Customer
{
private:
    int customerID;
    string name;
    string phone;
    string address;

public:
    // Default constructor
    Customer();

    // Parameterized constructor
    Customer(
        int id,
        string n,
        string p,
        string a
    );

    // Display customer details
    void display();

    // Getter functions
    int getCustomerID();
    string getName();
    string getPhone();
    string getAddress();

    // Setter functions
    void setName(string n);
    void setPhone(string p);
    void setAddress(string a);
};

#endif