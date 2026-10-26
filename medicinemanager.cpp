#include "../include/medicinemanager.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

// Constructor
MedicineManager::MedicineManager()
{
    count = 0;
}

// Add medicine
bool MedicineManager::addMedicine(Medicine medicine)
{
    if (count >= 100)
    {
        return false;
    }

    // Do not allow duplicate medicine IDs
    if (searchMedicine(medicine.getMedicineID()) != nullptr)
    {
        return false;
    }

    medicines[count] = medicine;
    count++;

    return true;
}

// Update medicine
bool MedicineManager::updateMedicine(
    int id,
    string name,
    string category,
    double price,
    int quantity,
    string expiryDate)
{
    Medicine* medicine = searchMedicine(id);

    if (medicine == nullptr)
    {
        return false;
    }

    medicine->setName(name);
    medicine->setCategory(category);
    medicine->setPrice(price);
    medicine->setQuantity(quantity);
    medicine->setExpiryDate(expiryDate);

    return true;
}

// Delete medicine
bool MedicineManager::deleteMedicine(int id)
{
    for (int i = 0; i < count; i++)
    {
        if (medicines[i].getMedicineID() == id)
        {
            // Shift remaining medicines to the left
            for (int j = i; j < count - 1; j++)
            {
                medicines[j] = medicines[j + 1];
            }

            count--;

            return true;
        }
    }

    return false;
}

// Search medicine by ID
Medicine* MedicineManager::searchMedicine(int id)
{
    for (int i = 0; i < count; i++)
    {
        if (medicines[i].getMedicineID() == id)
        {
            return &medicines[i];
        }
    }

    return nullptr;
}

// Search medicine by name
Medicine* MedicineManager::searchMedicine(string name)
{
    for (int i = 0; i < count; i++)
    {
        if (medicines[i].getName() == name)
        {
            return &medicines[i];
        }
    }

    return nullptr;
}

// Reduce medicine stock
bool MedicineManager::reduceStock(int id, int quantity)
{
    Medicine* medicine = searchMedicine(id);

    if (medicine == nullptr)
    {
        return false;
    }

    return medicine->decreaseQuantity(quantity);
}

// Increase medicine stock
bool MedicineManager::increaseStock(int id, int quantity)
{
    Medicine* medicine = searchMedicine(id);

    if (medicine == nullptr)
    {
        return false;
    }

    if (quantity <= 0)
    {
        return false;
    }

    medicine->increaseQuantity(quantity);

    return true;
}

// Display all medicines
void MedicineManager::displayAllMedicines()
{
    if (count == 0)
    {
        cout << "No medicines available." << endl;
        return;
    }

    for (int i = 0; i < count; i++)
    {
        cout << "\n----- Medicine "
             << i + 1
             << " -----"
             << endl;

        medicines[i].display();
    }
}

// Count medicines having stock at or below the given limit
int MedicineManager::countLowStock(int limit)
{
    int lowStockCount = 0;

    if (limit < 0)
    {
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        if (medicines[i].getQuantity() <= limit)
        {
            lowStockCount++;
        }
    }

    return lowStockCount;
}

// Convert DD-MM-YYYY into a time value
static time_t convertDateToTime(string date)
{
    int day;
    int month;
    int year;

    char separator1;
    char separator2;

    stringstream ss(date);

    ss >> day >> separator1 >> month >> separator2 >> year;

    if (ss.fail() ||
        separator1 != '-' ||
        separator2 != '-' ||
        day < 1 ||
        day > 31 ||
        month < 1 ||
        month > 12 ||
        year < 1900)
    {
        return static_cast<time_t>(-1);
    }

    tm dateInfo = {};
    dateInfo.tm_mday = day;
    dateInfo.tm_mon = month - 1;
    dateInfo.tm_year = year - 1900;
    dateInfo.tm_hour = 12;

    return mktime(&dateInfo);
}

// Count expired or near-expiry medicines
int MedicineManager::countExpiredOrNearExpiry()
{
    int alertCount = 0;

    time_t currentTime = time(nullptr);

    const int DAYS_FOR_ALERT = 30;
    const double SECONDS_IN_A_DAY = 24.0 * 60.0 * 60.0;

    for (int i = 0; i < count; i++)
    {
        time_t expiryTime =
            convertDateToTime(medicines[i].getExpiryDate());

        if (expiryTime == static_cast<time_t>(-1))
        {
            continue;
        }

        double difference =
            difftime(expiryTime, currentTime);

        double daysRemaining =
            difference / SECONDS_IN_A_DAY;

        if (daysRemaining <= DAYS_FOR_ALERT)
        {
            alertCount++;
        }
    }

    return alertCount;
}

// Get total number of medicines
int MedicineManager::getCount()
{
    return count;
}

// Get medicine at a particular array index
Medicine* MedicineManager::getMedicineAt(int index)
{
    if (index < 0 || index >= count)
    {
        return nullptr;
    }

    return &medicines[index];
}