#include "GUI.h"

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

using namespace std;

/* =========================================================
   GLOBAL GUI COLORS
   ========================================================= */

static COLORREF GUI_BACKGROUND = RGB(245, 248, 252);
static COLORREF GUI_WHITE      = RGB(255, 255, 255);
static COLORREF GUI_PRIMARY    = RGB(48, 86, 211);
static COLORREF GUI_PRIMARY2   = RGB(74, 108, 224);
static COLORREF GUI_GREEN      = RGB(34, 166, 120);
static COLORREF GUI_ORANGE     = RGB(242, 153, 74);
static COLORREF GUI_RED        = RGB(220, 80, 80);
static COLORREF GUI_PURPLE     = RGB(126, 87, 194);
static COLORREF GUI_TEXT       = RGB(45, 55, 72);
static COLORREF GUI_GRAY       = RGB(110, 120, 135);
static COLORREF GUI_LIGHTGRAY  = RGB(225, 230, 238);
static COLORREF GUI_SIDEBAR    = RGB(250, 252, 255);


/* =========================================================
   GLOBAL GUI STATE
   ========================================================= */

static GUI* g_gui = NULL;

static MedicineManager* g_medicineManager = NULL;
static CustomerManager* g_customerManager = NULL;
static BillManager* g_billManager = NULL;
static SaleManager* g_saleManager = NULL;
static DataManager* g_dataManager = NULL;


/* =========================================================
   MAIN WINDOW CONTROL IDs
   ========================================================= */

#define ID_NAV_DASHBOARD   1001
#define ID_NAV_MEDICINE    1002
#define ID_NAV_CUSTOMER    1003
#define ID_NAV_BILLING     1004
#define ID_NAV_SALES       1005
#define ID_NAV_REPORTS     1006
#define ID_NAV_ALERTS      1007
#define ID_NAV_EXIT        1008

#define ID_MED_ADD         1101
#define ID_MED_UPDATE      1102
#define ID_MED_DELETE      1103
#define ID_MED_SEARCH      1104
#define ID_MED_LIST        1105
#define ID_MED_BACK        1106
#define ID_CUST_ADD        1111
#define ID_CUST_UPDATE     1112
#define ID_CUST_DELETE     1113
#define ID_CUST_SEARCH     1114
#define ID_CUST_LIST       1115
#define ID_CUST_BACK       1116

#define ID_DASH_ADD_MED    1201
#define ID_DASH_ADD_CUST   1202
#define ID_DASH_NEW_BILL   1203
#define ID_DASH_REPORT     1204
#define ID_BILL_NEW        1301
#define ID_BILL_HISTORY    1302
#define ID_BILL_REFRESH    1303
#define ID_BILL_BACK       1304

#define ID_SALES_REFRESH   1401
#define ID_SALES_DAILY     1402
#define ID_SALES_HISTORY   1403
#define ID_SALES_BACK      1404


/* =========================================================
   GLOBAL MAIN WINDOW BUTTON HANDLES
   ========================================================= */

static HWND hNavDashboard = NULL;
static HWND hNavMedicine  = NULL;
static HWND hNavCustomer  = NULL;
static HWND hNavBilling   = NULL;
static HWND hNavSales     = NULL;
static HWND hNavReports   = NULL;
static HWND hNavAlerts    = NULL;
static HWND hNavExit      = NULL;

static HWND hMedAdd       = NULL;
static HWND hMedUpdate    = NULL;
static HWND hMedDelete    = NULL;
static HWND hMedSearch    = NULL;
static HWND hMedList      = NULL;
static HWND hMedBack      = NULL;
static HWND hCustAdd      = NULL;
static HWND hCustUpdate   = NULL;
static HWND hCustDelete   = NULL;
static HWND hCustSearch   = NULL;
static HWND hCustList     = NULL;
static HWND hCustBack     = NULL;
static HWND hBillNew      = NULL;
static HWND hBillHistory  = NULL;
static HWND hBillRefresh  = NULL;
static HWND hBillBack     = NULL;

static HWND hSalesRefresh = NULL;
static HWND hSalesDaily   = NULL;
static HWND hSalesHistory = NULL;
static HWND hSalesBack    = NULL;

static HWND hDashAddMed   = NULL;
static HWND hDashAddCust  = NULL;
static HWND hDashNewBill  = NULL;
static HWND hDashReport   = NULL;


/* =========================================================
   FORWARD DECLARATIONS FOR MODULE WINDOWS
   ========================================================= */

static void openMedicineAddWindow();
static void openMedicineSearchWindow();
static void openMedicineUpdateWindow();
static void openMedicineDeleteWindow();

static void openCustomerAddWindow();
static void openCustomerSearchWindow();
static void openCustomerUpdateWindow();
static void openCustomerDeleteWindow();

static void hideMainControls();

static void showMainControlsForScreen(int screen);
static HWND createMainButton(
    HWND parent,
    const char* text,
    int id,
    int x,
    int y,
    int width,
    int height
);

static void setFont(
    HWND control,
    int size,
    bool bold
);

static void drawText(
    HDC dc,
    const char* text,
    int x,
    int y,
    int width,
    int height,
    COLORREF color,
    int size,
    bool bold
);

static void drawFilledRect(
    HDC dc,
    int left,
    int top,
    int right,
    int bottom,
    COLORREF color
);

static void drawCard(
    HDC dc,
    int left,
    int top,
    int right,
    int bottom,
    COLORREF color
);


/* =========================================================
   CONSTRUCTOR
   ========================================================= */

GUI::GUI(
    MedicineManager* medicineManager,
    CustomerManager* customerManager,
    BillManager* billManager,
    SaleManager* saleManager,
    DataManager* dataManager
)
{
    mainWindow = NULL;

    this->medicineManager = medicineManager;
    this->customerManager = customerManager;
    this->billManager = billManager;
    this->saleManager = saleManager;
    this->dataManager = dataManager;

    currentScreen = 0;
    currentBill = NULL;

    g_gui = this;

    g_medicineManager = medicineManager;
    g_customerManager = customerManager;
    g_billManager = billManager;
    g_saleManager = saleManager;
    g_dataManager = dataManager;
}


/* =========================================================
   START GUI
   ========================================================= */

bool GUI::start()
{
    HINSTANCE instance = GetModuleHandleA(NULL);

    WNDCLASSA windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));

    windowClass.lpfnWndProc = GUI::WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "PharmaCareMainWindow";
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground =
        CreateSolidBrush(GUI_BACKGROUND);

    if (!RegisterClassA(&windowClass))
    {
        DWORD errorCode = GetLastError();

        if (errorCode != ERROR_CLASS_ALREADY_EXISTS)
        {
            char message[200];

            sprintf(
                message,
                "RegisterClassA failed.\nError code: %lu",
                errorCode
            );

            MessageBoxA(
                NULL,
                message,
                "PharmaCare Error",
                MB_OK | MB_ICONERROR
            );

            return false;
        }
    }

    mainWindow = CreateWindowExA(
        0,
        "PharmaCareMainWindow",
        "PharmaCare - Pharmacy Store Management",
        WS_OVERLAPPEDWINDOW,
        100,
        50,
        1250,
        760,
        NULL,
        NULL,
        instance,
        this
    );

    if (mainWindow == NULL)
    {
        DWORD errorCode = GetLastError();

        char message[200];

        sprintf(
            message,
            "CreateWindowExA failed.\nError code: %lu",
            errorCode
        );

        MessageBoxA(
            NULL,
            message,
            "PharmaCare Error",
            MB_OK | MB_ICONERROR
        );

        return false;
    }

    ShowWindow(mainWindow, SW_SHOW);
    UpdateWindow(mainWindow);

    MSG message;

    while (GetMessageA(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
}
/* =========================================================
   WINDOW PROCEDURE
   ========================================================= */

LRESULT CALLBACK GUI::WindowProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    GUI* gui = NULL;

    if (message == WM_NCCREATE)
    {
        CREATESTRUCTA* createData =
            reinterpret_cast<CREATESTRUCTA*>(lParam);

            gui =
            reinterpret_cast<GUI*>(createData->lpCreateParams);

        if (gui != NULL)
        {
            gui->mainWindow = window;
        }

        SetWindowLongPtrA(
            window,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(gui)
        );
    }
    else
    {
        gui =
            reinterpret_cast<GUI*>(
                GetWindowLongPtrA(window, GWLP_USERDATA)
            );
    }

    if (gui != NULL)
    {
        return gui->handleMessage(
            message,
            wParam,
            lParam
        );
    }

    return DefWindowProcA(
        window,
        message,
        wParam,
        lParam
    );
}


/* =========================================================
   MAIN WINDOW MESSAGE HANDLER
   ========================================================= */

LRESULT GUI::handleMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            createNavigation();
            createDashboard();

            showMainControlsForScreen(currentScreen);

            return 0;
        }


        case WM_COMMAND:
        {
            int controlID = LOWORD(wParam);

            switch (controlID)
            {
                /* ---------------- DASHBOARD ---------------- */

                case ID_NAV_DASHBOARD:
                    currentScreen = 0;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;


                /* ---------------- MEDICINE ---------------- */

                case ID_NAV_MEDICINE:
                    currentScreen = 1;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;

                case ID_MED_ADD:
                    addMedicineWindow();
                    return 0;

                case ID_MED_UPDATE:
                    updateMedicineWindow();
                    return 0;

                case ID_MED_DELETE:
                    deleteMedicineWindow();
                    return 0;

                case ID_MED_SEARCH:
                    searchMedicineWindow();
                    return 0;

                case ID_MED_LIST:
                    currentScreen = 1;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;

                case ID_MED_BACK:
                    currentScreen = 0;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;


                /* ---------------- CUSTOMER ---------------- */

                case ID_NAV_CUSTOMER:
                    currentScreen = 2;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;
                case ID_CUST_ADD:
                    addCustomerWindow();
                    return 0;

                case ID_CUST_UPDATE:
                    updateCustomerWindow();
                    return 0;

                case ID_CUST_DELETE:
                    deleteCustomerWindow();
                    return 0;

                case ID_CUST_SEARCH:
                    searchCustomerWindow();
                    return 0;

                case ID_CUST_LIST:
                    currentScreen = 2;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;

                case ID_CUST_BACK:
                    currentScreen = 0;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;    


                /* ---------------- BILLING ---------------- */

                case ID_NAV_BILLING:
                    currentScreen = 3;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;
                case ID_BILL_NEW:
                    createNewBill();
                    return 0;

                case ID_BILL_HISTORY:
                    showSalesHistory();
                    return 0;

                case ID_BILL_REFRESH:
                    currentScreen = 3;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;

                case ID_BILL_BACK:
                    currentScreen = 0;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;    


                /* ---------------- SALES ---------------- */

                case ID_NAV_SALES:
                    currentScreen = 4;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;

                case ID_SALES_REFRESH:
                    currentScreen = 4;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;

                case ID_SALES_DAILY:
                    showDailySales();
                    return 0;

                case ID_SALES_HISTORY:
                    showSalesHistory();
                    return 0;

                case ID_SALES_BACK:
                    currentScreen = 0;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;


                /* ---------------- REPORTS ---------------- */

                case ID_NAV_REPORTS:
                    currentScreen = 5;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;


                /* ---------------- ALERTS ---------------- */

                case ID_NAV_ALERTS:
                    currentScreen = 6;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;


                /* ---------------- EXIT ---------------- */

                case ID_NAV_EXIT:
                    saveData();
                    DestroyWindow(mainWindow);
                    return 0;


                /* ---------------- DASHBOARD QUICK ACTIONS ---------------- */

                case ID_DASH_ADD_MED:
                    addMedicineWindow();
                    return 0;

                case ID_DASH_ADD_CUST:
                    addCustomerWindow();
                    return 0;

                case ID_DASH_NEW_BILL:
                    createNewBill();
                    return 0;

                case ID_DASH_REPORT:
                    currentScreen = 5;
                    showMainControlsForScreen(currentScreen);
                    InvalidateRect(mainWindow, NULL, TRUE);
                    return 0;
            }

            break;
        }


        case WM_PAINT:
        {
            PAINTSTRUCT paintStruct;

            HDC deviceContext =
                BeginPaint(mainWindow, &paintStruct);

            drawInterface(deviceContext);

            EndPaint(mainWindow, &paintStruct);

            return 0;
        }


        case WM_ERASEBKGND:
            return 1;


        case WM_DESTROY:
        {
            saveData();

            PostQuitMessage(0);

            return 0;
        }
    }

    return DefWindowProcA(
        mainWindow,
        message,
        wParam,
        lParam
    );
}


/* =========================================================
   CREATE NAVIGATION
   ========================================================= */

void GUI::createNavigation()
{
    hNavDashboard = createMainButton(
        mainWindow,
        "Dashboard",
        ID_NAV_DASHBOARD,
        20,
        110,
        190,
        42
    );

    hNavMedicine = createMainButton(
        mainWindow,
        "Medicines",
        ID_NAV_MEDICINE,
        20,
        160,
        190,
        42
    );

    hNavCustomer = createMainButton(
        mainWindow,
        "Customers",
        ID_NAV_CUSTOMER,
        20,
        210,
        190,
        42
    );

    hNavBilling = createMainButton(
        mainWindow,
        "Billing",
        ID_NAV_BILLING,
        20,
        260,
        190,
        42
    );

    hNavSales = createMainButton(
        mainWindow,
        "Sales",
        ID_NAV_SALES,
        20,
        310,
        190,
        42
    );

    hNavReports = createMainButton(
        mainWindow,
        "Reports",
        ID_NAV_REPORTS,
        20,
        360,
        190,
        42
    );

    hNavAlerts = createMainButton(
        mainWindow,
        "Alerts",
        ID_NAV_ALERTS,
        20,
        410,
        190,
        42
    );

    hNavExit = createMainButton(
        mainWindow,
        "Save && Exit",
        ID_NAV_EXIT,
        20,
        630,
        190,
        42
    );


    /* Medicine action buttons */

    hMedAdd = createMainButton(
        mainWindow,
        "Add Medicine",
        ID_MED_ADD,
        250,
        125,
        145,
        40
    );

    hMedUpdate = createMainButton(
        mainWindow,
        "Update",
        ID_MED_UPDATE,
        405,
        125,
        125,
        40
    );

    hMedDelete = createMainButton(
        mainWindow,
        "Delete",
        ID_MED_DELETE,
        540,
        125,
        125,
        40
    );

    hMedSearch = createMainButton(
        mainWindow,
        "Search",
        ID_MED_SEARCH,
        675,
        125,
        125,
        40
    );

    hMedList = createMainButton(
        mainWindow,
        "Refresh List",
        ID_MED_LIST,
        810,
        125,
        125,
        40
    );

    hMedBack = createMainButton(
        mainWindow,
        "Back",
        ID_MED_BACK,
        945,
        125,
        100,
        40
    );
    /* Customer action buttons */

    hCustAdd = createMainButton(
        mainWindow,
        "Add Customer",
        ID_CUST_ADD,
        250,
        125,
        145,
        40
    );

    hCustUpdate = createMainButton(
        mainWindow,
        "Update",
        ID_CUST_UPDATE,
        405,
        125,
        125,
        40
    );

    hCustDelete = createMainButton(
        mainWindow,
        "Delete",
        ID_CUST_DELETE,
        540,
        125,
        125,
        40
    );

    hCustSearch = createMainButton(
        mainWindow,
        "Search",
        ID_CUST_SEARCH,
        675,
        125,
        125,
        40
    );

    hCustList = createMainButton(
        mainWindow,
        "Refresh List",
        ID_CUST_LIST,
        810,
        125,
        125,
        40
    );

    hCustBack = createMainButton(
        mainWindow,
        "Back",
        ID_CUST_BACK,
        945,
        125,
        100,
        40
    );
    /* Billing action buttons */

hBillNew = createMainButton(
    mainWindow,
    "New Bill",
    ID_BILL_NEW,
    250,
    125,
    145,
    40
);

hBillHistory = createMainButton(
    mainWindow,
    "Bill History",
    ID_BILL_HISTORY,
    405,
    125,
    145,
    40
);

hBillRefresh = createMainButton(
    mainWindow,
    "Refresh",
    ID_BILL_REFRESH,
    560,
    125,
    125,
    40
);

hBillBack = createMainButton(
    mainWindow,
    "Back",
    ID_BILL_BACK,
    695,
    125,
    100,
    40
);

    /* Sales action buttons */

    hSalesRefresh = createMainButton(
        mainWindow,
        "Refresh",
        ID_SALES_REFRESH,
        250,
        125,
        120,
        40
    );

    hSalesDaily = createMainButton(
        mainWindow,
        "Daily Sales",
        ID_SALES_DAILY,
        380,
        125,
        130,
        40
    );

    hSalesHistory = createMainButton(
        mainWindow,
        "Detailed History",
        ID_SALES_HISTORY,
        520,
        125,
        150,
        40
    );

    hSalesBack = createMainButton(
        mainWindow,
        "Back",
        ID_SALES_BACK,
        680,
        125,
        100,
        40
    );
}


/* =========================================================
   CREATE DASHBOARD QUICK ACTIONS
   ========================================================= */

void GUI::createDashboard()
{
    hDashAddMed = createMainButton(
        mainWindow,
        "Add Medicine",
        ID_DASH_ADD_MED,
        250,
        560,
        180,
        45
    );

    hDashAddCust = createMainButton(
        mainWindow,
        "Add Customer",
        ID_DASH_ADD_CUST,
        450,
        560,
        180,
        45
    );

    hDashNewBill = createMainButton(
        mainWindow,
        "Create New Bill",
        ID_DASH_NEW_BILL,
        650,
        560,
        180,
        45
    );

    hDashReport = createMainButton(
        mainWindow,
        "View Reports",
        ID_DASH_REPORT,
        850,
        560,
        180,
        45
    );
}


/* =========================================================
   CREATE MAIN BUTTON
   ========================================================= */

static HWND createMainButton(
    HWND parent,
    const char* text,
    int id,
    int x,
    int y,
    int width,
    int height
)
{
    HWND button = CreateWindowExA(
        0,
        "BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x,
        y,
        width,
        height,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleA(NULL),
        NULL
    );

    if (button != NULL)
    {
        setFont(
            button,
            10,
            true
        );
    }

    return button;
}


/* =========================================================
   SET CONTROL FONT
   ========================================================= */

static void setFont(
    HWND control,
    int size,
    bool bold
)
{
    HFONT font = CreateFontA(
        -size,
        0,
        0,
        0,
        bold ? FW_BOLD : FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI"
    );

    SendMessageA(
        control,
        WM_SETFONT,
        reinterpret_cast<WPARAM>(font),
        TRUE
    );
}


/* =========================================================
   HIDE MAIN CONTROLS
   ========================================================= */

