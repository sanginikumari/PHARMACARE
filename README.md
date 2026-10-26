# PharmaCare – Pharmacy Management System

PharmaCare is a desktop-based pharmacy management application developed in **C++** using the **Windows API (Win32)**. It provides a graphical interface for managing medicine inventory, customer records, billing, sales transactions, and basic sales reports. Application records are stored locally in text files so that data can be loaded again when the program is restarted.

## Features

- **Dashboard:** Main application screen with navigation to the available modules.
- **Medicine management:** Add, view, update, delete, and search medicine records.
- **Inventory tracking:** Maintain medicine prices and quantities, reduce stock when items are billed, and increase stock when required.
- **Stock and expiry alerts:** View low-stock and expired/near-expiry medicine alerts.
- **Customer management:** Add, view, update, delete, and search customer records.
- **Billing:** Create bills for customers, add medicines and quantities, and calculate bill totals.
- **Sales tracking:** Store sales transactions and view sales history.
- **Reports:** Review daily sales and sales totals.
- **Local data persistence:** Save and load medicine, customer, bill, bill-item, and sales information using text files.

## Technology Stack

- **Language:** C++
- **GUI:** Windows API (Win32)
- **Data structures:** Fixed-size arrays and a linked list for bill items
- **Storage:** Plain-text files
- **Build tools:** A C++ compiler such as MinGW-w64 g++ or MSVC

## Project Structure

```text
pharmacare/
├── data/                  # Runtime data files
│   ├── bills.txt
│   ├── bill_items.txt
│   ├── customers.txt
│   ├── medicines.txt
│   └── sales.txt
├── include/               # Header files and class declarations
│   ├── bill.h
│   ├── billitem.h
│   ├── billmanager.h
│   ├── customer.h
│   ├── customermanager.h
│   ├── datamanager.h
│   ├── GUI.h
│   ├── medicine.h
│   ├── medicinemanager.h
│   ├── sale.h
│   └── salemanager.h
├── src/                   # Class implementations
│   ├── bill.cpp
│   ├── billitem.cpp
│   ├── billmanager.cpp
│   ├── customer.cpp
│   ├── customermanager.cpp
│   ├── datamanager.cpp
│   ├── GUI.cpp
│   ├── medicine.cpp
│   ├── medicinemanager.cpp
│   ├── sale.cpp
│   └── salemanager.cpp
├── main.cpp               # Application entry point
└── README.md
```

The `.vscode/` directory contains editor/build configuration files. `scratch/` contains a validation test source file and is not required to run the main application. Existing `.exe` files, if present, are prebuilt binaries; rebuild from source when making code changes.

## Requirements

- Windows 10 or Windows 11 (the interface uses `windows.h` and Win32 APIs).
- A C++ compiler:
  - MinGW-w64 (with `g++`), or
  - Microsoft Visual C++ (MSVC).
- A terminal or IDE such as Visual Studio Code.

## Build and Run (MinGW-w64)

1. Install MinGW-w64 and make sure `g++` is available in your system `PATH`.
2. Open a terminal in the `pharmacare` project directory (the directory containing `main.cpp`).
3. Compile the source files:

   ```bash
   g++ -std=c++11 -Wall -Wextra main.cpp src/*.cpp -o PharmaCare.exe -lgdi32 -luser32
   ```

4. Run the application from the same project directory:

   ```bash
   .\PharmaCare.exe
   ```

Running it from the project directory is important because the application uses relative paths to read and write files in `data/`.

### Build with Visual Studio (MSVC)

Create a Windows Console or Desktop C++ project, add `main.cpp`, all `.cpp` files in `src/`, and the headers in `include/`. Build as a Windows application with the Windows SDK available. The Win32 libraries used by the application should be linked by the project; if needed, add `User32.lib` and `Gdi32.lib`.

## Data Storage

The application stores its records in the `data/` directory:

| File | Purpose |
|---|---|
| `medicines.txt` | Medicine details and inventory quantities |
| `customers.txt` | Customer records |
| `bills.txt` | Bill-level information |
| `bill_items.txt` | Individual medicine items belonging to bills |
| `sales.txt` | Completed sales records |

The program loads saved records during startup and uses the data manager to save application data. Keep the `data/` directory alongside the executable and run the program with the project directory as its working directory. Back up this directory before manually editing or replacing stored records.

## Main Modules

- **Medicine / MedicineManager:** Represents medicine details and manages inventory operations.
- **Customer / CustomerManager:** Represents and manages customer information.
- **Bill / BillItem / BillManager:** Builds bills and maintains their line items.
- **Sale / SaleManager:** Records transactions and calculates sales totals.
- **DataManager:** Handles loading and saving application data.
- **GUI:** Creates the Win32 graphical interface and connects user actions to the managers.

## Notes and Limitations

- This project is designed for Windows because its graphical interface depends on the Win32 API.
- The managers use fixed-size arrays (for example, up to 100 medicines/customers/bills and 500 sales); these are implementation limits, not unlimited storage.
- Data is stored locally in text files. The project does not include a database server or cloud synchronization.
- This is an academic/software project and should not be treated as a validated production pharmacy or healthcare system.
- Keep backups of the `data/` folder to avoid losing locally stored records.

## Troubleshooting

- **`windows.h` not found:** Use a Windows C++ toolchain with the Windows SDK or MinGW-w64.
- **Undefined references to Win32 functions:** Ensure `-lgdi32 -luser32` are included in the MinGW link command.
- **Data appears empty after launching:** Run the executable from the `pharmacare` directory so the relative `data/` paths resolve correctly.
- **Changes do not appear in the executable:** Recompile the source files and run the newly built executable.

## Author / Project Information

**Project:** PharmaCare – Pharmacy Management System  
**Implementation:** C++ with Win32 GUI  
**Purpose:** Academic project demonstrating object-oriented programming, data structures, file handling, inventory management, and billing workflows.

---

*Update this README if the project’s features, compiler settings, or data-file formats change.*
