#ifndef GUI_H
#define GUI_H

#include <windows.h>
#include <string>

#include "medicinemanager.h"
#include "customermanager.h"
#include "billmanager.h"
#include "salemanager.h"
#include "datamanager.h"

using namespace std;

class GUI
{
private:
    // Main application window
    HWND mainWindow;

    // Managers
    MedicineManager* medicineManager;
    CustomerManager* customerManager;
    BillManager* billManager;
    SaleManager* saleManager;
    DataManager* dataManager;

    // Current screen
    int currentScreen;

    // Current bill
    Bill* currentBill;

    // Main window procedure
    static LRESULT CALLBACK WindowProcedure(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    // Handle window messages
    LRESULT handleMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    // Create main dashboard
    void createDashboard();

    // Create navigation buttons
    void createNavigation();

    // Draw complete interface
    void drawInterface(
        HDC deviceContext
    );

    // Draw dashboard
    void drawDashboard(
        HDC deviceContext
    );

    // Draw medicine screen
    void drawMedicineScreen(
        HDC deviceContext
    );

    // Draw customer screen
    void drawCustomerScreen(
        HDC deviceContext
    );

    // Draw billing screen
    void drawBillingScreen(
        HDC deviceContext
    );

    // Draw sales screen
    void drawSalesScreen(
        HDC deviceContext
    );

    // Draw reports screen
    void drawReportsScreen(
        HDC deviceContext
    );

    // Draw alerts
    void drawAlerts(
        HDC deviceContext
    );

    // Show message box
    void showMessage(
        string title,
        string message
    );

    // Medicine operations
    void addMedicineWindow();
    void updateMedicineWindow();
    void deleteMedicineWindow();
    void searchMedicineWindow();

    // Customer operations
    void addCustomerWindow();
    void updateCustomerWindow();
    void deleteCustomerWindow();
    void searchCustomerWindow();

    // Billing operations
    void createNewBill();
    void addMedicineToCurrentBill();
    void showCurrentBill();

    // Sales/report operations
    void showSalesHistory();
    void showDailySales();

    // Get current date
    string getCurrentDate();

    // Convert string to integer
    int stringToInteger(
        string value
    );

    // Convert string to double
    double stringToDouble(
        string value
    );

public:
    // Constructor
    GUI(
        MedicineManager* medicineManager,
        CustomerManager* customerManager,
        BillManager* billManager,
        SaleManager* saleManager,
        DataManager* dataManager
    );

    // Start GUI application
    bool start();

    // Save data
    void saveData();

    // Main window accessor
    HWND getMainWindow() const { return mainWindow; }
};

#endif