static void hideMainControls()
{
    HWND controls[] =
    {
        hNavDashboard,
        hNavMedicine,
        hNavCustomer,
        hNavBilling,
        hNavSales,
        hNavReports,
        hNavAlerts,
        hNavExit,

        hMedAdd,
        hMedUpdate,
        hMedDelete,
        hMedSearch,
        hMedList,
        hMedBack,
        hCustAdd,
        hCustUpdate,
        hCustDelete,
        hCustSearch,
        hCustList,
        hCustBack,
        hBillNew,
        hBillHistory,
        hBillRefresh,
        hBillBack,

        hSalesRefresh,
        hSalesDaily,
        hSalesHistory,
        hSalesBack,

        hDashAddMed,
        hDashAddCust,
        hDashNewBill,
        hDashReport
    };

    int count =
        sizeof(controls) / sizeof(controls[0]);

    for (int i = 0; i < count; i++)
    {
        if (controls[i] != NULL)
        {
            ShowWindow(
                controls[i],
                SW_HIDE
            );
        }
    }
}


/* =========================================================
   SHOW CONTROLS ACCORDING TO CURRENT SCREEN
   ========================================================= */

static void showMainControlsForScreen(int screen)
{
    hideMainControls();

    /* Navigation is always visible */

    HWND navigation[] =
    {
        hNavDashboard,
        hNavMedicine,
        hNavCustomer,
        hNavBilling,
        hNavSales,
        hNavReports,
        hNavAlerts,
        hNavExit
    };

    int navigationCount =
        sizeof(navigation) /
        sizeof(navigation[0]);

    for (int i = 0; i < navigationCount; i++)
    {
        if (navigation[i] != NULL)
        {
            ShowWindow(
                navigation[i],
                SW_SHOW
            );
        }
    }


    /* Dashboard */

    if (screen == 0)
    {
        ShowWindow(
            hDashAddMed,
            SW_SHOW
        );

        ShowWindow(
            hDashAddCust,
            SW_SHOW
        );

        ShowWindow(
            hDashNewBill,
            SW_SHOW
        );

        ShowWindow(
            hDashReport,
            SW_SHOW
        );
    }


    /* Medicine */

    if (screen == 1)
    {
        ShowWindow(hMedAdd, SW_SHOW);
        ShowWindow(hMedUpdate, SW_SHOW);
        ShowWindow(hMedDelete, SW_SHOW);
        ShowWindow(hMedSearch, SW_SHOW);
        ShowWindow(hMedList, SW_SHOW);
        ShowWindow(hMedBack, SW_SHOW);
    }
    /* Customer */

    if (screen == 2)
    {
        ShowWindow(hCustAdd, SW_SHOW);
        ShowWindow(hCustUpdate, SW_SHOW);
        ShowWindow(hCustDelete, SW_SHOW);
        ShowWindow(hCustSearch, SW_SHOW);
        ShowWindow(hCustList, SW_SHOW);
        ShowWindow(hCustBack, SW_SHOW);
    }
    if (screen == 3)
    {
        ShowWindow(hBillNew, SW_SHOW);
        ShowWindow(hBillHistory, SW_SHOW);
        ShowWindow(hBillRefresh, SW_SHOW);
        ShowWindow(hBillBack, SW_SHOW);
    }

    if (screen == 4)
    {
        ShowWindow(hSalesRefresh, SW_SHOW);
        ShowWindow(hSalesDaily, SW_SHOW);
        ShowWindow(hSalesHistory, SW_SHOW);
        ShowWindow(hSalesBack, SW_SHOW);
    }
}


/* =========================================================
   DRAW INTERFACE
   ========================================================= */

void GUI::drawInterface(HDC deviceContext)
{
    RECT clientRect;

    GetClientRect(
        mainWindow,
        &clientRect
    );

    drawFilledRect(
        deviceContext,
        0,
        0,
        clientRect.right,
        clientRect.bottom,
        GUI_BACKGROUND
    );


    /* Sidebar */

    drawFilledRect(
        deviceContext,
        0,
        0,
        230,
        clientRect.bottom,
        GUI_SIDEBAR
    );


    /* Sidebar top brand area */

    drawFilledRect(
        deviceContext,
        0,
        0,
        230,
        85,
        GUI_PRIMARY
    );

    drawText(
        deviceContext,
        "PharmaCare",
        20,
        18,
        190,
        30,
        GUI_WHITE,
        22,
        true
    );

    drawText(
        deviceContext,
        "Pharmacy Management",
        20,
        50,
        190,
        22,
        RGB(220, 230, 255),
        10,
        false
    );


    /* Main title */

    if (currentScreen == 2 || currentScreen == 3)
{
    drawText(
        deviceContext,
        "PharmaCare Pharmacy Store Management",
        250,
        35,
        850,
        40,
        GUI_TEXT,
        24,
        true
    );

    drawText(
        deviceContext,
        "Billing",
        250,
        80,
        500,
        35,
        RGB(35, 45, 65),
        20,
        true
    );
}
else
{
    drawText(
        deviceContext,
        "PharmaCare Pharmacy Store",
        250,
        35,
        700,
        40,
        GUI_TEXT,
        24,
        true
    );

    drawText(
        deviceContext,
        "Professional medicine, customer and billing management",
        250,
        72,
        750,
        25,
        GUI_GRAY,
        11,
        false
    );
}
    switch (currentScreen)
    {
        case 0:
            drawDashboard(deviceContext);
            break;

        case 1:
            drawMedicineScreen(deviceContext);
            break;

        case 2:
            drawCustomerScreen(deviceContext);
            break;

        case 3:
            drawBillingScreen(deviceContext);
            break;

        case 4:
            drawSalesScreen(deviceContext);
            break;

        case 5:
            drawReportsScreen(deviceContext);
            break;

        case 6:
            drawAlerts(deviceContext);
            break;

        default:
            drawDashboard(deviceContext);
            break;
    }
}


/* =========================================================
   DRAW DASHBOARD
   ========================================================= */

void GUI::drawDashboard(HDC deviceContext)
{
    int medicineCount = 0;
    int customerCount = 0;
    int salesCount = 0;
    double income = 0.0;
    int lowStock = 0;
    int expiryAlerts = 0;

    if (medicineManager != NULL)
    {
        medicineCount =
            medicineManager->getCount();

        lowStock =
            medicineManager->countLowStock(10);

        expiryAlerts =
            medicineManager->countExpiredOrNearExpiry();
    }

    if (customerManager != NULL)
    {
        customerCount =
            customerManager->getCount();
    }

    if (saleManager != NULL)
    {
        salesCount =
            saleManager->getCount();

        income =
            saleManager->calculateTotalIncome();
    }


    /* Dashboard subtitle */

    drawText(
        deviceContext,
        "Overview",
        250,
        115,
        250,
        30,
        GUI_TEXT,
        16,
        true
    );


    /* ---------------- CARD 1 ---------------- */

    drawCard(
        deviceContext,
        250,
        155,
        475,
        275,
        GUI_WHITE
    );

    drawText(
        deviceContext,
        "TOTAL MEDICINES",
        275,
        180,
        180,
        25,
        GUI_GRAY,
        10,
        true
    );

    {
        stringstream ss;
        ss << medicineCount;

        drawText(
            deviceContext,
            ss.str().c_str(),
            275,
            215,
            170,
            45,
            GUI_PRIMARY,
            30,
            true
        );
    }

    drawText(
        deviceContext,
        "Items in inventory",
        275,
        255,
        170,
        22,
        GUI_GRAY,
        10,
        false
    );


    /* ---------------- CARD 2 ---------------- */

    drawCard(
        deviceContext,
        500,
        155,
        725,
        275,
        GUI_WHITE
    );

    drawText(
        deviceContext,
        "TOTAL CUSTOMERS",
        525,
        180,
        180,
        25,
        GUI_GRAY,
        10,
        true
    );

    {
        stringstream ss;
        ss << customerCount;

        drawText(
            deviceContext,
            ss.str().c_str(),
            525,
            215,
            170,
            45,
            GUI_PURPLE,
            30,
            true
        );
    }

    drawText(
        deviceContext,
        "Registered customers",
        525,
        255,
        190,
        22,
        GUI_GRAY,
        10,
        false
    );


    /* ---------------- CARD 3 ---------------- */

    drawCard(
        deviceContext,
        250,
        295,
        475,
        415,
        GUI_WHITE
    );

    drawText(
        deviceContext,
        "TOTAL SALES",
        275,
        320,
        180,
        25,
        GUI_GRAY,
        10,
        true
    );

    {
        stringstream ss;
        ss << salesCount;

        drawText(
            deviceContext,
            ss.str().c_str(),
            275,
            355,
            170,
            45,
            GUI_GREEN,
            30,
            true
        );
    }

    drawText(
        deviceContext,
        "Completed transactions",
        275,
        395,
        190,
        22,
        GUI_GRAY,
        10,
        false
    );


    /* ---------------- CARD 4 ---------------- */

    drawCard(
        deviceContext,
        500,
        295,
        725,
        415,
        GUI_WHITE
    );

    drawText(
        deviceContext,
        "TOTAL INCOME",
        525,
        320,
        180,
        25,
        GUI_GRAY,
        10,
        true
    );

    {
        stringstream ss;

        ss << fixed
           << setprecision(2)
           << "Rs. "
           << income;

        drawText(
            deviceContext,
            ss.str().c_str(),
            525,
            355,
            190,
            45,
            GUI_ORANGE,
            22,
            true
        );
    }

    drawText(
        deviceContext,
        "Overall sales income",
        525,
        395,
        190,
        22,
        GUI_GRAY,
        10,
        false
    );


    /* ---------------- ALERT SUMMARY ---------------- */

    drawCard(
        deviceContext,
        750,
        155,
        1225,
        415,
        GUI_WHITE
    );

    drawText(
        deviceContext,
        "Inventory Alerts",
        780,
        180,
        250,
        30,
        GUI_TEXT,
        17,
        true
    );


    drawFilledRect(
        deviceContext,
        780,
        230,
        1195,
        295,
        RGB(255, 247, 235)
    );

    drawText(
        deviceContext,
        "Low Stock",
        805,
        245,
        200,
        25,
        GUI_ORANGE,
        12,
        true
    );

    {
        stringstream ss;
        ss << lowStock << " medicine(s)";

        drawText(
            deviceContext,
            ss.str().c_str(),
            1030,
            245,
            150,
            25,
            GUI_TEXT,
            11,
            true
        );
    }


    drawFilledRect(
        deviceContext,
        780,
        315,
        1195,
        380,
        RGB(255, 240, 240)
    );

    drawText(
        deviceContext,
        "Expiry Alerts",
        805,
        330,
        200,
        25,
        GUI_RED,
        12,
        true
    );

    {
        stringstream ss;
        ss << expiryAlerts << " medicine(s)";

        drawText(
            deviceContext,
            ss.str().c_str(),
            1030,
            330,
            150,
            25,
            GUI_TEXT,
            11,
            true
        );
    }


    /* Quick action heading */

    drawText(
        deviceContext,
        "Quick Actions",
        250,
        505,
        250,
        30,
        GUI_TEXT,
        16,
        true
    );
}


/* =========================================================
   DRAW MEDICINE SCREEN
   ========================================================= */

void GUI::drawMedicineScreen(HDC deviceContext)
{
    drawText(
        deviceContext,
        "Medicine Management",
        250,
        175,
        400,
        35,
        GUI_TEXT,
        20,
        true
    );

    drawText(
        deviceContext,
        "Manage medicines, stock quantity, pricing and expiry dates.",
        250,
        208,
        650,
        25,
        GUI_GRAY,
        11,
        false
    );


    /* Table header */

    int tableLeft = 250;
    int tableTop = 255;

    int colID = 70;
    int colName = 180;
    int colCategory = 150;
    int colPrice = 110;
    int colQuantity = 110;
    int colExpiry = 140;

    int totalWidth =
        colID +
        colName +
        colCategory +
        colPrice +
        colQuantity +
        colExpiry;


    drawFilledRect(
        deviceContext,
        tableLeft,
        tableTop,
        tableLeft + totalWidth,
        tableTop + 42,
        GUI_PRIMARY
    );


    drawText(
        deviceContext,
        "ID",
        tableLeft + 10,
        tableTop + 10,
        colID - 15,
        25,
        GUI_WHITE,
        10,
        true
    );

    drawText(
        deviceContext,
        "Medicine Name",
        tableLeft + colID + 10,
        tableTop + 10,
        colName - 15,
        25,
        GUI_WHITE,
        10,
        true
    );

    drawText(
        deviceContext,
        "Category",
        tableLeft + colID + colName + 10,
        tableTop + 10,
        colCategory - 15,
        25,
        GUI_WHITE,
        10,
        true
    );

    drawText(
        deviceContext,
        "Price",
        tableLeft + colID + colName + colCategory + 10,
        tableTop + 10,
        colPrice - 15,
        25,
        GUI_WHITE,
        10,
        true
    );

    drawText(
        deviceContext,
        "Quantity",
        tableLeft +
        colID +
        colName +
        colCategory +
        colPrice +
        10,
        tableTop + 10,
        colQuantity - 15,
        25,
        GUI_WHITE,
        10,
        true
    );

    drawText(
        deviceContext,
        "Expiry Date",
        tableLeft +
        colID +
        colName +
        colCategory +
        colPrice +
        colQuantity +
        10,
        tableTop + 10,
        colExpiry - 15,
        25,
        GUI_WHITE,
        10,
        true
    );


    /* Table rows */

    if (medicineManager == NULL)
    {
        return;
    }

    int rowTop = tableTop + 42;

    for (int i = 0;
         i < medicineManager->getCount() && i < 12;
         i++)
    {
        Medicine* medicine =
            medicineManager->getMedicineAt(i);

        if (medicine == NULL)
        {
            continue;
        }

        COLORREF rowColor =
            (i % 2 == 0)
            ? GUI_WHITE
            : RGB(248, 250, 253);

        drawFilledRect(
            deviceContext,
            tableLeft,
            rowTop,
            tableLeft + totalWidth,
            rowTop + 38,
            rowColor
        );


        stringstream id;
        id << medicine->getMedicineID();

        stringstream price;
        price << fixed
              << setprecision(2)
              << medicine->getPrice();

        stringstream quantity;
        quantity << medicine->getQuantity();


        drawText(
            deviceContext,
            id.str().c_str(),
            tableLeft + 10,
            rowTop + 8,
            colID - 15,
            22,
            GUI_TEXT,
            10,
            false
        );

        drawText(
            deviceContext,
            medicine->getName().c_str(),
            tableLeft + colID + 10,
            rowTop + 8,
            colName - 15,
            22,
            GUI_TEXT,
            10,
            false
        );

        drawText(
            deviceContext,
            medicine->getCategory().c_str(),
            tableLeft + colID + colName + 10,
            rowTop + 8,
            colCategory - 15,
            22,
            GUI_TEXT,
            10,
            false
        );

        drawText(
            deviceContext,
            price.str().c_str(),
            tableLeft +
            colID +
            colName +
            colCategory +
            10,
            rowTop + 8,
            colPrice - 15,
            22,
            GUI_TEXT,
            10,
            false
        );

        drawText(
            deviceContext,
            quantity.str().c_str(),
            tableLeft +
            colID +
            colName +
            colCategory +
            colPrice +
            10,
            rowTop + 8,
            colQuantity - 15,
            22,
            GUI_TEXT,
            10,
            false
        );

        drawText(
            deviceContext,
            medicine->getExpiryDate().c_str(),
            tableLeft +
            colID +
            colName +
            colCategory +
            colPrice +
            colQuantity +
            10,
            rowTop + 8,
            colExpiry - 15,
            22,
            GUI_TEXT,
            10,
            false
        );


        rowTop += 38;
    }
}


/* =========================================================
   DRAW CUSTOMER SCREEN
   ========================================================= */

void GUI::drawCustomerScreen(HDC deviceContext)
{
    
    int x = 270;
    int y = 205;

    // Table header
    HBRUSH headerBrush =
        CreateSolidBrush(RGB(70, 90, 150));

    RECT headerRect =
    {
        x,
        y,
        1150,
        y + 48
    };

    FillRect(
        deviceContext,
        &headerRect,
        headerBrush
    );

    DeleteObject(headerBrush);

    drawText(
        deviceContext,
        "Customer ID",
        x + 15,
        y + 10,
        110,
        30,
        RGB(255, 255, 255),
        12,
        true
    );

    drawText(
        deviceContext,
        "Name",
        x + 145,
        y + 10,
        180,
        30,
        RGB(255, 255, 255),
        15,
        true
    );

    drawText(
        deviceContext,
        "Phone",
        x + 365,
        y + 10,
        140,
        30,
        RGB(255, 255, 255),
        15,
        true
    );

    drawText(
        deviceContext,
        "Address",
        x + 535,
        y + 10,
        300,
        30,
        RGB(255, 255, 255),
        15,
        true
    );

    int rowY = y + 48;

    for (
        int i = 0;
        i < customerManager->getCount() && i < 10;
        i++
    )
    {
        Customer* customer =
            customerManager->getCustomerAt(i);

        if (customer == NULL)
            continue;

        COLORREF rowColor;

        if (i % 2 == 0)
            rowColor = RGB(248, 250, 253);
        else
            rowColor = RGB(238, 243, 250);

        HBRUSH rowBrush =
            CreateSolidBrush(rowColor);

        RECT rowRect =
        {
            x,
            rowY,
            1150,
            rowY + 40
        };

        FillRect(
            deviceContext,
            &rowRect,
            rowBrush
        );

        DeleteObject(rowBrush);

        string customerID =
            to_string(customer->getCustomerID());

        string name =
            customer->getName();

        string phone =
            customer->getPhone();

        string address =
            customer->getAddress();

        drawText(
            deviceContext,
            customerID.c_str(),
            x + 15,
            rowY + 10,
            110,
            30,
            RGB(45, 55, 70),
            11,
            false
        );

        drawText(
            deviceContext,
            name.c_str(),
            x + 145,
            rowY + 10,
            180,
            30,
            RGB(45, 55, 70),
            11,
            false
        );

        drawText(
            deviceContext,
            phone.c_str(),
            x + 365,
            rowY + 10,
            140,
            30,
            RGB(45, 55, 70),
            11,
            false
        );

        drawText(
            deviceContext,
            address.c_str(),
            x + 535,
            rowY + 10,
            300,
            30,
            RGB(45, 55, 70),
            11,
            false
        );

        rowY += 40;
    }

    // Table border
    HPEN borderPen =
        CreatePen(
            PS_SOLID,
            1,
            RGB(215, 220, 230)
        );

    HPEN oldPen =
        (HPEN)SelectObject(
            deviceContext,
            borderPen
        );

    HBRUSH oldBrush =
        (HBRUSH)SelectObject(
            deviceContext,
            GetStockObject(NULL_BRUSH)
        );

    Rectangle(
        deviceContext,
        x,
        y,
        1150,
        rowY
    );

    SelectObject(
        deviceContext,
        oldBrush
    );

    SelectObject(
        deviceContext,
        oldPen
    );

    DeleteObject(borderPen);

    if (customerManager->getCount() == 0)
    {
        drawText(
            deviceContext,
            "No customers found.",
            500,
            390,
            300,
            35,
            RGB(120, 130, 145),
            18,
            false
        );
    }
}

