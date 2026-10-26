#include "../include/datamanager.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

// Constructor
DataManager::DataManager()
{
    medicinesFile = "data/medicines.txt";
    customersFile = "data/customers.txt";
    billsFile = "data/bills.txt";
    billItemsFile = "data/bill_items.txt";
    salesFile = "data/sales.txt";
}

// Save all application data
bool DataManager::saveAll(
    MedicineManager& medicineManager,
    CustomerManager& customerManager,
    BillManager& billManager,
    SaleManager& saleManager)
{
    bool medicinesSaved =
        saveMedicines(medicineManager);

    bool customersSaved =
        saveCustomers(customerManager);

    bool billsSaved =
        saveBills(billManager);

    bool billItemsSaved =
        saveBillItems(billManager);

    bool salesSaved =
        saveSales(saleManager);

    return medicinesSaved &&
           customersSaved &&
           billsSaved &&
           billItemsSaved &&
           salesSaved;
}

// Load all application data
bool DataManager::loadAll(
    MedicineManager& medicineManager,
    CustomerManager& customerManager,
    BillManager& billManager,
    SaleManager& saleManager)
{
    bool medicinesLoaded =
        loadMedicines(medicineManager);

    bool customersLoaded =
        loadCustomers(customerManager);

    bool billsLoaded =
        loadBills(billManager);

    bool billItemsLoaded =
        loadBillItems(
            billManager,
            medicineManager
        );

    bool salesLoaded =
        loadSales(saleManager);

    billManager.updateNextBillNumber();
    saleManager.updateNextSaleID();

    return medicinesLoaded &&
           customersLoaded &&
           billsLoaded &&
           billItemsLoaded &&
           salesLoaded;
}

// ======================================================
// MEDICINES
// ======================================================

bool DataManager::saveMedicines(
    MedicineManager& medicineManager)
{
    ofstream file(
        medicinesFile.c_str()
    );

    if (!file.is_open())
    {
        return false;
    }

    for (int i = 0;
         i < medicineManager.getCount();
         i++)
    {
        Medicine* medicine =
            medicineManager.getMedicineAt(i);

        if (medicine != nullptr)
        {
            file
                << medicine->getMedicineID()
                << "|"
                << medicine->getName()
                << "|"
                << medicine->getCategory()
                << "|"
                << medicine->getPrice()
                << "|"
                << medicine->getQuantity()
                << "|"
                << medicine->getExpiryDate()
                << endl;
        }
    }

    file.close();

    return true;
}

bool DataManager::loadMedicines(
    MedicineManager& medicineManager)
{
    ifstream file(
        medicinesFile.c_str()
    );

    if (!file.is_open())
    {
        return true;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string idText;
        string name;
        string category;
        string priceText;
        string quantityText;
        string expiryDate;

        getline(ss, idText, '|');
        getline(ss, name, '|');
        getline(ss, category, '|');
        getline(ss, priceText, '|');
        getline(ss, quantityText, '|');
        getline(ss, expiryDate, '|');

        if (idText.empty())
        {
            continue;
        }

        int id =
            atoi(idText.c_str());

        double price =
            atof(priceText.c_str());

        int quantity =
            atoi(quantityText.c_str());

        Medicine medicine(
            id,
            name,
            category,
            price,
            quantity,
            expiryDate
        );

        medicineManager.addMedicine(
            medicine
        );
    }

    file.close();

    return true;
}

// ======================================================
// CUSTOMERS
// ======================================================

bool DataManager::saveCustomers(
    CustomerManager& customerManager)
{
    ofstream file(
        customersFile.c_str()
    );

    if (!file.is_open())
    {
        return false;
    }

    for (int i = 0;
         i < customerManager.getCount();
         i++)
    {
        Customer* customer =
            customerManager.getCustomerAt(i);

        if (customer != nullptr)
        {
            file
                << customer->getCustomerID()
                << "|"
                << customer->getName()
                << "|"
                << customer->getPhone()
                << "|"
                << customer->getAddress()
                << endl;
        }
    }

    file.close();

    return true;
}

bool DataManager::loadCustomers(
    CustomerManager& customerManager)
{
    ifstream file(
        customersFile.c_str()
    );

    if (!file.is_open())
    {
        return true;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string idText;
        string name;
        string phone;
        string address;

        getline(ss, idText, '|');
        getline(ss, name, '|');
        getline(ss, phone, '|');
        getline(ss, address, '|');

        if (idText.empty())
        {
            continue;
        }

        int id =
            atoi(idText.c_str());

        Customer customer(
            id,
            name,
            phone,
            address
        );

        customerManager.addCustomer(
            customer
        );
    }

    file.close();

    return true;
}

// ======================================================
// BILLS
// ======================================================

bool DataManager::saveBills(
    BillManager& billManager)
{
    ofstream file(
        billsFile.c_str()
    );

    if (!file.is_open())
    {
        return false;
    }

    for (int i = 0;
         i < billManager.getCount();
         i++)
    {
        Bill* bill =
            billManager.getBillAt(i);

        if (bill != nullptr)
        {
            file
                << bill->getBillNumber()
                << "|"
                << bill->getCustomerID()
                << "|"
                << bill->getCustomerName()
                << "|"
                << bill->getDate()
                << "|"
                << bill->getTotalAmount()
                << endl;
        }
    }

    file.close();

    return true;
}

