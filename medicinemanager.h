#ifndef MEDICINE_MANAGER_H
#define MEDICINE_MANAGER_H

#include "medicine.h"
#include <string>

using namespace std;

class MedicineManager
{
private:
    // Required DSA: Array
    Medicine medicines[100];

    int count;

public:
    MedicineManager();

    // Add medicine
    bool addMedicine(Medicine medicine);

    // Update medicine
    bool updateMedicine(
        int id,
        string name,
        string category,
        double price,
        int quantity,
        string expiryDate
    );

    // Delete medicine
    bool deleteMedicine(int id);

    // Search medicine by ID
    Medicine* searchMedicine(int id);

    // Search medicine by name
    Medicine* searchMedicine(string name);

    // Reduce stock during billing
    bool reduceStock(int id, int quantity);

    // Increase stock
    bool increaseStock(int id, int quantity);

    // Display all medicines
    void displayAllMedicines();

    // Low-stock alert
    int countLowStock(int limit);

    // Expiry alert
    int countExpiredOrNearExpiry();

    // Get number of medicines
    int getCount();

    // Get medicine at a particular array index
    Medicine* getMedicineAt(int index);
};

#endif