/* =========================================================
   DRAW BILLING SCREEN
   ========================================================= */

void GUI::drawBillingScreen(HDC deviceContext)
{
    
    // Summary cards
    int cardY = 190;

    // Bill number card
    HBRUSH blueBrush =
        CreateSolidBrush(RGB(232, 240, 255));

    RECT billCard =
    {
        270,
        cardY,
        540,
        cardY + 105
    };

    FillRect(
        deviceContext,
        &billCard,
        blueBrush
    );

    DeleteObject(blueBrush);

    drawText(
        deviceContext,
        "NEXT BILL NUMBER",
        290,
        cardY + 18,
        220,
        25,
        RGB(80, 95, 120),
        13,
        true
    );

    string nextBill =
        to_string(billManager->getNextBillNumber());

    drawText(
        deviceContext,
        nextBill.c_str(),
        290,
        cardY + 48,
        200,
        40,
        RGB(45, 75, 150),
        20,
        true
    );

    // Total bills card
    HBRUSH greenBrush =
        CreateSolidBrush(RGB(232, 249, 240));

    RECT totalCard =
    {
        560,
        cardY,
        830,
        cardY + 105
    };

    FillRect(
        deviceContext,
        &totalCard,
        greenBrush
    );

    DeleteObject(greenBrush);

    drawText(
        deviceContext,
        "TOTAL BILLS",
        580,
        cardY + 18,
        200,
        25,
        RGB(80, 105, 90),
        13,
        true
    );

    string totalBills =
        to_string(billManager->getCount());

    drawText(
        deviceContext,
        totalBills.c_str(),
        580,
        cardY + 48,
        200,
        40,
        RGB(40, 130, 80),
        20,
        true
    );

    
}
/* =========================================================
   DRAW SALES SCREEN
   ========================================================= */

void GUI::drawSalesScreen(HDC deviceContext)
{
    drawText(
        deviceContext,
        "Sales Management",
        250,
        175,
        400,
        35,
        GUI_TEXT,
        20,
        true
    );

    drawText(
        deviceContext,
        "View complete sales history and daily sales information.",
        250,
        210,
        700,
        25,
        GUI_GRAY,
        11,
        false
    );

    string today = getCurrentDate();
    int todaySalesCount = 0;
    double todayIncome = 0.0;
    int totalSales = 0;
    double totalIncome = 0.0;

    if (saleManager != NULL)
    {
        todaySalesCount = saleManager->countDailySales(today);
        todayIncome = saleManager->calculateDailyIncome(today);

        // Also check if alternate DD-MM-YYYY format exists for today
        time_t now = time(NULL);
        tm* lt = localtime(&now);
        if (lt != NULL)
        {
            char altBuf[20];
            sprintf(altBuf, "%02d-%02d-%04d", lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
            string altToday = altBuf;
            if (altToday != today)
            {
                todaySalesCount += saleManager->countDailySales(altToday);
                todayIncome += saleManager->calculateDailyIncome(altToday);
            }
        }

        totalSales = saleManager->getCount();
        totalIncome = saleManager->calculateTotalIncome();
    }

    /* Stat Cards */
    int cardY = 240;
    int cardHeight = 70;

    // Card 1: Today's Sales
    drawCard(deviceContext, 250, cardY, 460, cardY + cardHeight, GUI_WHITE);
    drawText(deviceContext, "TODAY'S SALES", 265, cardY + 10, 180, 20, GUI_GRAY, 10, true);
    string todaySalesStr = to_string(todaySalesCount) + " Bills";
    drawText(deviceContext, todaySalesStr.c_str(), 265, cardY + 32, 180, 30, GUI_PRIMARY, 16, true);

    // Card 2: Today's Revenue
    drawCard(deviceContext, 480, cardY, 690, cardY + cardHeight, GUI_WHITE);
    drawText(deviceContext, "TODAY'S REVENUE", 495, cardY + 10, 180, 20, GUI_GRAY, 10, true);
    stringstream ssToday;
    ssToday << "Rs. " << fixed << setprecision(2) << todayIncome;
    drawText(deviceContext, ssToday.str().c_str(), 495, cardY + 32, 180, 30, GUI_GREEN, 16, true);

    // Card 3: Total Sales
    drawCard(deviceContext, 710, cardY, 920, cardY + cardHeight, GUI_WHITE);
    drawText(deviceContext, "TOTAL TRANSACTIONS", 725, cardY + 10, 180, 20, GUI_GRAY, 10, true);
    string totalSalesStr = to_string(totalSales) + " Total";
    drawText(deviceContext, totalSalesStr.c_str(), 725, cardY + 32, 180, 30, GUI_TEXT, 16, true);

    // Card 4: Total Revenue
    drawCard(deviceContext, 940, cardY, 1150, cardY + cardHeight, GUI_WHITE);
    drawText(deviceContext, "TOTAL REVENUE", 955, cardY + 10, 180, 20, GUI_GRAY, 10, true);
    stringstream ssTotal;
    ssTotal << "Rs. " << fixed << setprecision(2) << totalIncome;
    drawText(deviceContext, ssTotal.str().c_str(), 955, cardY + 32, 180, 30, GUI_PRIMARY, 16, true);

    /* Sales History Table */
    int tableLeft = 250;
    int tableTop = 325;

    int colSaleID = 100;
    int colBillNo = 130;
    int colDate   = 160;
    int colAmount = 150;

    int totalWidth = colSaleID + colBillNo + colDate + colAmount;

    // Header Background
    drawFilledRect(
        deviceContext,
        tableLeft,
        tableTop,
        tableLeft + totalWidth,
        tableTop + 40,
        GUI_PRIMARY
    );

    drawText(deviceContext, "Sale ID", tableLeft + 15, tableTop + 10, colSaleID - 20, 25, GUI_WHITE, 10, true);
    drawText(deviceContext, "Bill No.", tableLeft + colSaleID + 15, tableTop + 10, colBillNo - 20, 25, GUI_WHITE, 10, true);
    drawText(deviceContext, "Date", tableLeft + colSaleID + colBillNo + 15, tableTop + 10, colDate - 20, 25, GUI_WHITE, 10, true);
    drawText(deviceContext, "Amount", tableLeft + colSaleID + colBillNo + colDate + 15, tableTop + 10, colAmount - 20, 25, GUI_WHITE, 10, true);

    if (saleManager == NULL || totalSales == 0)
    {
        drawFilledRect(
            deviceContext,
            tableLeft,
            tableTop + 40,
            tableLeft + totalWidth,
            tableTop + 110,
            GUI_WHITE
        );

        drawText(
            deviceContext,
            "No sales records found. Sales are automatically recorded when bills are generated.",
            tableLeft + 20,
            tableTop + 60,
            totalWidth - 40,
            30,
            GUI_GRAY,
            11,
            false
        );
        return;
    }

    int rowTop = tableTop + 40;
    // Display up to 10 most recent sales
    int displayedCount = 0;
    for (int i = totalSales - 1; i >= 0 && displayedCount < 10; i--)
    {
        Sale* sale = saleManager->getSaleAt(i);
        if (sale == NULL)
        {
            continue;
        }

        COLORREF rowColor = (displayedCount % 2 == 0) ? GUI_WHITE : RGB(248, 250, 253);

        drawFilledRect(
            deviceContext,
            tableLeft,
            rowTop,
            tableLeft + totalWidth,
            rowTop + 34,
            rowColor
        );

        string sId = to_string(sale->getSaleID());
        string bNo = to_string(sale->getBillNumber());
        string sDate = sale->getDate();
        stringstream sAmt;
        sAmt << "Rs. " << fixed << setprecision(2) << sale->getAmount();

        drawText(deviceContext, sId.c_str(), tableLeft + 15, rowTop + 7, colSaleID - 20, 22, GUI_TEXT, 10, false);
        drawText(deviceContext, bNo.c_str(), tableLeft + colSaleID + 15, rowTop + 7, colBillNo - 20, 22, GUI_TEXT, 10, false);
        drawText(deviceContext, sDate.c_str(), tableLeft + colSaleID + colBillNo + 15, rowTop + 7, colDate - 20, 22, GUI_TEXT, 10, false);
        drawText(deviceContext, sAmt.str().c_str(), tableLeft + colSaleID + colBillNo + colDate + 15, rowTop + 7, colAmount - 20, 22, GUI_TEXT, 10, true);

        rowTop += 34;
        displayedCount++;
    }
}


/* =========================================================
   DRAW REPORTS SCREEN
   ========================================================= */

void GUI::drawReportsScreen(HDC deviceContext)
{
    drawText(
        deviceContext,
        "Daily Sales Report",
        250,
        175,
        500,
        35,
        GUI_TEXT,
        20,
        true
    );

    drawText(
        deviceContext,
        "Sales performance for the last 7 days.",
        250,
        210,
        700,
        25,
        GUI_GRAY,
        11,
        false
    );


    int tableLeft = 250;
    int tableTop = 255;

    int dateWidth = 220;
    int countWidth = 180;
    int incomeWidth = 220;

    int totalWidth =
        dateWidth +
        countWidth +
        incomeWidth;


    /* Table Header */

    drawFilledRect(
        deviceContext,
        tableLeft,
        tableTop,
        tableLeft + totalWidth,
        tableTop + 42,
        GUI_PRIMARY
    );


    drawText(
        deviceContext,
        "Date",
        tableLeft + 15,
        tableTop + 10,
        dateWidth - 20,
        25,
        GUI_WHITE,
        10,
        true
    );


    drawText(
        deviceContext,
        "Number of Sales",
        tableLeft + dateWidth + 15,
        tableTop + 10,
        countWidth - 20,
        25,
        GUI_WHITE,
        10,
        true
    );


    drawText(
        deviceContext,
        "Daily Income",
        tableLeft +
        dateWidth +
        countWidth +
        15,
        tableTop + 10,
        incomeWidth - 20,
        25,
        GUI_WHITE,
        10,
        true
    );


    if (saleManager == NULL)
    {
        return;
    }


    time_t currentTime =
        time(NULL);


    int rowTop =
        tableTop + 42;


    double sevenDayTotal =
        0.0;


    for (int i = 0; i < 7; i++)
    {
        time_t dayTime =
            currentTime -
            static_cast<time_t>(i) *
            24 *
            60 *
            60;


        tm* day =
            localtime(&dayTime);


        if (day == NULL)
        {
            continue;
        }


        char dateBuffer[20];


        sprintf(
            dateBuffer,
            "%04d-%02d-%02d",
            day->tm_year + 1900,
            day->tm_mon + 1,
            day->tm_mday
        );


        string date =
            dateBuffer;


        int salesCount =
            saleManager->countDailySales(
                date
            );


        double income =
            saleManager->calculateDailyIncome(
                date
            );


        sevenDayTotal +=
            income;


        COLORREF rowColor =
            (i % 2 == 0)
            ? GUI_WHITE
            : RGB(248, 250, 253);


        drawFilledRect(
            deviceContext,
            tableLeft,
            rowTop,
            tableLeft + totalWidth,
            rowTop + 42,
            rowColor
        );


        drawText(
            deviceContext,
            date.c_str(),
            tableLeft + 15,
            rowTop + 10,
            dateWidth - 20,
            25,
            GUI_TEXT,
            10,
            false
        );


        stringstream countText;

        countText
            << salesCount;


        drawText(
            deviceContext,
            countText.str().c_str(),
            tableLeft +
            dateWidth +
            15,
            rowTop + 10,
            countWidth - 20,
            25,
            GUI_TEXT,
            10,
            false
        );


        stringstream incomeText;

        incomeText
            << fixed
            << setprecision(2)
            << "Rs. "
            << income;


        drawText(
            deviceContext,
            incomeText.str().c_str(),
            tableLeft +
            dateWidth +
            countWidth +
            15,
            rowTop + 10,
            incomeWidth - 20,
            25,
            GUI_GREEN,
            10,
            true
        );


        rowTop += 42;
    }


    /* Seven-day total */

    drawCard(
        deviceContext,
        250,
        610,
        870,
        690,
        GUI_WHITE
    );


    stringstream totalText;

    totalText
        << "7-Day Total Income: Rs. "
        << fixed
        << setprecision(2)
        << sevenDayTotal;


    drawText(
        deviceContext,
        totalText.str().c_str(),
        275,
        630,
        560,
        30,
        GUI_PRIMARY,
        14,
        true
    );
}


/* =========================================================
   DRAW ALERTS SCREEN
   ========================================================= */

void GUI::drawAlerts(HDC deviceContext)
{
    drawText(
        deviceContext,
        "Inventory Alerts",
        250,
        175,
        450,
        35,
        GUI_TEXT,
        20,
        true
    );


    drawText(
        deviceContext,
        "Low-stock and expiry alerts requiring attention.",
        250,
        210,
        700,
        25,
        GUI_GRAY,
        11,
        false
    );


    /* =====================================================
       LOW STOCK TABLE
       ===================================================== */

    drawText(
        deviceContext,
        "Low Stock Medicines",
        250,
        255,
        350,
        30,
        GUI_ORANGE,
        16,
        true
    );


    int tableLeft = 250;
    int tableTop = 295;


    int idWidth = 70;
    int nameWidth = 180;
    int categoryWidth = 150;
    int quantityWidth = 100;


    int totalWidth =
        idWidth +
        nameWidth +
        categoryWidth +
        quantityWidth;


    drawFilledRect(
        deviceContext,
        tableLeft,
        tableTop,
        tableLeft + totalWidth,
        tableTop + 40,
        GUI_ORANGE
    );


    drawText(
        deviceContext,
        "ID",
        tableLeft + 10,
        tableTop + 8,
        idWidth - 15,
        25,
        GUI_WHITE,
        10,
        true
    );


    drawText(
        deviceContext,
        "Medicine",
        tableLeft + idWidth + 10,
        tableTop + 8,
        nameWidth - 15,
        25,
        GUI_WHITE,
        10,
        true
    );


    drawText(
        deviceContext,
        "Category",
        tableLeft +
        idWidth +
        nameWidth +
        10,
        tableTop + 8,
        categoryWidth - 15,
        25,
        GUI_WHITE,
        10,
        true
    );


    drawText(
        deviceContext,
        "Quantity",
        tableLeft +
        idWidth +
        nameWidth +
        categoryWidth +
        10,
        tableTop + 8,
        quantityWidth - 15,
        25,
        GUI_WHITE,
        10,
        true
    );


    int rowTop =
        tableTop + 40;


    if (medicineManager != NULL)
    {
        for (
            int i = 0;
            i < medicineManager->getCount();
            i++
        )
        {
            Medicine* medicine =
                medicineManager->getMedicineAt(i);


            if (medicine == NULL)
            {
                continue;
            }


            if (medicine->getQuantity() > 10)
            {
                continue;
            }


            COLORREF rowColor =
                (i % 2 == 0)
                ? GUI_WHITE
                : RGB(255, 249, 240);


            drawFilledRect(
                deviceContext,
                tableLeft,
                rowTop,
                tableLeft + totalWidth,
                rowTop + 38,
                rowColor
            );


            stringstream idText;
            idText
                << medicine->getMedicineID();


            stringstream quantityText;
            quantityText
                << medicine->getQuantity();


            drawText(
                deviceContext,
                idText.str().c_str(),
                tableLeft + 10,
                rowTop + 8,
                idWidth - 15,
                22,
                GUI_TEXT,
                10,
                false
            );


            drawText(
                deviceContext,
                medicine->getName().c_str(),
                tableLeft +
                idWidth +
                10,
                rowTop + 8,
                nameWidth - 15,
                22,
                GUI_TEXT,
                10,
                false
            );


            drawText(
                deviceContext,
                medicine->getCategory().c_str(),
                tableLeft +
                idWidth +
                nameWidth +
                10,
                rowTop + 8,
                categoryWidth - 15,
                22,
                GUI_TEXT,
                10,
                false
            );


            drawText(
                deviceContext,
                quantityText.str().c_str(),
                tableLeft +
                idWidth +
                nameWidth +
                categoryWidth +
                10,
                rowTop + 8,
                quantityWidth - 15,
                22,
                GUI_RED,
                10,
                true
            );


            rowTop += 38;
        }
    }


    /* =====================================================
       EXPIRY ALERTS
       ===================================================== */

    int expiryTop =
        520;


    drawText(
        deviceContext,
        "Expired / Near-Expiry Medicines",
        250,
        expiryTop,
        450,
        30,
        GUI_RED,
        16,
        true
    );


    int expiryTableTop =
        expiryTop + 40;


    int expiryIDWidth = 60;
    int expiryNameWidth = 150;
    int expiryCategoryWidth = 130;
    int expiryQuantityWidth = 90;
    int expiryDateWidth = 130;
    int expiryStatusWidth = 150;


    int expiryTotalWidth =
        expiryIDWidth +
        expiryNameWidth +
        expiryCategoryWidth +
        expiryQuantityWidth +
        expiryDateWidth +
        expiryStatusWidth;


    drawFilledRect(
        deviceContext,
        tableLeft,
        expiryTableTop,
        tableLeft + expiryTotalWidth,
        expiryTableTop + 40,
        GUI_RED
    );


    drawText(
        deviceContext,
        "ID",
        tableLeft + 8,
        expiryTableTop + 8,
        expiryIDWidth - 10,
        25,
        GUI_WHITE,
        9,
        true
    );


    drawText(
        deviceContext,
        "Medicine",
        tableLeft +
        expiryIDWidth +
        8,
        expiryTableTop + 8,
        expiryNameWidth - 10,
        25,
        GUI_WHITE,
        9,
        true
    );


    drawText(
        deviceContext,
        "Category",
        tableLeft +
        expiryIDWidth +
        expiryNameWidth +
        8,
        expiryTableTop + 8,
        expiryCategoryWidth - 10,
        25,
        GUI_WHITE,
        9,
        true
    );


    drawText(
        deviceContext,
        "Qty",
        tableLeft +
        expiryIDWidth +
        expiryNameWidth +
        expiryCategoryWidth +
        8,
        expiryTableTop + 8,
        expiryQuantityWidth - 10,
        25,
        GUI_WHITE,
        9,
        true
    );


    drawText(
        deviceContext,
        "Expiry",
        tableLeft +
        expiryIDWidth +
        expiryNameWidth +
        expiryCategoryWidth +
        expiryQuantityWidth +
        8,
        expiryTableTop + 8,
        expiryDateWidth - 10,
        25,
        GUI_WHITE,
        9,
        true
    );


    drawText(
        deviceContext,
        "Status",
        tableLeft +
        expiryIDWidth +
        expiryNameWidth +
        expiryCategoryWidth +
        expiryQuantityWidth +
        expiryDateWidth +
        8,
        expiryTableTop + 8,
        expiryStatusWidth - 10,
        25,
        GUI_WHITE,
        9,
        true
    );


    /*
       The existing MedicineManager already provides the
       expiry-alert count. Here we display the detailed
       medicine information using the stored expiry date.
    */

    int expiryRow =
        expiryTableTop + 40;


    if (medicineManager != NULL)
    {
        time_t currentTime =
            time(NULL);


        for (
            int i = 0;
            i < medicineManager->getCount();
            i++
        )
        {
            Medicine* medicine =
                medicineManager->getMedicineAt(i);


            if (medicine == NULL)
            {
                continue;
            }


            string expiryDate =
                medicine->getExpiryDate();


            int year = 0;
int month = 0;
int day = 0;

/*
   Accept common date formats:

   YYYY-MM-DD
   YYYY/MM/DD
   DD-MM-YYYY
   DD/MM/YYYY
*/

int first = 0;
int second = 0;
int third = 0;

char separator1 = '-';
char separator2 = '-';

if (sscanf(
        expiryDate.c_str(),
        "%d%c%d%c%d",
        &first,
        &separator1,
        &second,
        &separator2,
        &third
    ) == 5)
{
    if (first >= 1900)
    {
        // YYYY-MM-DD
        year = first;
        month = second;
        day = third;
    }
    else if (third >= 1900)
    {
        // DD-MM-YYYY
        day = first;
        month = second;
        year = third;
    }
}

            if (year <= 0 ||
                month <= 0 ||
                day <= 0)
            {
                continue;
            }


            tm expiryTime;

            ZeroMemory(
                &expiryTime,
                sizeof(expiryTime)
            );


            expiryTime.tm_year =
                year - 1900;

            expiryTime.tm_mon =
                month - 1;

            expiryTime.tm_mday =
                day;

            expiryTime.tm_hour =
                12;


            time_t expiryTimestamp =
                mktime(&expiryTime);


            if (expiryTimestamp == -1)
            {
                continue;
            }


            double difference =
                difftime(
                    expiryTimestamp,
                    currentTime
                );


            int daysRemaining =
                static_cast<int>(
                    difference /
                    (24.0 * 60.0 * 60.0)
                );


            bool expired =
                daysRemaining < 0;


            bool nearExpiry =
                !expired &&
                daysRemaining <= 30;


            if (!expired &&
                !nearExpiry)
            {
                continue;
            }


            string status;


            if (expired)
            {
                status = "EXPIRED";
            }
            else
            {
                status = "NEAR EXPIRY";
            }


            COLORREF rowColor =
                expired
                ? RGB(255, 240, 240)
                : RGB(255, 248, 235);


            drawFilledRect(
                deviceContext,
                tableLeft,
                expiryRow,
                tableLeft + expiryTotalWidth,
                expiryRow + 38,
                rowColor
            );


            stringstream idText;

            idText
                << medicine->getMedicineID();


            stringstream quantityText;

            quantityText
                << medicine->getQuantity();


            drawText(
                deviceContext,
                idText.str().c_str(),
                tableLeft + 8,
                expiryRow + 8,
                expiryIDWidth - 10,
                22,
                GUI_TEXT,
                9,
                false
            );


            drawText(
                deviceContext,
                medicine->getName().c_str(),
                tableLeft +
                expiryIDWidth +
                8,
                expiryRow + 8,
                expiryNameWidth - 10,
                22,
                GUI_TEXT,
                9,
                false
            );


            drawText(
                deviceContext,
                medicine->getCategory().c_str(),
                tableLeft +
                expiryIDWidth +
                expiryNameWidth +
                8,
                expiryRow + 8,
                expiryCategoryWidth - 10,
                22,
                GUI_TEXT,
                9,
                false
            );


            drawText(
                deviceContext,
                quantityText.str().c_str(),
                tableLeft +
                expiryIDWidth +
                expiryNameWidth +
                expiryCategoryWidth +
                8,
                expiryRow + 8,
                expiryQuantityWidth - 10,
                22,
                GUI_TEXT,
                9,
                false
            );


            drawText(
                deviceContext,
                expiryDate.c_str(),
                tableLeft +
                expiryIDWidth +
                expiryNameWidth +
                expiryCategoryWidth +
                expiryQuantityWidth +
                8,
                expiryRow + 8,
                expiryDateWidth - 10,
                22,
                GUI_TEXT,
                9,
                false
            );


            drawText(
                deviceContext,
                status.c_str(),
                tableLeft +
                expiryIDWidth +
                expiryNameWidth +
                expiryCategoryWidth +
                expiryQuantityWidth +
                expiryDateWidth +
                8,
                expiryRow + 8,
                expiryStatusWidth - 10,
                22,
                expired
                ? GUI_RED
                : GUI_ORANGE,
                9,
                true
            );


            expiryRow += 38;


            /*
               Keep the screen inside the visible area.
               The full information remains available through
               the alert summary/count and table.
            */
            if (expiryRow > 700)
            {
                break;
            }
        }
    }
}
/* =========================================================
   DRAW BASIC CARD
   ========================================================= */

static void drawCard(
    HDC dc,
    int left,
    int top,
    int right,
    int bottom,
    COLORREF color
)
{
    HBRUSH brush =
        CreateSolidBrush(color);

    RECT rect;
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;

    FillRect(
        dc,
        &rect,
        brush
    );

    DeleteObject(brush);

    HPEN pen =
        CreatePen(
            PS_SOLID,
            1,
            GUI_LIGHTGRAY
        );

    HPEN oldPen =
        reinterpret_cast<HPEN>(
            SelectObject(dc, pen)
        );

    HBRUSH oldBrush =
        reinterpret_cast<HBRUSH>(
            SelectObject(dc, GetStockObject(NULL_BRUSH))
        );

    Rectangle(
        dc,
        left,
        top,
        right,
        bottom
    );

    SelectObject(
        dc,
        oldBrush
    );

    SelectObject(
        dc,
        oldPen
    );

    DeleteObject(pen);
}


/* =========================================================
   DRAW FILLED RECTANGLE
   ========================================================= */

static void drawFilledRect(
    HDC dc,
    int left,
    int top,
    int right,
    int bottom,
    COLORREF color
)
{
    HBRUSH brush =
        CreateSolidBrush(color);

    RECT rect;

    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;

    FillRect(
        dc,
        &rect,
        brush
    );

    DeleteObject(brush);
}


/* =========================================================
   DRAW TEXT
   ========================================================= */

static void drawText(
    HDC dc,
    const char* text,
    int x,
    int y,
    int width,
    int height,
    COLORREF color,
    int size,
    bool bold
)
{
    HFONT font =
        CreateFontA(
            -size,
            0,
            0,
            0,
            bold ? FW_BOLD : FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            "Segoe UI"
        );

    HFONT oldFont =
        reinterpret_cast<HFONT>(
            SelectObject(dc, font)
        );

    SetTextColor(
        dc,
        color
    );

    SetBkMode(
        dc,
        TRANSPARENT
    );

    RECT rect;

    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;

    DrawTextA(
        dc,
        text,
        -1,
        &rect,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE
    );

    SelectObject(
        dc,
        oldFont
    );

    DeleteObject(font);
}


/* =========================================================
   SHOW MESSAGE
   ========================================================= */

void GUI::showMessage(
    string title,
    string message
)
{
    MessageBoxA(
        mainWindow,
        message.c_str(),
        title.c_str(),
        MB_OK | MB_ICONINFORMATION
    );
}


/* =========================================================
   STRING TO INTEGER
   ========================================================= */

int GUI::stringToInteger(string value)
{
    if (value.empty())
    {
        return 0;
    }

    return atoi(
        value.c_str()
    );
}


/* =========================================================
   STRING TO DOUBLE
   ========================================================= */

double GUI::stringToDouble(string value)
{
    if (value.empty())
    {
        return 0.0;
    }

    return atof(
        value.c_str()
    );
}


/* =========================================================
   CURRENT DATE
   ========================================================= */

string GUI::getCurrentDate()
{
    time_t currentTime =
        time(NULL);

    tm* localTime =
        localtime(&currentTime);

    if (localTime == NULL)
    {
        return "";
    }

    char dateBuffer[20];

    sprintf(
        dateBuffer,
        "%04d-%02d-%02d",
        localTime->tm_year + 1900,
        localTime->tm_mon + 1,
        localTime->tm_mday
    );

    return string(dateBuffer);
}


/* =========================================================
   INPUT VALIDATION AND COMMON FORM HELPERS
   ========================================================= */

static const char* MEDICINE_CATEGORIES[] = {
    "Tablets",
    "Syrup",
    "Capsules",
    "Injection",
    "Drops",
    "Ointment",
    "Cream",
    "Powder",
    "Inhaler",
    "Other"
};
static const int MEDICINE_CATEGORY_COUNT = 10;

static string trimString(const string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos)
    {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

static string getTrimmedText(HWND control)
{
    if (control == NULL)
    {
        return "";
    }
    char buffer[1024];
    buffer[0] = '\0';
    GetWindowTextA(control, buffer, sizeof(buffer));
    return trimString(buffer);
}

static string formatPrice(double price)
{
    stringstream ss;
    ss << fixed << setprecision(2) << price;
    return ss.str();
}

static bool isValidCalendarDate(const string& dateStr)
{
    if (dateStr.length() != 10)
    {
        return false;
    }

    if (dateStr[2] != '-' || dateStr[5] != '-')
    {
        return false;
    }

    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5) continue;
        if (!isdigit(static_cast<unsigned char>(dateStr[i])))
        {
            return false;
        }
    }

    int day = atoi(dateStr.substr(0, 2).c_str());
    int month = atoi(dateStr.substr(3, 2).c_str());
    int year = atoi(dateStr.substr(6, 4).c_str());

    if (year < 1900 || year > 2100)
    {
        return false;
    }

    if (month < 1 || month > 12)
    {
        return false;
    }

    int daysInMonth = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        daysInMonth = 30;
    }
    else if (month == 2)
    {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        daysInMonth = isLeap ? 29 : 28;
    }

    if (day < 1 || day > daysInMonth)
    {
        return false;
    }

    return true;
}