bool DataManager::loadBills(
    BillManager& billManager)
{
    ifstream file(
        billsFile.c_str()
    );

    if (!file.is_open())
    {
        return true;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string billNumberText;
        string customerIDText;
        string customerName;
        string date;
        string amountText;

        getline(
            ss,
            billNumberText,
            '|'
        );

        getline(
            ss,
            customerIDText,
            '|'
        );

        getline(
            ss,
            customerName,
            '|'
        );

        getline(
            ss,
            date,
            '|'
        );

        getline(
            ss,
            amountText,
            '|'
        );

        if (billNumberText.empty())
        {
            continue;
        }

        int billNumber =
            atoi(
                billNumberText.c_str()
            );

        int customerID =
            atoi(
                customerIDText.c_str()
            );

        Bill* bill =
            new Bill(
                billNumber,
                customerID,
                customerName,
                date
            );

        if (!billManager.addLoadedBill(bill))
        {
            delete bill;
        }
    }

    file.close();

    return true;
}

// ======================================================
// BILL ITEMS
// ======================================================

bool DataManager::saveBillItems(
    BillManager& billManager)
{
    ofstream file(
        billItemsFile.c_str()
    );

    if (!file.is_open())
    {
        return false;
    }

    for (int i = 0;
         i < billManager.getCount();
         i++)
    {
        Bill* bill =
            billManager.getBillAt(i);

        if (bill == nullptr)
        {
            continue;
        }

        BillItem* current =
            bill->getHead();

        while (current != nullptr)
        {
            Medicine medicine =
                current->getMedicine();

            file
                << bill->getBillNumber()
                << "|"
                << medicine.getMedicineID()
                << "|"
                << current->getQuantity()
                << endl;

            current = current->next;
        }
    }

    file.close();

    return true;
}

bool DataManager::loadBillItems(
    BillManager& billManager,
    MedicineManager& medicineManager)
{
    ifstream file(
        billItemsFile.c_str()
    );

    if (!file.is_open())
    {
        return true;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string billNumberText;
        string medicineIDText;
        string quantityText;

        getline(
            ss,
            billNumberText,
            '|'
        );

        getline(
            ss,
            medicineIDText,
            '|'
        );

        getline(
            ss,
            quantityText,
            '|'
        );

        if (billNumberText.empty() ||
            medicineIDText.empty() ||
            quantityText.empty())
        {
            continue;
        }

        int billNumber =
            atoi(
                billNumberText.c_str()
            );

        int medicineID =
            atoi(
                medicineIDText.c_str()
            );

        int quantity =
            atoi(
                quantityText.c_str()
            );

        if (quantity <= 0)
        {
            continue;
        }

        Bill* bill =
            billManager.searchBill(
                billNumber
            );

        if (bill == nullptr)
        {
            continue;
        }

        Medicine* medicine =
            medicineManager.searchMedicine(
                medicineID
            );

        if (medicine == nullptr)
        {
            continue;
        }

        // Reconstruct the linked-list item.
        // Stock is NOT reduced here because the
        // current stock has already been loaded
        // from medicines.txt.
        bill->addItem(
            *medicine,
            quantity
        );
    }

    file.close();

    return true;
}

// ======================================================
// SALES
// ======================================================

bool DataManager::saveSales(
    SaleManager& saleManager)
{
    ofstream file(
        salesFile.c_str()
    );

    if (!file.is_open())
    {
        return false;
    }

    for (int i = 0;
         i < saleManager.getCount();
         i++)
    {
        Sale* sale =
            saleManager.getSaleAt(i);

        if (sale != nullptr)
        {
            file
                << sale->getSaleID()
                << "|"
                << sale->getBillNumber()
                << "|"
                << sale->getDate()
                << "|"
                << sale->getAmount()
                << endl;
        }
    }

    file.close();

    return true;
}

bool DataManager::loadSales(
    SaleManager& saleManager)
{
    ifstream file(
        salesFile.c_str()
    );

    if (!file.is_open())
    {
        return true;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string saleIDText;
        string billNumberText;
        string date;
        string amountText;

        getline(
            ss,
            saleIDText,
            '|'
        );

        getline(
            ss,
            billNumberText,
            '|'
        );

        getline(
            ss,
            date,
            '|'
        );

        getline(
            ss,
            amountText,
            '|'
        );

        if (saleIDText.empty())
        {
            continue;
        }

        int saleID =
            atoi(
                saleIDText.c_str()
            );

        int billNumber =
            atoi(
                billNumberText.c_str()
            );

        double amount =
            atof(
                amountText.c_str()
            );

        Sale sale(
            saleID,
            billNumber,
            date,
            amount
        );

        saleManager.addLoadedSale(
            sale
        );
    }

    file.close();

    return true;
}