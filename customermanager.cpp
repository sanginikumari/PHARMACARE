#include "../include/customermanager.h"
#include <iostream>

using namespace std;

// Constructor
CustomerManager::CustomerManager()
{
    count = 0;
}

// Add customer
bool CustomerManager::addCustomer(Customer customer)
{
    if (count >= 100)
    {
        return false;
    }

    // Do not allow duplicate customer IDs
    if (searchCustomer(customer.getCustomerID()) != nullptr)
    {
        return false;
    }

    int newID = customer.getCustomerID();

    // Find the correct position according to Customer ID
    int position = 0;

    while (
        position < count &&
        customers[position].getCustomerID() < newID
    )
    {
        position++;
    }

    // Shift customers to the right
    for (
        int i = count;
        i > position;
        i--
    )
    {
        customers[i] = customers[i - 1];
    }

    // Insert customer at the correct position
    customers[position] = customer;

    count++;

    return true;
}
// Update customer
bool CustomerManager::updateCustomer(
    int id,
    string name,
    string phone,
    string address)
{
    Customer* customer = searchCustomer(id);

    if (customer == nullptr)
    {
        return false;
    }

    customer->setName(name);
    customer->setPhone(phone);
    customer->setAddress(address);

    return true;
}

// Delete customer
bool CustomerManager::deleteCustomer(int id)
{
    for (int i = 0; i < count; i++)
    {
        if (customers[i].getCustomerID() == id)
        {
            // Shift elements to the left
            for (int j = i; j < count - 1; j++)
            {
                customers[j] = customers[j + 1];
            }

            count--;

            return true;
        }
    }

    return false;
}

// Search customer by ID
Customer* CustomerManager::searchCustomer(int id)
{
    for (int i = 0; i < count; i++)
    {
        if (customers[i].getCustomerID() == id)
        {
            return &customers[i];
        }
    }

    return nullptr;
}

// Search customer by name
Customer* CustomerManager::searchCustomer(string name)
{
    for (int i = 0; i < count; i++)
    {
        if (customers[i].getName() == name)
        {
            return &customers[i];
        }
    }

    return nullptr;
}

// Display all customers
void CustomerManager::displayAllCustomers()
{
    if (count == 0)
    {
        cout << "No customers available." << endl;
        return;
    }

    for (int i = 0; i < count; i++)
    {
        cout << "\n----- Customer "
             << i + 1
             << " -----"
             << endl;

        customers[i].display();
    }
}

// Get total number of customers
int CustomerManager::getCount()
{
    return count;
}

// Get customer at a specific array index
Customer* CustomerManager::getCustomerAt(int index)
{
    if (index < 0 || index >= count)
    {
        return nullptr;
    }

    return &customers[index];
}