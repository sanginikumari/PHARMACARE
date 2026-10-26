#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>

using namespace std;

class Medicine
{
private:
    int medicineID;
    string name;
    string category;
    double price;
    int quantity;
    string expiryDate;

public:
    // Default constructor
    Medicine();

    // Parameterized constructor
    Medicine(
        int id,
        string n,
        string c,
        double p,
        int q,
        string expiry
    );

    // Display medicine details
    void display();

    // Getter functions
    int getMedicineID();
    string getName();
    string getCategory();
    double getPrice();
    int getQuantity();
    string getExpiryDate();

    // Setter functions
    void setName(string n);
    void setCategory(string c);
    void setPrice(double p);
    void setQuantity(int q);
    void setExpiryDate(string expiry);

    // Stock functions
    void increaseQuantity(int amount);
    bool decreaseQuantity(int amount);
};

#endif