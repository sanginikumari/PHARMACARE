#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "customer.h"
#include <string>

using namespace std;

class CustomerManager
{
private:
    // Required DSA: Array
    Customer customers[100];

    int count;

public:
    CustomerManager();

    // Add customer
    bool addCustomer(Customer customer);

    // Update customer
    bool updateCustomer(
        int id,
        string name,
        string phone,
        string address
    );

    // Delete customer
    bool deleteCustomer(int id);

    // Search customer by ID
    Customer* searchCustomer(int id);

    // Search customer by name
    Customer* searchCustomer(string name);

    // Display all customers
    void displayAllCustomers();

    // Get number of customers
    int getCount();

    // Access customer by array index
    Customer* getCustomerAt(int index);
};

#endif