static bool isValidID(const string& str, int& outID)
{
    string t = trimString(str);
    if (t.empty())
    {
        return false;
    }

    for (size_t i = 0; i < t.length(); i++)
    {
        if (!isdigit(static_cast<unsigned char>(t[i])))
        {
            return false;
        }
    }

    char* endPtr = nullptr;
    long val = strtol(t.c_str(), &endPtr, 10);
    if (endPtr == nullptr || *endPtr != '\0')
    {
        return false;
    }

    if (val <= 0 || val > 1000000L)
    {
        return false;
    }

    outID = static_cast<int>(val);
    return true;
}

static bool isValidPrice(const string& str, double& outPrice)
{
    string t = trimString(str);
    if (t.empty())
    {
        return false;
    }

    int dotCount = 0;
    for (size_t i = 0; i < t.length(); i++)
    {
        if (t[i] == '.')
        {
            dotCount++;
            if (dotCount > 1) return false;
        }
        else if (!isdigit(static_cast<unsigned char>(t[i])))
        {
            return false;
        }
    }

    if (t == ".")
    {
        return false;
    }

    char* endPtr = nullptr;
    double val = strtod(t.c_str(), &endPtr);
    if (endPtr == nullptr || *endPtr != '\0')
    {
        return false;
    }

    if (val <= 0.0)
    {
        return false;
    }

    outPrice = val;
    return true;
}

static bool isValidQuantity(const string& str, int& outQty)
{
    string t = trimString(str);
    if (t.empty())
    {
        return false;
    }

    for (size_t i = 0; i < t.length(); i++)
    {
        if (!isdigit(static_cast<unsigned char>(t[i])))
        {
            return false;
        }
    }

    char* endPtr = nullptr;
    long val = strtol(t.c_str(), &endPtr, 10);
    if (endPtr == nullptr || *endPtr != '\0')
    {
        return false;
    }

    if (val < 0 || val > 1000000L)
    {
        return false;
    }

    outQty = static_cast<int>(val);
    return true;
}

static bool isValidPhone(const string& str, string& outPhone)
{
    string t = trimString(str);
    if (t.length() != 10)
    {
        return false;
    }

    for (size_t i = 0; i < t.length(); i++)
    {
        if (!isdigit(static_cast<unsigned char>(t[i])))
        {
            return false;
        }
    }

    outPhone = t;
    return true;
}

static bool isNonEmptyString(const string& str, string& outStr)
{
    outStr = trimString(str);
    return !outStr.empty();
}

static bool isNameMatch(const string& target, const string& query)
{
    string t = trimString(target);
    string q = trimString(query);
    if (t.empty() || q.empty()) return false;
    if (t.length() == q.length())
    {
        bool exact = true;
        for (size_t i = 0; i < t.length(); i++)
        {
            if (tolower(static_cast<unsigned char>(t[i])) != tolower(static_cast<unsigned char>(q[i])))
            {
                exact = false;
                break;
            }
        }
        if (exact) return true;
    }

    string lt = t;
    string lq = q;
    for (size_t i = 0; i < lt.length(); i++) lt[i] = tolower(static_cast<unsigned char>(lt[i]));
    for (size_t i = 0; i < lq.length(); i++) lq[i] = tolower(static_cast<unsigned char>(lq[i]));
    return (lt.find(lq) != string::npos);
}

static HWND createFormLabel(
    HWND parent,
    const char* text,
    int x,
    int y,
    int width,
    int height
)
{
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        text,
        WS_CHILD | WS_VISIBLE,
        x,
        y,
        width,
        height,
        parent,
        NULL,
        GetModuleHandleA(NULL),
        NULL
    );

    if (label != NULL)
    {
        setFont(label, 10, false);
    }

    return label;
}

static HWND createFormEdit(
    HWND parent,
    int x,
    int y,
    int width,
    int height
)
{
    HWND edit = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD |
        WS_VISIBLE |
        ES_AUTOHSCROLL,
        x,
        y,
        width,
        height,
        parent,
        NULL,
        GetModuleHandleA(NULL),
        NULL
    );

    if (edit != NULL)
    {
        setFont(edit, 10, false);
    }

    return edit;
}


/* =========================================================
   MEDICINE ADD WINDOW
   ========================================================= */

static HWND s_medAddWindow = NULL;
static HWND s_addMedId = NULL;
static HWND s_addMedName = NULL;
static HWND s_addMedCat = NULL;
static HWND s_addMedPrice = NULL;
static HWND s_addMedQty = NULL;
static HWND s_addMedExp = NULL;

static LRESULT CALLBACK MedicineAddProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            createFormLabel(window, "Medicine ID", 40, 45, 150, 25);
            s_addMedId = createFormEdit(window, 200, 40, 320, 28);

            createFormLabel(window, "Medicine Name", 40, 85, 150, 25);
            s_addMedName = createFormEdit(window, 200, 80, 320, 28);

            createFormLabel(window, "Category", 40, 125, 150, 25);
            s_addMedCat = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                200, 120, 320, 220,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_addMedCat, 10, false);
            for (int i = 0; i < MEDICINE_CATEGORY_COUNT; i++)
            {
                SendMessageA(s_addMedCat, CB_ADDSTRING, 0, (LPARAM)MEDICINE_CATEGORIES[i]);
            }
            SendMessageA(s_addMedCat, CB_SETCURSEL, 0, 0);

            createFormLabel(window, "Price (Rs.)", 40, 165, 150, 25);
            s_addMedPrice = createFormEdit(window, 200, 160, 320, 28);

            createFormLabel(window, "Quantity / Stock", 40, 205, 150, 25);
            s_addMedQty = createFormEdit(window, 200, 200, 320, 28);

            createFormLabel(window, "Expiry (DD-MM-YYYY)", 40, 245, 150, 25);
            s_addMedExp = createFormEdit(window, 200, 240, 320, 28);

            createMainButton(window, "Add Medicine", 101, 200, 290, 150, 38);
            createMainButton(window, "Cancel", 102, 370, 290, 110, 38);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);

            if (controlId == 102)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 101)
            {
                int id = 0;
                if (!isValidID(getTrimmedText(s_addMedId), id))
                {
                    MessageBoxA(window, "Please enter a valid Medicine ID (positive integer).", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedId);
                    return 0;
                }

                if (g_medicineManager != NULL && g_medicineManager->searchMedicine(id) != nullptr)
                {
                    MessageBoxA(window, "Medicine ID already exists. Each medicine must have a unique ID.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedId);
                    return 0;
                }

                string name;
                if (!isNonEmptyString(getTrimmedText(s_addMedName), name))
                {
                    MessageBoxA(window, "Medicine Name cannot be empty or whitespace only.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedName);
                    return 0;
                }

                int catSel = SendMessageA(s_addMedCat, CB_GETCURSEL, 0, 0);
                char catBuf[100] = {0};
                if (catSel != CB_ERR)
                {
                    SendMessageA(s_addMedCat, CB_GETLBTEXT, catSel, (LPARAM)catBuf);
                }
                string category = trimString(catBuf);
                if (category.empty())
                {
                    MessageBoxA(window, "Please select a valid medicine category.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedCat);
                    return 0;
                }

                double price = 0.0;
                if (!isValidPrice(getTrimmedText(s_addMedPrice), price))
                {
                    MessageBoxA(window, "Invalid Price. Price must be a numeric value greater than zero.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedPrice);
                    return 0;
                }

                int quantity = 0;
                if (!isValidQuantity(getTrimmedText(s_addMedQty), quantity))
                {
                    MessageBoxA(window, "Invalid Quantity. Quantity must be a valid non-negative integer (0 or greater).", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedQty);
                    return 0;
                }

                string expiry = getTrimmedText(s_addMedExp);
                if (!isValidCalendarDate(expiry))
                {
                    MessageBoxA(window, "Invalid Expiry Date. Please enter a real valid calendar date in exact DD-MM-YYYY format (e.g. 25-12-2027).\nCheck leap year, month (01-12) and valid calendar days.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addMedExp);
                    return 0;
                }

                Medicine med(id, name, category, price, quantity, expiry);
                if (g_medicineManager != NULL && g_medicineManager->addMedicine(med))
                {
                    if (g_dataManager != NULL)
                    {
                        g_dataManager->saveAll(*g_medicineManager, *g_customerManager, *g_billManager, *g_saleManager);
                    }
                    MessageBoxA(window, "Medicine added successfully!", "PharmaCare", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(window);
                    if (g_gui != NULL && g_gui->getMainWindow() != NULL)
                    {
                        InvalidateRect(g_gui->getMainWindow(), NULL, TRUE);
                    }
                }
                else
                {
                    MessageBoxA(window, "Failed to add medicine. Array storage may be full.", "Error", MB_OK | MB_ICONERROR);
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_medAddWindow = NULL;
            s_addMedId = NULL;
            s_addMedName = NULL;
            s_addMedCat = NULL;
            s_addMedPrice = NULL;
            s_addMedQty = NULL;
            s_addMedExp = NULL;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openMedicineAddWindow()
{
    if (s_medAddWindow != NULL)
    {
        SetForegroundWindow(s_medAddWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = MedicineAddProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareMedAdd";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_medAddWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareMedAdd",
        "Add Medicine - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        360, 120, 580, 400,
        NULL, NULL, instance, NULL
    );

    if (s_medAddWindow != NULL)
    {
        ShowWindow(s_medAddWindow, SW_SHOW);
        UpdateWindow(s_medAddWindow);
    }
}


/* =========================================================
   MEDICINE SEARCH WINDOW
   ========================================================= */

static HWND s_medSearchWindow = NULL;
static HWND s_searchMedType = NULL;
static HWND s_searchMedValue = NULL;
static HWND s_searchMedResults = NULL;

static LRESULT CALLBACK MedicineSearchProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            createFormLabel(window, "Search By:", 40, 35, 90, 25);
            s_searchMedType = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                135, 30, 170, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_searchMedType, 10, false);
            SendMessageA(s_searchMedType, CB_ADDSTRING, 0, (LPARAM)"Medicine ID");
            SendMessageA(s_searchMedType, CB_ADDSTRING, 0, (LPARAM)"Medicine Name");
            SendMessageA(s_searchMedType, CB_SETCURSEL, 0, 0);

            createFormLabel(window, "Search For:", 320, 35, 90, 25);
            s_searchMedValue = createFormEdit(window, 415, 30, 160, 28);

            createMainButton(window, "Search", 201, 590, 28, 90, 32);

            createFormLabel(window, "Search Results:", 40, 75, 200, 20);

            s_searchMedResults = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "Enter a Medicine ID or Name above and click 'Search'.",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                40, 100, 640, 240,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_searchMedResults, 10, false);

            createMainButton(window, "Close", 202, 560, 355, 120, 36);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);

            if (controlId == 202)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 201)
            {
                int typeSel = SendMessageA(s_searchMedType, CB_GETCURSEL, 0, 0);
                string query = getTrimmedText(s_searchMedValue);

                if (query.empty())
                {
                    SetWindowTextA(s_searchMedResults, "Please enter a Medicine ID or Name to search.");
                    return 0;
                }

                if (typeSel == 0) // Search by ID
                {
                    int id = 0;
                    if (!isValidID(query, id))
                    {
                        SetWindowTextA(s_searchMedResults, "Invalid Medicine ID. ID must be a positive integer.");
                        return 0;
                    }

                    if (g_medicineManager == NULL)
                    {
                        SetWindowTextA(s_searchMedResults, "Medicine manager is unavailable.");
                        return 0;
                    }

                    Medicine* med = g_medicineManager->searchMedicine(id);
                    if (med == nullptr)
                    {
                        string notFound = "No medicine found with Medicine ID: " + query;
                        SetWindowTextA(s_searchMedResults, notFound.c_str());
                    }
                    else
                    {
                        stringstream ss;
                        ss << "========================================================\r\n";
                        ss << "                   MEDICINE DETAILS                     \r\n";
                        ss << "========================================================\r\n\r\n";
                        ss << "  Medicine ID:       " << med->getMedicineID() << "\r\n";
                        ss << "  Medicine Name:     " << med->getName() << "\r\n";
                        ss << "  Category:          " << med->getCategory() << "\r\n";
                        ss << "  Price:             Rs. " << fixed << setprecision(2) << med->getPrice() << "\r\n";
                        ss << "  Stock Quantity:    " << med->getQuantity() << " units\r\n";
                        ss << "  Expiry Date:       " << med->getExpiryDate() << "\r\n";
                        ss << "========================================================\r\n";
                        SetWindowTextA(s_searchMedResults, ss.str().c_str());
                    }
                }
                else // Search by Name
                {
                    if (g_medicineManager == NULL)
                    {
                        SetWindowTextA(s_searchMedResults, "Medicine manager is unavailable.");
                        return 0;
                    }

                    vector<Medicine*> matches;
                    for (int i = 0; i < g_medicineManager->getCount(); i++)
                    {
                        Medicine* m = g_medicineManager->getMedicineAt(i);
                        if (m != NULL && isNameMatch(m->getName(), query))
                        {
                            matches.push_back(m);
                        }
                    }

                    if (matches.empty())
                    {
                        string notFound = "No medicine found matching name: \"" + query + "\"";
                        SetWindowTextA(s_searchMedResults, notFound.c_str());
                    }
                    else if (matches.size() == 1)
                    {
                        Medicine* med = matches[0];
                        stringstream ss;
                        ss << "========================================================\r\n";
                        ss << "                   MEDICINE DETAILS                     \r\n";
                        ss << "========================================================\r\n\r\n";
                        ss << "  Medicine ID:       " << med->getMedicineID() << "\r\n";
                        ss << "  Medicine Name:     " << med->getName() << "\r\n";
                        ss << "  Category:          " << med->getCategory() << "\r\n";
                        ss << "  Price:             Rs. " << fixed << setprecision(2) << med->getPrice() << "\r\n";
                        ss << "  Stock Quantity:    " << med->getQuantity() << " units\r\n";
                        ss << "  Expiry Date:       " << med->getExpiryDate() << "\r\n";
                        ss << "========================================================\r\n";
                        SetWindowTextA(s_searchMedResults, ss.str().c_str());
                    }
                    else
                    {
                        stringstream ss;
                        ss << "Found " << matches.size() << " medicines matching \"" << query << "\":\r\n\r\n";
                        for (size_t i = 0; i < matches.size(); i++)
                        {
                            Medicine* m = matches[i];
                            ss << "[" << (i + 1) << "] Medicine ID: " << m->getMedicineID()
                               << " | Name: " << m->getName()
                               << " | Category: " << m->getCategory()
                               << " | Price: Rs. " << fixed << setprecision(2) << m->getPrice()
                               << " | Stock: " << m->getQuantity()
                               << " | Expiry: " << m->getExpiryDate() << "\r\n";
                        }
                        ss << "\r\nNote: Multiple medicines share this name. To update or delete a specific record, use its Medicine ID.";
                        SetWindowTextA(s_searchMedResults, ss.str().c_str());
                    }
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_medSearchWindow = NULL;
            s_searchMedType = NULL;
            s_searchMedValue = NULL;
            s_searchMedResults = NULL;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openMedicineSearchWindow()
{
    if (s_medSearchWindow != NULL)
    {
        SetForegroundWindow(s_medSearchWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = MedicineSearchProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareMedSearch";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_medSearchWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareMedSearch",
        "Search Medicine - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        340, 110, 740, 450,
        NULL, NULL, instance, NULL
    );

    if (s_medSearchWindow != NULL)
    {
        ShowWindow(s_medSearchWindow, SW_SHOW);
        UpdateWindow(s_medSearchWindow);
    }
}


/* =========================================================
   MEDICINE UPDATE WINDOW
   ========================================================= */

static HWND s_medUpdateWindow = NULL;
static HWND s_updMedFindType = NULL;
static HWND s_updMedFindValue = NULL;
static HWND s_updMedDetails = NULL;
static HWND s_updMedFieldCombo = NULL;
static HWND s_updMedNewValue = NULL;
static HWND s_updMedCategoryCombo = NULL;
static int s_updSelectedMedID = 0;

static LRESULT CALLBACK MedicineUpdateProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            s_updSelectedMedID = 0;

            createFormLabel(window, "Find By:", 40, 35, 75, 25);
            s_updMedFindType = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                120, 30, 150, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updMedFindType, 10, false);
            SendMessageA(s_updMedFindType, CB_ADDSTRING, 0, (LPARAM)"Medicine ID");
            SendMessageA(s_updMedFindType, CB_ADDSTRING, 0, (LPARAM)"Medicine Name");
            SendMessageA(s_updMedFindType, CB_SETCURSEL, 0, 0);

            s_updMedFindValue = createFormEdit(window, 280, 30, 220, 28);
            createMainButton(window, "Find Medicine", 301, 515, 28, 140, 32);

            createFormLabel(window, "Selected Medicine Details (ID is primary key and cannot be changed):", 40, 70, 550, 20);
            s_updMedDetails = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "Search by Medicine ID or Name above to select a medicine to update.",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                40, 95, 615, 80,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updMedDetails, 10, false);

            createFormLabel(window, "Field to Update:", 40, 195, 140, 25);
            s_updMedFieldCombo = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                190, 190, 220, 180,
                window,
                (HMENU)310,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updMedFieldCombo, 10, false);
            SendMessageA(s_updMedFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Medicine Name");
            SendMessageA(s_updMedFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Category");
            SendMessageA(s_updMedFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Price");
            SendMessageA(s_updMedFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Quantity / Stock");
            SendMessageA(s_updMedFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Expiry Date");
            SendMessageA(s_updMedFieldCombo, CB_SETCURSEL, 0, 0);

            createFormLabel(window, "New Value:", 40, 240, 140, 25);
            s_updMedNewValue = createFormEdit(window, 190, 235, 280, 28);

            s_updMedCategoryCombo = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
                190, 235, 280, 220,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updMedCategoryCombo, 10, false);
            for (int i = 0; i < MEDICINE_CATEGORY_COUNT; i++)
            {
                SendMessageA(s_updMedCategoryCombo, CB_ADDSTRING, 0, (LPARAM)MEDICINE_CATEGORIES[i]);
            }
            SendMessageA(s_updMedCategoryCombo, CB_SETCURSEL, 0, 0);

            createMainButton(window, "Update Field", 302, 190, 290, 150, 38);
            createMainButton(window, "Close", 303, 360, 290, 110, 38);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);
            int notifyCode = HIWORD(wParam);

            if (controlId == 303)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 310 && notifyCode == CBN_SELCHANGE)
            {
                int fieldSel = SendMessageA(s_updMedFieldCombo, CB_GETCURSEL, 0, 0);
                if (fieldSel == 1) // Category
                {
                    ShowWindow(s_updMedNewValue, SW_HIDE);
                    ShowWindow(s_updMedCategoryCombo, SW_SHOW);
                    if (s_updSelectedMedID > 0 && g_medicineManager != NULL)
                    {
                        Medicine* m = g_medicineManager->searchMedicine(s_updSelectedMedID);
                        if (m != NULL)
                        {
                            int idx = SendMessageA(s_updMedCategoryCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)m->getCategory().c_str());
                            if (idx != CB_ERR) SendMessageA(s_updMedCategoryCombo, CB_SETCURSEL, idx, 0);
                        }
                    }
                }
                else
                {
                    ShowWindow(s_updMedCategoryCombo, SW_HIDE);
                    ShowWindow(s_updMedNewValue, SW_SHOW);
                    if (s_updSelectedMedID > 0 && g_medicineManager != NULL)
                    {
                        Medicine* m = g_medicineManager->searchMedicine(s_updSelectedMedID);
                        if (m != NULL)
                        {
                            if (fieldSel == 0) SetWindowTextA(s_updMedNewValue, m->getName().c_str());
                            else if (fieldSel == 2) SetWindowTextA(s_updMedNewValue, formatPrice(m->getPrice()).c_str());
                            else if (fieldSel == 3) SetWindowTextA(s_updMedNewValue, to_string(m->getQuantity()).c_str());
                            else if (fieldSel == 4) SetWindowTextA(s_updMedNewValue, m->getExpiryDate().c_str());
                        }
                    }
                }
                return 0;
            }

            if (controlId == 301) // Find Medicine
            {
                int typeSel = SendMessageA(s_updMedFindType, CB_GETCURSEL, 0, 0);
                string query = getTrimmedText(s_updMedFindValue);

                if (query.empty())
                {
                    MessageBoxA(window, "Please enter a Medicine ID or Name to find.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                if (g_medicineManager == NULL)
                {
                    MessageBoxA(window, "Medicine manager is unavailable.", "Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                Medicine* foundMed = nullptr;

                if (typeSel == 0) // By ID
                {
                    int id = 0;
                    if (!isValidID(query, id))
                    {
                        MessageBoxA(window, "Invalid Medicine ID. ID must be a positive integer.", "Validation Error", MB_OK | MB_ICONWARNING);
                        return 0;
                    }

                    foundMed = g_medicineManager->searchMedicine(id);
                    if (foundMed == nullptr)
                    {
                        MessageBoxA(window, "Medicine ID was not found.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_updSelectedMedID = 0;
                        SetWindowTextA(s_updMedDetails, "Medicine not found. Please try another ID or Name.");
                        return 0;
                    }
                }
                else // By Name
                {
                    vector<Medicine*> matches;
                    for (int i = 0; i < g_medicineManager->getCount(); i++)
                    {
                        Medicine* m = g_medicineManager->getMedicineAt(i);
                        if (m != NULL && isNameMatch(m->getName(), query))
                        {
                            matches.push_back(m);
                        }
                    }

                    if (matches.empty())
                    {
                        MessageBoxA(window, "No medicine found matching that name.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_updSelectedMedID = 0;
                        SetWindowTextA(s_updMedDetails, "Medicine not found. Please try another Name or ID.");
                        return 0;
                    }
                    else if (matches.size() > 1)
                    {
                        stringstream ss;
                        ss << "Multiple medicines share this name:\r\n";
                        for (size_t i = 0; i < matches.size(); i++)
                        {
                            ss << "ID: " << matches[i]->getMedicineID()
                               << " | " << matches[i]->getName()
                               << " | " << matches[i]->getCategory()
                               << " | Stock: " << matches[i]->getQuantity() << "\r\n";
                        }
                        ss << "\r\nPlease search by Medicine ID above to choose the exact record to update.";
                        SetWindowTextA(s_updMedDetails, ss.str().c_str());
                        s_updSelectedMedID = 0;
                        return 0;
                    }
                    else
                    {
                        foundMed = matches[0];
                    }
                }

                s_updSelectedMedID = foundMed->getMedicineID();

                stringstream ss;
                ss << "ID: " << foundMed->getMedicineID()
                   << " | Name: " << foundMed->getName()
                   << " | Category: " << foundMed->getCategory()
                   << " | Price: Rs. " << fixed << setprecision(2) << foundMed->getPrice()
                   << " | Stock: " << foundMed->getQuantity()
                   << " | Expiry: " << foundMed->getExpiryDate();
                SetWindowTextA(s_updMedDetails, ss.str().c_str());

                // Refresh input box for current field selection
                SendMessageA(window, WM_COMMAND, MAKEWPARAM(310, CBN_SELCHANGE), (LPARAM)s_updMedFieldCombo);

                return 0;
            }

            if (controlId == 302) // Update Field
            {
                if (s_updSelectedMedID <= 0 || g_medicineManager == NULL)
                {
                    MessageBoxA(window, "Please find and select an existing medicine first.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                Medicine* med = g_medicineManager->searchMedicine(s_updSelectedMedID);
                if (med == nullptr)
                {
                    MessageBoxA(window, "Selected medicine no longer exists.", "Error", MB_OK | MB_ICONERROR);
                    s_updSelectedMedID = 0;
                    return 0;
                }

                int fieldSel = SendMessageA(s_updMedFieldCombo, CB_GETCURSEL, 0, 0);

                if (fieldSel == 0) // Name
                {
                    string newName;
                    if (!isNonEmptyString(getTrimmedText(s_updMedNewValue), newName))
                    {
                        MessageBoxA(window, "Medicine Name cannot be empty or whitespace only.", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updMedNewValue);
                        return 0;
                    }
                    med->setName(newName);
                }
                else if (fieldSel == 1) // Category
                {
                    int cSel = SendMessageA(s_updMedCategoryCombo, CB_GETCURSEL, 0, 0);
                    char cBuf[100] = {0};
                    if (cSel != CB_ERR)
                    {
                        SendMessageA(s_updMedCategoryCombo, CB_GETLBTEXT, cSel, (LPARAM)cBuf);
                    }
                    string newCat = trimString(cBuf);
                    if (newCat.empty())
                    {
                        MessageBoxA(window, "Please select a valid category.", "Validation Error", MB_OK | MB_ICONWARNING);
                        return 0;
                    }
                    med->setCategory(newCat);
                }
                else if (fieldSel == 2) // Price
                {
                    double newPrice = 0.0;
                    if (!isValidPrice(getTrimmedText(s_updMedNewValue), newPrice))
                    {
                        MessageBoxA(window, "Invalid Price. Must be a numeric value greater than zero.", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updMedNewValue);
                        return 0;
                    }
                    med->setPrice(newPrice);
                }
                else if (fieldSel == 3) // Quantity / Stock
                {
                    int newQty = 0;
                    if (!isValidQuantity(getTrimmedText(s_updMedNewValue), newQty))
                    {
                        MessageBoxA(window, "Invalid Quantity. Must be a valid non-negative integer.", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updMedNewValue);
                        return 0;
                    }
                    med->setQuantity(newQty);
                }
                else if (fieldSel == 4) // Expiry Date
                {
                    string newExp = getTrimmedText(s_updMedNewValue);
                    if (!isValidCalendarDate(newExp))
                    {
                        MessageBoxA(window, "Invalid Expiry Date. Please enter a real valid calendar date in exact DD-MM-YYYY format (e.g. 25-12-2027).", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updMedNewValue);
                        return 0;
                    }
                    med->setExpiryDate(newExp);
                }

                if (g_dataManager != NULL)
                {
                    g_dataManager->saveAll(*g_medicineManager, *g_customerManager, *g_billManager, *g_saleManager);
                }

                stringstream ss;
                ss << "ID: " << med->getMedicineID()
                   << " | Name: " << med->getName()
                   << " | Category: " << med->getCategory()
                   << " | Price: Rs. " << fixed << setprecision(2) << med->getPrice()
                   << " | Stock: " << med->getQuantity()
                   << " | Expiry: " << med->getExpiryDate();
                SetWindowTextA(s_updMedDetails, ss.str().c_str());

                MessageBoxA(window, "Medicine updated successfully!", "PharmaCare", MB_OK | MB_ICONINFORMATION);

                if (g_gui != NULL && g_gui->getMainWindow() != NULL)
                {
                    InvalidateRect(g_gui->getMainWindow(), NULL, TRUE);
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_medUpdateWindow = NULL;
            s_updMedFindType = NULL;
            s_updMedFindValue = NULL;
            s_updMedDetails = NULL;
            s_updMedFieldCombo = NULL;
            s_updMedNewValue = NULL;
            s_updMedCategoryCombo = NULL;
            s_updSelectedMedID = 0;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openMedicineUpdateWindow()
{
    if (s_medUpdateWindow != NULL)
    {
        SetForegroundWindow(s_medUpdateWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = MedicineUpdateProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareMedUpdate";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_medUpdateWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareMedUpdate",
        "Update Medicine - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        340, 110, 710, 390,
        NULL, NULL, instance, NULL
    );

    if (s_medUpdateWindow != NULL)
    {
        ShowWindow(s_medUpdateWindow, SW_SHOW);
        UpdateWindow(s_medUpdateWindow);
    }
}


/* =========================================================
   MEDICINE DELETE WINDOW
   ========================================================= */

static HWND s_medDeleteWindow = NULL;
static HWND s_delMedFindType = NULL;
static HWND s_delMedFindValue = NULL;
static HWND s_delMedDetails = NULL;
static int s_delSelectedMedID = 0;

static LRESULT CALLBACK MedicineDeleteProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            s_delSelectedMedID = 0;

            createFormLabel(window, "Find By:", 40, 35, 75, 25);
            s_delMedFindType = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                120, 30, 150, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_delMedFindType, 10, false);
            SendMessageA(s_delMedFindType, CB_ADDSTRING, 0, (LPARAM)"Medicine ID");
            SendMessageA(s_delMedFindType, CB_ADDSTRING, 0, (LPARAM)"Medicine Name");
            SendMessageA(s_delMedFindType, CB_SETCURSEL, 0, 0);

            s_delMedFindValue = createFormEdit(window, 280, 30, 220, 28);
            createMainButton(window, "Find Medicine", 401, 515, 28, 140, 32);

            createFormLabel(window, "Medicine Record Details:", 40, 70, 250, 20);
            s_delMedDetails = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "Search by Medicine ID or Name above to find the medicine to delete.",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                40, 95, 615, 140,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_delMedDetails, 10, false);

            createMainButton(window, "Delete Medicine", 402, 190, 255, 160, 38);
            createMainButton(window, "Cancel", 403, 370, 255, 110, 38);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);

            if (controlId == 403)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 401) // Find Medicine
            {
                int typeSel = SendMessageA(s_delMedFindType, CB_GETCURSEL, 0, 0);
                string query = getTrimmedText(s_delMedFindValue);

                if (query.empty())
                {
                    MessageBoxA(window, "Please enter a Medicine ID or Name to find.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                if (g_medicineManager == NULL)
                {
                    MessageBoxA(window, "Medicine manager is unavailable.", "Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                Medicine* foundMed = nullptr;

                if (typeSel == 0) // By ID
                {
                    int id = 0;
                    if (!isValidID(query, id))
                    {
                        MessageBoxA(window, "Invalid Medicine ID. ID must be a positive integer.", "Validation Error", MB_OK | MB_ICONWARNING);
                        return 0;
                    }

                    foundMed = g_medicineManager->searchMedicine(id);
                    if (foundMed == nullptr)
                    {
                        MessageBoxA(window, "Medicine ID was not found.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_delSelectedMedID = 0;
                        SetWindowTextA(s_delMedDetails, "Medicine not found. Please try another ID or Name.");
                        return 0;
                    }
                }
                else // By Name
                {
                    vector<Medicine*> matches;
                    for (int i = 0; i < g_medicineManager->getCount(); i++)
                    {
                        Medicine* m = g_medicineManager->getMedicineAt(i);
                        if (m != NULL && isNameMatch(m->getName(), query))
                        {
                            matches.push_back(m);
                        }
                    }

                    if (matches.empty())
                    {
                        MessageBoxA(window, "No medicine found matching that name.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_delSelectedMedID = 0;
                        SetWindowTextA(s_delMedDetails, "Medicine not found. Please try another Name or ID.");
                        return 0;
                    }
                    else if (matches.size() > 1)
                    {
                        stringstream ss;
                        ss << "Multiple medicines share this name:\r\n\r\n";
                        for (size_t i = 0; i < matches.size(); i++)
                        {
                            ss << "ID: " << matches[i]->getMedicineID()
                               << " | Name: " << matches[i]->getName()
                               << " | Category: " << matches[i]->getCategory()
                               << " | Stock: " << matches[i]->getQuantity() << "\r\n";
                        }
                        ss << "\r\nPlease search by Medicine ID above to specify the exact record to delete.";
                        SetWindowTextA(s_delMedDetails, ss.str().c_str());
                        s_delSelectedMedID = 0;
                        return 0;
                    }
                    else
                    {
                        foundMed = matches[0];
                    }
                }

                s_delSelectedMedID = foundMed->getMedicineID();

                stringstream ss;
                ss << "========================================================\r\n";
                ss << "              MEDICINE RECORD FOUND                     \r\n";
                ss << "========================================================\r\n\r\n";
                ss << "  Medicine ID:       " << foundMed->getMedicineID() << "\r\n";
                ss << "  Medicine Name:     " << foundMed->getName() << "\r\n";
                ss << "  Category:          " << foundMed->getCategory() << "\r\n";
                ss << "  Price:             Rs. " << fixed << setprecision(2) << foundMed->getPrice() << "\r\n";
                ss << "  Stock Quantity:    " << foundMed->getQuantity() << " units\r\n";
                ss << "  Expiry Date:       " << foundMed->getExpiryDate() << "\r\n";
                ss << "========================================================\r\n\r\n";
                ss << "Click 'Delete Medicine' below to confirm removal.";
                SetWindowTextA(s_delMedDetails, ss.str().c_str());

                return 0;
            }

            if (controlId == 402) // Delete Medicine
            {
                if (s_delSelectedMedID <= 0 || g_medicineManager == NULL)
                {
                    MessageBoxA(window, "Please find and select a medicine record first.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                Medicine* med = g_medicineManager->searchMedicine(s_delSelectedMedID);
                string confirmMsg = "Are you sure you want to permanently delete this medicine record?\n\n";
                confirmMsg += "Medicine ID: " + to_string(s_delSelectedMedID) + "\n";
                if (med != NULL)
                {
                    confirmMsg += "Name: " + med->getName() + "\n";
                    confirmMsg += "Category: " + med->getCategory() + "\n";
                }

                int answer = MessageBoxA(window, confirmMsg.c_str(), "Confirm Medicine Deletion", MB_YESNO | MB_ICONQUESTION);
                if (answer == IDYES)
                {
                    if (g_medicineManager->deleteMedicine(s_delSelectedMedID))
                    {
                        if (g_dataManager != NULL)
                        {
                            g_dataManager->saveAll(*g_medicineManager, *g_customerManager, *g_billManager, *g_saleManager);
                        }
                        MessageBoxA(window, "Medicine deleted successfully.", "PharmaCare", MB_OK | MB_ICONINFORMATION);
                        s_delSelectedMedID = 0;
                        DestroyWindow(window);
                        if (g_gui != NULL && g_gui->getMainWindow() != NULL)
                        {
                            InvalidateRect(g_gui->getMainWindow(), NULL, TRUE);
                        }
                    }
                    else
                    {
                        MessageBoxA(window, "Failed to delete medicine.", "Error", MB_OK | MB_ICONERROR);
                    }
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_medDeleteWindow = NULL;
            s_delMedFindType = NULL;
            s_delMedFindValue = NULL;
            s_delMedDetails = NULL;
            s_delSelectedMedID = 0;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openMedicineDeleteWindow()
{
    if (s_medDeleteWindow != NULL)
    {
        SetForegroundWindow(s_medDeleteWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = MedicineDeleteProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareMedDelete";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_medDeleteWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareMedDelete",
        "Delete Medicine - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        340, 110, 710, 360,
        NULL, NULL, instance, NULL
    );

    if (s_medDeleteWindow != NULL)
    {
        ShowWindow(s_medDeleteWindow, SW_SHOW);
        UpdateWindow(s_medDeleteWindow);
    }
}

void GUI::addMedicineWindow()
{
    openMedicineAddWindow();
}

void GUI::updateMedicineWindow()
{
    openMedicineUpdateWindow();
}

void GUI::deleteMedicineWindow()
{
    openMedicineDeleteWindow();
}

void GUI::searchMedicineWindow()
{
    openMedicineSearchWindow();
}

/* =========================================================
   SHOW CURRENT BILL
   ========================================================= */

void GUI::showCurrentBill()
{
    /*
       Full printable bill viewer is implemented
       in Part 3.
    */
}


/* =========================================================
   SAVE DATA
   ========================================================= */

void GUI::saveData()
{
    if (dataManager != NULL &&
        medicineManager != NULL &&
        customerManager != NULL &&
        billManager != NULL &&
        saleManager != NULL)
    {
        dataManager->saveAll(
            *medicineManager,
            *customerManager,
            *billManager,
            *saleManager
        );
    }
}
// ============================================================
// PHARMACARE GUI - PART 2
// CUSTOMER MODULE
// ============================================================

/* =========================================================
   CUSTOMER ADD WINDOW
   ========================================================= */

static HWND s_custAddWindow = NULL;
static HWND s_addCustId = NULL;
static HWND s_addCustName = NULL;
static HWND s_addCustPhone = NULL;
static HWND s_addCustAddr = NULL;

static LRESULT CALLBACK CustomerAddProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            createFormLabel(window, "Customer ID", 40, 45, 140, 25);
            s_addCustId = createFormEdit(window, 190, 40, 300, 28);

            createFormLabel(window, "Customer Name", 40, 85, 140, 25);
            s_addCustName = createFormEdit(window, 190, 80, 300, 28);

            createFormLabel(window, "Phone (10 Digits)", 40, 125, 140, 25);
            s_addCustPhone = createFormEdit(window, 190, 120, 300, 28);

            createFormLabel(window, "Address", 40, 165, 140, 25);
            s_addCustAddr = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                190, 160, 300, 28,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_addCustAddr, 10, false);

            createMainButton(window, "Add Customer", 501, 190, 215, 150, 38);
            createMainButton(window, "Cancel", 502, 360, 215, 110, 38);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);

            if (controlId == 502)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 501)
            {
                int id = 0;
                if (!isValidID(getTrimmedText(s_addCustId), id))
                {
                    MessageBoxA(window, "Please enter a valid Customer ID (positive integer).", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addCustId);
                    return 0;
                }

                if (g_customerManager != NULL && g_customerManager->searchCustomer(id) != nullptr)
                {
                    MessageBoxA(window, "Customer ID already exists. Each customer must have a unique ID.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addCustId);
                    return 0;
                }

                string name;
                if (!isNonEmptyString(getTrimmedText(s_addCustName), name))
                {
                    MessageBoxA(window, "Customer Name cannot be empty or whitespace only.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addCustName);
                    return 0;
                }

                string phone;
                if (!isValidPhone(getTrimmedText(s_addCustPhone), phone))
                {
                    MessageBoxA(window, "Invalid Phone Number. Must contain exactly 10 digits without letters, spaces, or symbols.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addCustPhone);
                    return 0;
                }

                string address;
                if (!isNonEmptyString(getTrimmedText(s_addCustAddr), address))
                {
                    MessageBoxA(window, "Customer Address cannot be empty or whitespace only.", "Validation Error", MB_OK | MB_ICONWARNING);
                    SetFocus(s_addCustAddr);
                    return 0;
                }

                Customer customer(id, name, phone, address);
                if (g_customerManager != NULL && g_customerManager->addCustomer(customer))
                {
                    if (g_dataManager != NULL)
                    {
                        g_dataManager->saveAll(*g_medicineManager, *g_customerManager, *g_billManager, *g_saleManager);
                    }
                    MessageBoxA(window, "Customer added successfully!", "PharmaCare", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(window);
                    if (g_gui != NULL && g_gui->getMainWindow() != NULL)
                    {
                        InvalidateRect(g_gui->getMainWindow(), NULL, TRUE);
                    }
                }
                else
                {
                    MessageBoxA(window, "Failed to add customer. Storage might be full.", "Error", MB_OK | MB_ICONERROR);
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_custAddWindow = NULL;
            s_addCustId = NULL;
            s_addCustName = NULL;
            s_addCustPhone = NULL;
            s_addCustAddr = NULL;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openCustomerAddWindow()
{
    if (s_custAddWindow != NULL)
    {
        SetForegroundWindow(s_custAddWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = CustomerAddProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareCustAdd";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_custAddWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareCustAdd",
        "Add Customer - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        360, 120, 560, 320,
        NULL, NULL, instance, NULL
    );

    if (s_custAddWindow != NULL)
    {
        ShowWindow(s_custAddWindow, SW_SHOW);
        UpdateWindow(s_custAddWindow);
    }
}


/* =========================================================
   CUSTOMER SEARCH WINDOW
   ========================================================= */

static HWND s_custSearchWindow = NULL;
static HWND s_searchCustType = NULL;
static HWND s_searchCustValue = NULL;
static HWND s_searchCustResults = NULL;

static LRESULT CALLBACK CustomerSearchProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            createFormLabel(window, "Search By:", 40, 35, 90, 25);
            s_searchCustType = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                135, 30, 170, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_searchCustType, 10, false);
            SendMessageA(s_searchCustType, CB_ADDSTRING, 0, (LPARAM)"Customer ID");
            SendMessageA(s_searchCustType, CB_ADDSTRING, 0, (LPARAM)"Customer Name");
            SendMessageA(s_searchCustType, CB_SETCURSEL, 0, 0);

            createFormLabel(window, "Search For:", 320, 35, 90, 25);
            s_searchCustValue = createFormEdit(window, 415, 30, 160, 28);

            createMainButton(window, "Search", 601, 590, 28, 90, 32);

            createFormLabel(window, "Search Results:", 40, 75, 200, 20);

            s_searchCustResults = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "Enter a Customer ID or Name above and click 'Search'.",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                40, 100, 640, 240,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_searchCustResults, 10, false);

            createMainButton(window, "Close", 602, 560, 355, 120, 36);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);

            if (controlId == 602)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 601)
            {
                int typeSel = SendMessageA(s_searchCustType, CB_GETCURSEL, 0, 0);
                string query = getTrimmedText(s_searchCustValue);

                if (query.empty())
                {
                    SetWindowTextA(s_searchCustResults, "Please enter a Customer ID or Name to search.");
                    return 0;
                }

                if (g_customerManager == NULL)
                {
                    SetWindowTextA(s_searchCustResults, "Customer manager is unavailable.");
                    return 0;
                }

                if (typeSel == 0) // By ID
                {
                    int id = 0;
                    if (!isValidID(query, id))
                    {
                        SetWindowTextA(s_searchCustResults, "Invalid Customer ID. ID must be a positive integer.");
                        return 0;
                    }

                    Customer* c = g_customerManager->searchCustomer(id);
                    if (c == nullptr)
                    {
                        string notFound = "No customer found with Customer ID: " + query;
                        SetWindowTextA(s_searchCustResults, notFound.c_str());
                    }
                    else
                    {
                        stringstream ss;
                        ss << "========================================================\r\n";
                        ss << "                   CUSTOMER DETAILS                     \r\n";
                        ss << "========================================================\r\n\r\n";
                        ss << "  Customer ID:       " << c->getCustomerID() << "\r\n";
                        ss << "  Customer Name:     " << c->getName() << "\r\n";
                        ss << "  Phone Number:      " << c->getPhone() << "\r\n";
                        ss << "  Address:           " << c->getAddress() << "\r\n";
                        ss << "========================================================\r\n";
                        SetWindowTextA(s_searchCustResults, ss.str().c_str());
                    }
                }
                else // By Name
                {
                    vector<Customer*> matches;
                    for (int i = 0; i < g_customerManager->getCount(); i++)
                    {
                        Customer* c = g_customerManager->getCustomerAt(i);
                        if (c != NULL && isNameMatch(c->getName(), query))
                        {
                            matches.push_back(c);
                        }
                    }

                    if (matches.empty())
                    {
                        string notFound = "No customer found matching name: \"" + query + "\"";
                        SetWindowTextA(s_searchCustResults, notFound.c_str());
                    }
                    else if (matches.size() == 1)
                    {
                        Customer* c = matches[0];
                        stringstream ss;
                        ss << "========================================================\r\n";
                        ss << "                   CUSTOMER DETAILS                     \r\n";
                        ss << "========================================================\r\n\r\n";
                        ss << "  Customer ID:       " << c->getCustomerID() << "\r\n";
                        ss << "  Customer Name:     " << c->getName() << "\r\n";
                        ss << "  Phone Number:      " << c->getPhone() << "\r\n";
                        ss << "  Address:           " << c->getAddress() << "\r\n";
                        ss << "========================================================\r\n";
                        SetWindowTextA(s_searchCustResults, ss.str().c_str());
                    }
                    else
                    {
                        stringstream ss;
                        ss << "Found " << matches.size() << " customers matching \"" << query << "\":\r\n\r\n";
                        for (size_t i = 0; i < matches.size(); i++)
                        {
                            Customer* c = matches[i];
                            ss << "[" << (i + 1) << "] Customer ID: " << c->getCustomerID()
                               << " | Name: " << c->getName()
                               << " | Phone: " << c->getPhone()
                               << " | Address: " << c->getAddress() << "\r\n";
                        }
                        ss << "\r\nNote: Multiple customers share this name. To update or delete a specific customer, use Customer ID.";
                        SetWindowTextA(s_searchCustResults, ss.str().c_str());
                    }
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_custSearchWindow = NULL;
            s_searchCustType = NULL;
            s_searchCustValue = NULL;
            s_searchCustResults = NULL;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openCustomerSearchWindow()
{
    if (s_custSearchWindow != NULL)
    {
        SetForegroundWindow(s_custSearchWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = CustomerSearchProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareCustSearch";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_custSearchWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareCustSearch",
        "Search Customer - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        340, 110, 740, 450,
        NULL, NULL, instance, NULL
    );

    if (s_custSearchWindow != NULL)
    {
        ShowWindow(s_custSearchWindow, SW_SHOW);
        UpdateWindow(s_custSearchWindow);
    }
}


/* =========================================================
   CUSTOMER UPDATE WINDOW
   ========================================================= */

static HWND s_custUpdateWindow = NULL;
static HWND s_updCustFindType = NULL;
static HWND s_updCustFindValue = NULL;
static HWND s_updCustDetails = NULL;
static HWND s_updCustFieldCombo = NULL;
static HWND s_updCustNewValue = NULL;
static int s_updSelectedCustID = 0;

static LRESULT CALLBACK CustomerUpdateProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            s_updSelectedCustID = 0;

            createFormLabel(window, "Find By:", 40, 35, 75, 25);
            s_updCustFindType = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                120, 30, 150, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updCustFindType, 10, false);
            SendMessageA(s_updCustFindType, CB_ADDSTRING, 0, (LPARAM)"Customer ID");
            SendMessageA(s_updCustFindType, CB_ADDSTRING, 0, (LPARAM)"Customer Name");
            SendMessageA(s_updCustFindType, CB_SETCURSEL, 0, 0);

            s_updCustFindValue = createFormEdit(window, 280, 30, 220, 28);
            createMainButton(window, "Find Customer", 701, 515, 28, 140, 32);

            createFormLabel(window, "Selected Customer Details (ID is primary key and cannot be changed):", 40, 70, 550, 20);
            s_updCustDetails = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "Search by Customer ID or Name above to select a customer to update.",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                40, 95, 615, 75,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updCustDetails, 10, false);

            createFormLabel(window, "Field to Update:", 40, 185, 140, 25);
            s_updCustFieldCombo = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                190, 180, 220, 150,
                window,
                (HMENU)710,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_updCustFieldCombo, 10, false);
            SendMessageA(s_updCustFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Customer Name");
            SendMessageA(s_updCustFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Phone Number");
            SendMessageA(s_updCustFieldCombo, CB_ADDSTRING, 0, (LPARAM)"Address");
            SendMessageA(s_updCustFieldCombo, CB_SETCURSEL, 0, 0);

            createFormLabel(window, "New Value:", 40, 230, 140, 25);
            s_updCustNewValue = createFormEdit(window, 190, 225, 280, 28);

            createMainButton(window, "Update Field", 702, 190, 280, 150, 38);
            createMainButton(window, "Close", 703, 360, 280, 110, 38);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);
            int notifyCode = HIWORD(wParam);

            if (controlId == 703)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 710 && notifyCode == CBN_SELCHANGE)
            {
                if (s_updSelectedCustID > 0 && g_customerManager != NULL)
                {
                    Customer* c = g_customerManager->searchCustomer(s_updSelectedCustID);
                    if (c != NULL)
                    {
                        int sel = SendMessageA(s_updCustFieldCombo, CB_GETCURSEL, 0, 0);
                        if (sel == 0) SetWindowTextA(s_updCustNewValue, c->getName().c_str());
                        else if (sel == 1) SetWindowTextA(s_updCustNewValue, c->getPhone().c_str());
                        else if (sel == 2) SetWindowTextA(s_updCustNewValue, c->getAddress().c_str());
                    }
                }
                return 0;
            }

            if (controlId == 701) // Find Customer
            {
                int typeSel = SendMessageA(s_updCustFindType, CB_GETCURSEL, 0, 0);
                string query = getTrimmedText(s_updCustFindValue);

                if (query.empty())
                {
                    MessageBoxA(window, "Please enter a Customer ID or Name to find.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                if (g_customerManager == NULL)
                {
                    MessageBoxA(window, "Customer manager is unavailable.", "Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                Customer* foundCust = nullptr;

                if (typeSel == 0) // By ID
                {
                    int id = 0;
                    if (!isValidID(query, id))
                    {
                        MessageBoxA(window, "Invalid Customer ID. ID must be a positive integer.", "Validation Error", MB_OK | MB_ICONWARNING);
                        return 0;
                    }

                    foundCust = g_customerManager->searchCustomer(id);
                    if (foundCust == nullptr)
                    {
                        MessageBoxA(window, "Customer ID was not found.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_updSelectedCustID = 0;
                        SetWindowTextA(s_updCustDetails, "Customer not found. Please try another ID or Name.");
                        return 0;
                    }
                }
                else // By Name
                {
                    vector<Customer*> matches;
                    for (int i = 0; i < g_customerManager->getCount(); i++)
                    {
                        Customer* c = g_customerManager->getCustomerAt(i);
                        if (c != NULL && isNameMatch(c->getName(), query))
                        {
                            matches.push_back(c);
                        }
                    }

                    if (matches.empty())
                    {
                        MessageBoxA(window, "No customer found matching that name.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_updSelectedCustID = 0;
                        SetWindowTextA(s_updCustDetails, "Customer not found. Please try another Name or ID.");
                        return 0;
                    }
                    else if (matches.size() > 1)
                    {
                        stringstream ss;
                        ss << "Multiple customers share this name:\r\n";
                        for (size_t i = 0; i < matches.size(); i++)
                        {
                            ss << "ID: " << matches[i]->getCustomerID()
                               << " | Name: " << matches[i]->getName()
                               << " | Phone: " << matches[i]->getPhone()
                               << " | Address: " << matches[i]->getAddress() << "\r\n";
                        }
                        ss << "\r\nPlease search by Customer ID above to choose the exact record to update.";
                        SetWindowTextA(s_updCustDetails, ss.str().c_str());
                        s_updSelectedCustID = 0;
                        return 0;
                    }
                    else
                    {
                        foundCust = matches[0];
                    }
                }

                s_updSelectedCustID = foundCust->getCustomerID();

                stringstream ss;
                ss << "ID: " << foundCust->getCustomerID()
                   << " | Name: " << foundCust->getName()
                   << " | Phone: " << foundCust->getPhone()
                   << " | Address: " << foundCust->getAddress();
                SetWindowTextA(s_updCustDetails, ss.str().c_str());

                // Pre-fill input value
                SendMessageA(window, WM_COMMAND, MAKEWPARAM(710, CBN_SELCHANGE), (LPARAM)s_updCustFieldCombo);

                return 0;
            }

            if (controlId == 702) // Update Field
            {
                if (s_updSelectedCustID <= 0 || g_customerManager == NULL)
                {
                    MessageBoxA(window, "Please find and select an existing customer first.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                Customer* c = g_customerManager->searchCustomer(s_updSelectedCustID);
                if (c == nullptr)
                {
                    MessageBoxA(window, "Selected customer no longer exists.", "Error", MB_OK | MB_ICONERROR);
                    s_updSelectedCustID = 0;
                    return 0;
                }

                int fieldSel = SendMessageA(s_updCustFieldCombo, CB_GETCURSEL, 0, 0);

                if (fieldSel == 0) // Name
                {
                    string newName;
                    if (!isNonEmptyString(getTrimmedText(s_updCustNewValue), newName))
                    {
                        MessageBoxA(window, "Customer Name cannot be empty or whitespace only.", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updCustNewValue);
                        return 0;
                    }
                    c->setName(newName);
                }
                else if (fieldSel == 1) // Phone
                {
                    string newPhone;
                    if (!isValidPhone(getTrimmedText(s_updCustNewValue), newPhone))
                    {
                        MessageBoxA(window, "Invalid Phone Number. Must contain exactly 10 digits without letters, spaces, or symbols.", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updCustNewValue);
                        return 0;
                    }
                    c->setPhone(newPhone);
                }
                else if (fieldSel == 2) // Address
                {
                    string newAddr;
                    if (!isNonEmptyString(getTrimmedText(s_updCustNewValue), newAddr))
                    {
                        MessageBoxA(window, "Customer Address cannot be empty or whitespace only.", "Validation Error", MB_OK | MB_ICONWARNING);
                        SetFocus(s_updCustNewValue);
                        return 0;
                    }
                    c->setAddress(newAddr);
                }

                if (g_dataManager != NULL)
                {
                    g_dataManager->saveAll(*g_medicineManager, *g_customerManager, *g_billManager, *g_saleManager);
                }

                stringstream ss;
                ss << "ID: " << c->getCustomerID()
                   << " | Name: " << c->getName()
                   << " | Phone: " << c->getPhone()
                   << " | Address: " << c->getAddress();
                SetWindowTextA(s_updCustDetails, ss.str().c_str());

                MessageBoxA(window, "Customer updated successfully!", "PharmaCare", MB_OK | MB_ICONINFORMATION);

                if (g_gui != NULL && g_gui->getMainWindow() != NULL)
                {
                    InvalidateRect(g_gui->getMainWindow(), NULL, TRUE);
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_custUpdateWindow = NULL;
            s_updCustFindType = NULL;
            s_updCustFindValue = NULL;
            s_updCustDetails = NULL;
            s_updCustFieldCombo = NULL;
            s_updCustNewValue = NULL;
            s_updSelectedCustID = 0;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openCustomerUpdateWindow()
{
    if (s_custUpdateWindow != NULL)
    {
        SetForegroundWindow(s_custUpdateWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = CustomerUpdateProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareCustUpdate";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_custUpdateWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareCustUpdate",
        "Update Customer - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        340, 110, 710, 380,
        NULL, NULL, instance, NULL
    );

    if (s_custUpdateWindow != NULL)
    {
        ShowWindow(s_custUpdateWindow, SW_SHOW);
        UpdateWindow(s_custUpdateWindow);
    }
}


/* =========================================================
   CUSTOMER DELETE WINDOW
   ========================================================= */

static HWND s_custDeleteWindow = NULL;
static HWND s_delCustFindType = NULL;
static HWND s_delCustFindValue = NULL;
static HWND s_delCustDetails = NULL;
static int s_delSelectedCustID = 0;

static LRESULT CALLBACK CustomerDeleteProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            s_delSelectedCustID = 0;

            createFormLabel(window, "Find By:", 40, 35, 75, 25);
            s_delCustFindType = CreateWindowExA(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                120, 30, 150, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_delCustFindType, 10, false);
            SendMessageA(s_delCustFindType, CB_ADDSTRING, 0, (LPARAM)"Customer ID");
            SendMessageA(s_delCustFindType, CB_ADDSTRING, 0, (LPARAM)"Customer Name");
            SendMessageA(s_delCustFindType, CB_SETCURSEL, 0, 0);

            s_delCustFindValue = createFormEdit(window, 280, 30, 220, 28);
            createMainButton(window, "Find Customer", 801, 515, 28, 140, 32);

            createFormLabel(window, "Customer Record Details:", 40, 70, 250, 20);
            s_delCustDetails = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "Search by Customer ID or Name above to find the customer to delete.",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                40, 95, 615, 120,
                window,
                NULL,
                GetModuleHandleA(NULL),
                NULL
            );
            setFont(s_delCustDetails, 10, false);

            createMainButton(window, "Delete Customer", 802, 190, 235, 160, 38);
            createMainButton(window, "Cancel", 803, 370, 235, 110, 38);

            return 0;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);

            if (controlId == 803)
            {
                DestroyWindow(window);
                return 0;
            }

            if (controlId == 801) // Find Customer
            {
                int typeSel = SendMessageA(s_delCustFindType, CB_GETCURSEL, 0, 0);
                string query = getTrimmedText(s_delCustFindValue);

                if (query.empty())
                {
                    MessageBoxA(window, "Please enter a Customer ID or Name to find.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                if (g_customerManager == NULL)
                {
                    MessageBoxA(window, "Customer manager is unavailable.", "Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                Customer* foundCust = nullptr;

                if (typeSel == 0) // By ID
                {
                    int id = 0;
                    if (!isValidID(query, id))
                    {
                        MessageBoxA(window, "Invalid Customer ID. ID must be a positive integer.", "Validation Error", MB_OK | MB_ICONWARNING);
                        return 0;
                    }

                    foundCust = g_customerManager->searchCustomer(id);
                    if (foundCust == nullptr)
                    {
                        MessageBoxA(window, "Customer ID was not found.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_delSelectedCustID = 0;
                        SetWindowTextA(s_delCustDetails, "Customer not found. Please try another ID or Name.");
                        return 0;
                    }
                }
                else // By Name
                {
                    vector<Customer*> matches;
                    for (int i = 0; i < g_customerManager->getCount(); i++)
                    {
                        Customer* c = g_customerManager->getCustomerAt(i);
                        if (c != NULL && isNameMatch(c->getName(), query))
                        {
                            matches.push_back(c);
                        }
                    }

                    if (matches.empty())
                    {
                        MessageBoxA(window, "No customer found matching that name.", "Not Found", MB_OK | MB_ICONWARNING);
                        s_delSelectedCustID = 0;
                        SetWindowTextA(s_delCustDetails, "Customer not found. Please try another Name or ID.");
                        return 0;
                    }
                    else if (matches.size() > 1)
                    {
                        stringstream ss;
                        ss << "Multiple customers share this name:\r\n\r\n";
                        for (size_t i = 0; i < matches.size(); i++)
                        {
                            ss << "ID: " << matches[i]->getCustomerID()
                               << " | Name: " << matches[i]->getName()
                               << " | Phone: " << matches[i]->getPhone()
                               << " | Address: " << matches[i]->getAddress() << "\r\n";
                        }
                        ss << "\r\nPlease search by Customer ID above to specify the exact record to delete.";
                        SetWindowTextA(s_delCustDetails, ss.str().c_str());
                        s_delSelectedCustID = 0;
                        return 0;
                    }
                    else
                    {
                        foundCust = matches[0];
                    }
                }

                s_delSelectedCustID = foundCust->getCustomerID();

                stringstream ss;
                ss << "========================================================\r\n";
                ss << "              CUSTOMER RECORD FOUND                     \r\n";
                ss << "========================================================\r\n\r\n";
                ss << "  Customer ID:       " << foundCust->getCustomerID() << "\r\n";
                ss << "  Customer Name:     " << foundCust->getName() << "\r\n";
                ss << "  Phone Number:      " << foundCust->getPhone() << "\r\n";
                ss << "  Address:           " << foundCust->getAddress() << "\r\n";
                ss << "========================================================\r\n\r\n";
                ss << "Click 'Delete Customer' below to confirm removal.";
                SetWindowTextA(s_delCustDetails, ss.str().c_str());

                return 0;
            }

            if (controlId == 802) // Delete Customer
            {
                if (s_delSelectedCustID <= 0 || g_customerManager == NULL)
                {
                    MessageBoxA(window, "Please find and select a customer record first.", "Warning", MB_OK | MB_ICONWARNING);
                    return 0;
                }

                Customer* cust = g_customerManager->searchCustomer(s_delSelectedCustID);
                string confirmMsg = "Are you sure you want to permanently delete this customer record?\n\n";
                confirmMsg += "Customer ID: " + to_string(s_delSelectedCustID) + "\n";
                if (cust != NULL)
                {
                    confirmMsg += "Name: " + cust->getName() + "\n";
                    confirmMsg += "Phone: " + cust->getPhone() + "\n";
                }

                int answer = MessageBoxA(window, confirmMsg.c_str(), "Confirm Customer Deletion", MB_YESNO | MB_ICONQUESTION);
                if (answer == IDYES)
                {
                    if (g_customerManager->deleteCustomer(s_delSelectedCustID))
                    {
                        if (g_dataManager != NULL)
                        {
                            g_dataManager->saveAll(*g_medicineManager, *g_customerManager, *g_billManager, *g_saleManager);
                        }
                        MessageBoxA(window, "Customer deleted successfully.", "PharmaCare", MB_OK | MB_ICONINFORMATION);
                        s_delSelectedCustID = 0;
                        DestroyWindow(window);
                        if (g_gui != NULL && g_gui->getMainWindow() != NULL)
                        {
                            InvalidateRect(g_gui->getMainWindow(), NULL, TRUE);
                        }
                    }
                    else
                    {
                        MessageBoxA(window, "Failed to delete customer.", "Error", MB_OK | MB_ICONERROR);
                    }
                }

                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY:
        {
            s_custDeleteWindow = NULL;
            s_delCustFindType = NULL;
            s_delCustFindValue = NULL;
            s_delCustDetails = NULL;
            s_delSelectedCustID = 0;
            return 0;
        }
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static void openCustomerDeleteWindow()
{
    if (s_custDeleteWindow != NULL)
    {
        SetForegroundWindow(s_custDeleteWindow);
        return;
    }

    HINSTANCE instance = GetModuleHandleA(NULL);
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = CustomerDeleteProcedure;
        wc.hInstance = instance;
        wc.lpszClassName = "PharmaCareCustDelete";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(GUI_BACKGROUND);
        RegisterClassA(&wc);
        registered = true;
    }

    s_custDeleteWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        "PharmaCareCustDelete",
        "Delete Customer - PharmaCare",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        340, 110, 710, 340,
        NULL, NULL, instance, NULL
    );

    if (s_custDeleteWindow != NULL)
    {
        ShowWindow(s_custDeleteWindow, SW_SHOW);
        UpdateWindow(s_custDeleteWindow);
    }
}

void GUI::addCustomerWindow()
{
    openCustomerAddWindow();
}

void GUI::updateCustomerWindow()
{
    openCustomerUpdateWindow();
}

void GUI::deleteCustomerWindow()
{
    openCustomerDeleteWindow();
}

void GUI::searchCustomerWindow()
{
    openCustomerSearchWindow();
}
// ============================================================
// PHARMACARE GUI - PART 3
// BILLING MODULE
// ============================================================

static HWND billingFormWindow = NULL;

static HWND billCustomerIdBox = NULL;
static HWND billMedicineIdBox = NULL;
static HWND billQuantityBox = NULL;

static HWND billCustomerNameBox = NULL;
static HWND billDateBox = NULL;

static HWND billItemsList = NULL;
static HWND billTotalLabel = NULL;

static Bill* activeBill = NULL;


// ------------------------------------------------------------
// Get text from billing control
// ------------------------------------------------------------

static string getBillingText(HWND control)
{
    char buffer[500];

    GetWindowTextA(
        control,
        buffer,
        sizeof(buffer)
    );

    return string(buffer);
}


// ------------------------------------------------------------
// Set text in billing control
// ------------------------------------------------------------

static void setBillingText(
    HWND control,
    string value
)
{
    if (control != NULL)
    {
        SetWindowTextA(
            control,
            value.c_str()
        );
    }
}


// ------------------------------------------------------------
// Create billing edit box
// ------------------------------------------------------------

static HWND createBillingEdit(
    HWND parent,
    int x,
    int y,
    int width,
    int height,
    string value
)
{
    HWND editBox =
        CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "EDIT",
            value.c_str(),
            WS_CHILD |
            WS_VISIBLE |
            WS_BORDER |
            ES_AUTOHSCROLL,
            x,
            y,
            width,
            height,
            parent,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

    SendMessageA(
        editBox,
        WM_SETFONT,
        (WPARAM)GetStockObject(DEFAULT_GUI_FONT),
        TRUE
    );

    return editBox;
}


// ------------------------------------------------------------
// Add text to bill items list
// ------------------------------------------------------------

static void addBillListText(
    string text
)
{
    if (billItemsList == NULL)
        return;

    SendMessageA(
        billItemsList,
        LB_ADDSTRING,
        0,
        (LPARAM)text.c_str()
    );
}


// ------------------------------------------------------------
// Refresh current bill display
// ------------------------------------------------------------

static void refreshBillDisplay()
{
    if (billItemsList == NULL ||
        activeBill == NULL)
    {
        return;
    }

    SendMessageA(
        billItemsList,
        LB_RESETCONTENT,
        0,
        0
    );


    BillItem* current =
        activeBill->getHead();


    while (current != NULL)
    {
        Medicine medicine =
            current->getMedicine();

        string row =
            "ID: " +
            to_string(
                medicine.getMedicineID()
            ) +
            "    " +
            medicine.getName() +
            "    Qty: " +
            to_string(
                current->getQuantity()
            ) +
            "    Price: " +
            to_string(
                medicine.getPrice()
            ) +
            "    Amount: " +
            to_string(
                current->getAmount()
            );


        addBillListText(
            row
        );


        current =
            current->next;
    }


    string total =
        "Grand Total: Rs. " +
        to_string(
            activeBill->calculateTotal()
        );


    setBillingText(
        billTotalLabel,
        total
    );
}


// ------------------------------------------------------------
// Billing form procedure
// ------------------------------------------------------------

static LRESULT CALLBACK BillingFormProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            HINSTANCE instance =
                GetModuleHandle(NULL);

            HFONT font =
                (HFONT)GetStockObject(DEFAULT_GUI_FONT);


            // ----------------------------------------
            // Title
            // ----------------------------------------

            HWND title =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "PharmaCare - New Bill",
                    WS_CHILD |
                    WS_VISIBLE |
                    SS_LEFT,
                    30,
                    20,
                    500,
                    30,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                title,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            // ----------------------------------------
            // Customer ID
            // ----------------------------------------

            HWND customerIDLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Customer ID",
                    WS_CHILD |
                    WS_VISIBLE,
                    30,
                    70,
                    120,
                    25,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                customerIDLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            billCustomerIdBox =
                createBillingEdit(
                    window,
                    150,
                    65,
                    180,
                    30,
                    ""
                );


            // ----------------------------------------
            // Customer Name
            // ----------------------------------------

            HWND customerNameLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Customer Name",
                    WS_CHILD |
                    WS_VISIBLE,
                    350,
                    70,
                    120,
                    25,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                customerNameLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            billCustomerNameBox =
                createBillingEdit(
                    window,
                    470,
                    65,
                    180,
                    30,
                    ""
                );


            // ----------------------------------------
            // Date
            // ----------------------------------------

            HWND dateLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Date",
                    WS_CHILD |
                    WS_VISIBLE,
                    30,
                    115,
                    120,
                    25,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                dateLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            string today =
                "Today";


            billDateBox =
                createBillingEdit(
                    window,
                    150,
                    110,
                    180,
                    30,
                    today
                );


            // ----------------------------------------
            // Medicine ID
            // ----------------------------------------

            HWND medicineIDLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Medicine ID",
                    WS_CHILD |
                    WS_VISIBLE,
                    30,
                    170,
                    120,
                    25,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                medicineIDLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            billMedicineIdBox =
                createBillingEdit(
                    window,
                    150,
                    165,
                    180,
                    30,
                    ""
                );


            // ----------------------------------------
            // Quantity
            // ----------------------------------------

            HWND quantityLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Quantity",
                    WS_CHILD |
                    WS_VISIBLE,
                    350,
                    170,
                    120,
                    25,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                quantityLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            billQuantityBox =
                createBillingEdit(
                    window,
                    470,
                    165,
                    180,
                    30,
                    ""
                );


            // ----------------------------------------
            // Add Item button
            // ----------------------------------------

            HWND addItemButton =
                CreateWindowExA(
                    0,
                    "BUTTON",
                    "Add Medicine",
                    WS_CHILD |
                    WS_VISIBLE |
                    BS_PUSHBUTTON,
                    670,
                    160,
                    130,
                    40,
                    window,
                    (HMENU)2001,
                    instance,
                    NULL
                );

            SendMessageA(
                addItemButton,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            // ----------------------------------------
            // Items label
            // ----------------------------------------

            HWND itemsLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Bill Items",
                    WS_CHILD |
                    WS_VISIBLE,
                    30,
                    225,
                    150,
                    25,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                itemsLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            // ----------------------------------------
            // Bill items list
            // ----------------------------------------

            billItemsList =
                CreateWindowExA(
                    WS_EX_CLIENTEDGE,
                    "LISTBOX",
                    "",
                    WS_CHILD |
                    WS_VISIBLE |
                    WS_BORDER |
                    LBS_NOTIFY |
                    WS_VSCROLL,
                    30,
                    255,
                    770,
                    190,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                billItemsList,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            // ----------------------------------------
            // Grand total
            // ----------------------------------------

            billTotalLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Grand Total: Rs. 0.00",
                    WS_CHILD |
                    WS_VISIBLE |
                    SS_LEFT,
                    30,
                    465,
                    350,
                    30,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                billTotalLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            // ----------------------------------------
            // Generate Bill
            // ----------------------------------------

            HWND generateButton =
                CreateWindowExA(
                    0,
                    "BUTTON",
                    "Generate Bill",
                    WS_CHILD |
                    WS_VISIBLE |
                    BS_PUSHBUTTON,
                    450,
                    460,
                    160,
                    40,
                    window,
                    (HMENU)2002,
                    instance,
                    NULL
                );

            SendMessageA(
                generateButton,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            // ----------------------------------------
            // Close
            // ----------------------------------------

            HWND closeButton =
                CreateWindowExA(
                    0,
                    "BUTTON",
                    "Close",
                    WS_CHILD |
                    WS_VISIBLE |
                    BS_PUSHBUTTON,
                    630,
                    460,
                    170,
                    40,
                    window,
                    (HMENU)2003,
                    instance,
                    NULL
                );

            SendMessageA(
                closeButton,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            refreshBillDisplay();

            return 0;
        }


        case WM_COMMAND:
        {
            int controlID =
                LOWORD(wParam);


            // ----------------------------------------
            // Add Medicine
            // ----------------------------------------

            if (controlID == 2001)
            {
                if (activeBill == NULL)
                {
                    MessageBoxA(
                        window,
                        "Please create a bill first.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                string medicineIDText =
                    getBillingText(
                        billMedicineIdBox
                    );

                string quantityText =
                    getBillingText(
                        billQuantityBox
                    );


                if (medicineIDText.empty() ||
                    quantityText.empty())
                {
                    MessageBoxA(
                        window,
                        "Please enter Medicine ID and Quantity.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                int medicineID =
                    atoi(
                        medicineIDText.c_str()
                    );

                int quantity =
                    atoi(
                        quantityText.c_str()
                    );


                if (medicineID <= 0 ||
                    quantity <= 0)
                {
                    MessageBoxA(
                        window,
                        "Medicine ID and Quantity must be greater than zero.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                if (g_medicineManager == NULL)
                {
                    MessageBoxA(
                        window,
                        "Medicine manager is unavailable.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONERROR
                    );

                    return 0;
                }


                Medicine* medicine =
                    g_medicineManager->searchMedicine(
                        medicineID
                    );


                if (medicine == NULL)
                {
                    MessageBoxA(
                        window,
                        "Medicine not found.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                if (medicine->getQuantity() < quantity)
                {
                    MessageBoxA(
                        window,
                        "Insufficient stock available.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                if (activeBill->addItem(
                        *medicine,
                        quantity))
                {
                    refreshBillDisplay();

                    setBillingText(
                        billMedicineIdBox,
                        ""
                    );

                    setBillingText(
                        billQuantityBox,
                        ""
                    );
                }
                else
                {
                    MessageBoxA(
                        window,
                        "Unable to add medicine to bill.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONERROR
                    );
                }


                return 0;
            }


            // ----------------------------------------
            // Generate Bill
            // ----------------------------------------

            if (controlID == 2002)
            {
                if (activeBill == NULL)
                {
                    MessageBoxA(
                        window,
                        "No bill has been created.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                if (activeBill->getHead() == NULL)
                {
                    MessageBoxA(
                        window,
                        "Please add at least one medicine.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONWARNING
                    );

                    return 0;
                }


                // Reduce stock for every item.
                BillItem* item =
                    activeBill->getHead();
                bool stockUpdated = true;    
                while (item != NULL)
                {
                    Medicine medicine =
                        item->getMedicine();


                    if (g_medicineManager == NULL ||
                        !g_medicineManager->reduceStock(
                            medicine.getMedicineID(),
                            item->getQuantity()))
                    {
                        stockUpdated =
                            false;

                        break;
                    }


                    item =
                        item->next;
                }


                if (!stockUpdated)
                {
                    MessageBoxA(
                        window,
                        "Unable to update medicine stock.",
                        "PharmaCare",
                        MB_OK |
                        MB_ICONERROR
                    );

                    return 0;
                }


                if (g_billManager != NULL)
{
    // Add sale record
    if (g_saleManager != NULL)
    {
        g_saleManager->addSale(
            activeBill->getBillNumber(),
            activeBill->getDate(),
            activeBill->getAmount()
        );
    }

    // Save all data
    if (g_dataManager != NULL)
    {
        g_dataManager->saveAll(
            *g_medicineManager,
            *g_customerManager,
            *g_billManager,
            *g_saleManager
        );
    }

    MessageBoxA(
        window,
        "Bill generated successfully and saved.",
        "PharmaCare",
        MB_OK |
        MB_ICONINFORMATION
    );

    DestroyWindow(window);
}


                return 0;
            }


            // ----------------------------------------
            // Close
            // ----------------------------------------

            if (controlID == 2003)
            {
                DestroyWindow(
                    window
                );

                return 0;
            }


            break;
        }


        case WM_CLOSE:
        {
            DestroyWindow(
                window
            );

            return 0;
        }


        case WM_DESTROY:
        {
            billingFormWindow = NULL;

            billCustomerIdBox = NULL;
            billMedicineIdBox = NULL;
            billQuantityBox = NULL;
            billCustomerNameBox = NULL;
            billDateBox = NULL;
            billItemsList = NULL;
            billTotalLabel = NULL;

            activeBill = NULL;

            return 0;
        }
    }


    return DefWindowProcA(
        window,
        message,
        wParam,
        lParam
    );
}


// ------------------------------------------------------------
// Open New Bill Form
// ------------------------------------------------------------

void GUI::createNewBill()
{
    if (billManager == NULL)
    {
        MessageBoxA(
            NULL,
            "Bill manager is unavailable.",
            "PharmaCare",
            MB_OK |
            MB_ICONERROR
        );

        return;
    }


    HINSTANCE instance =
        GetModuleHandle(NULL);


    WNDCLASSA formClass;

    ZeroMemory(
        &formClass,
        sizeof(formClass)
    );


    formClass.lpfnWndProc =
        BillingFormProcedure;

    formClass.hInstance =
        instance;

    formClass.lpszClassName =
        "PharmaCareBillingForm";

    formClass.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    formClass.hbrBackground =
        (HBRUSH)(COLOR_WINDOW + 1);


    RegisterClassA(
        &formClass
    );


    string customerIDText;


    // Ask customer ID through the complete form.
    // The actual bill object is created after the form
    // receives the customer information.


    int customerID =
        0;


    char inputBuffer[100];

    ZeroMemory(
        inputBuffer,
        sizeof(inputBuffer)
    );


    // Create a temporary complete billing form.
    // Customer information is entered directly in the form.


    string date =
        getCurrentDate();


    // A temporary customer ID of zero is replaced when
    // the user starts the actual billing workflow.


    activeBill =
        billManager->createBill(
            0,
            "",
            date
        );

    currentBill = activeBill;
    if (activeBill == NULL)
    {
        MessageBoxA(
            NULL,
            "Unable to create a new bill.",
            "PharmaCare",
            MB_OK |
            MB_ICONERROR
        );

        return;
    }


    billingFormWindow =
        CreateWindowExA(
            WS_EX_DLGMODALFRAME,
            "PharmaCareBillingForm",
            "PharmaCare - New Bill",
            WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU,
            250,
            80,
            850,
            570,
            NULL,
            NULL,
            instance,
            NULL
        );


    if (billingFormWindow == NULL)
    {
        delete activeBill;
        activeBill = NULL;

        MessageBoxA(
            NULL,
            "Unable to open billing window.",
            "PharmaCare",
            MB_OK |
            MB_ICONERROR
        );

        return;
    }


    ShowWindow(
        billingFormWindow,
        SW_SHOW
    );

    UpdateWindow(
        billingFormWindow
    );


    MSG message;


    while (
        billingFormWindow != NULL &&
        GetMessageA(
            &message,
            NULL,
            0,
            0
        ) > 0
    )
    {
        TranslateMessage(
            &message
        );

        DispatchMessageA(
            &message
        );
    }
    InvalidateRect(
    NULL,
    NULL,
    TRUE
);
}


// ------------------------------------------------------------
// Add medicine to current bill
// ------------------------------------------------------------

void GUI::addMedicineToCurrentBill()
{
    if (currentBill == NULL)
    {
        MessageBoxA(
            NULL,
            "No active bill.",
            "PharmaCare",
            MB_OK |
            MB_ICONWARNING
        );

        return;
    }
}

// ============================================================
// END OF PART 3
// ============================================================
// ============================================================
// PHARMACARE GUI - PART 4A
// SALES HISTORY + DAILY SALES
// ============================================================

static HWND salesWindow = NULL;
static HWND salesList = NULL;
static HWND salesTotalLabel = NULL;


// ------------------------------------------------------------
// Add text to sales list
// ------------------------------------------------------------

static void addSalesListText(string text)
{
    if (salesList == NULL)
        return;

    SendMessageA(
        salesList,
        LB_ADDSTRING,
        0,
        (LPARAM)text.c_str()
    );
}


// ------------------------------------------------------------
// Sales Window Procedure
// ------------------------------------------------------------

static LRESULT CALLBACK SalesWindowProcedure(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            HINSTANCE instance =
                GetModuleHandle(NULL);

            HFONT font =
                (HFONT)GetStockObject(DEFAULT_GUI_FONT);


            HWND title =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "PharmaCare - Sales History",
                    WS_CHILD |
                    WS_VISIBLE,
                    30,
                    20,
                    500,
                    30,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                title,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            salesList =
                CreateWindowExA(
                    WS_EX_CLIENTEDGE,
                    "LISTBOX",
                    "",
                    WS_CHILD |
                    WS_VISIBLE |
                    WS_BORDER |
                    WS_VSCROLL |
                    LBS_NOINTEGRALHEIGHT,
                    30,
                    70,
                    760,
                    360,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                salesList,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            salesTotalLabel =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Total Income: Rs. 0.00",
                    WS_CHILD |
                    WS_VISIBLE,
                    30,
                    450,
                    350,
                    30,
                    window,
                    NULL,
                    instance,
                    NULL
                );

            SendMessageA(
                salesTotalLabel,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            HWND closeButton =
                CreateWindowExA(
                    0,
                    "BUTTON",
                    "Close",
                    WS_CHILD |
                    WS_VISIBLE |
                    BS_PUSHBUTTON,
                    650,
                    445,
                    140,
                    40,
                    window,
                    (HMENU)4001,
                    instance,
                    NULL
                );

            SendMessageA(
                closeButton,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );


            return 0;
        }


        case WM_COMMAND:
        {
            if (LOWORD(wParam) == 4001)
            {
                DestroyWindow(window);

                return 0;
            }

            break;
        }


        case WM_CLOSE:
        {
            DestroyWindow(window);

            return 0;
        }


        case WM_DESTROY:
        {
            salesWindow = NULL;
            salesList = NULL;
            salesTotalLabel = NULL;

            return 0;
        }
    }


    return DefWindowProcA(
        window,
        message,
        wParam,
        lParam
    );
}


// ------------------------------------------------------------
// Show Sales History
// ------------------------------------------------------------

void GUI::showSalesHistory()
{
    if (saleManager == NULL)
    {
        MessageBoxA(
            NULL,
            "Sales manager is unavailable.",
            "PharmaCare",
            MB_OK |
            MB_ICONERROR
        );

        return;
    }


    HINSTANCE instance =
        GetModuleHandle(NULL);


    WNDCLASSA salesClass;

    ZeroMemory(
        &salesClass,
        sizeof(salesClass)
    );


    salesClass.lpfnWndProc =
        SalesWindowProcedure;

    salesClass.hInstance =
        instance;

    salesClass.lpszClassName =
        "PharmaCareSalesWindow";

    salesClass.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    salesClass.hbrBackground =
        (HBRUSH)(COLOR_WINDOW + 1);


    RegisterClassA(
        &salesClass
    );


    salesWindow =
        CreateWindowExA(
            WS_EX_DLGMODALFRAME,
            "PharmaCareSalesWindow",
            "PharmaCare - Sales History",
            WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU,
            300,
            100,
            850,
            550,
            NULL,
            NULL,
            instance,
            NULL
        );


    if (salesWindow == NULL)
    {
        MessageBoxA(
            NULL,
            "Unable to open Sales History.",
            "PharmaCare",
            MB_OK |
            MB_ICONERROR
        );

        return;
    }


    ShowWindow(
        salesWindow,
        SW_SHOW
    );

    UpdateWindow(
        salesWindow
    );


    // --------------------------------------------------------
    // Display sales
    // --------------------------------------------------------

    int salesCount =
        saleManager->getCount();


    double totalIncome =
        0.0;


    for (int i = 0; i < salesCount; i++)
    {
        Sale* sale =
            saleManager->getSaleAt(i);


        if (sale == NULL)
            continue;


        string row =
            "Sale ID: " +
            to_string(
                sale->getSaleID()
            ) +
            "    Bill No: " +
            to_string(
                sale->getBillNumber()
            ) +
            "    Date: " +
            sale->getDate() +
            "    Amount: Rs. " +
            to_string(
                sale->getAmount()
            );


        addSalesListText(
            row
        );


        totalIncome +=
            sale->getAmount();
    }


    string totalText =
        "Total Income: Rs. " +
        to_string(
            totalIncome
        );


    SetWindowTextA(
        salesTotalLabel,
        totalText.c_str()
    );


    MSG message;


    while (
        salesWindow != NULL &&
        GetMessageA(
            &message,
            NULL,
            0,
            0
        ) > 0
    )
    {
        TranslateMessage(
            &message
        );

        DispatchMessageA(
            &message
        );
    }


    InvalidateRect(
        NULL,
        NULL,
        TRUE
    );
}


// ------------------------------------------------------------
// Show Daily Sales
// ------------------------------------------------------------

void GUI::showDailySales()
{
    if (saleManager == NULL)
    {
        MessageBoxA(
            NULL,
            "Sales manager is unavailable.",
            "PharmaCare",
            MB_OK |
            MB_ICONERROR
        );

        return;
    }


    string today =
        getCurrentDate();


    int count =
        saleManager->countDailySales(
            today
        );


    double income =
        saleManager->calculateDailyIncome(
            today
        );


    string message =
        "Daily Sales\n\n"
        "Date: " +
        today +
        "\n"
        "Number of Sales: " +
        to_string(count) +
        "\n"
        "Daily Income: Rs. " +
        to_string(income);


    MessageBoxA(
        NULL,
        message.c_str(),
        "PharmaCare - Daily Sales",
        MB_OK |
        MB_ICONINFORMATION
    );
}


// ============================================================
// END OF PART 4A
// ============================================================