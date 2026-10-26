#include <iostream>
#include <string>
#include <cassert>
#include <cctype>

using namespace std;

// Extracted validation functions matching GUI.cpp
static string trimString(const string& s) {
    size_t start = 0;
    while (start < s.length() && (unsigned char)s[start] <= ' ') {
        start++;
    }
    if (start == s.length()) return "";
    size_t end = s.length() - 1;
    while (end > start && (unsigned char)s[end] <= ' ') {
        end--;
    }
    return s.substr(start, end - start + 1);
}

static bool isValidCalendarDate(const string& rawDate) {
    string d = trimString(rawDate);
    if (d.length() != 10) return false;
    if (d[2] != '-' || d[5] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)d[i])) return false;
    }
    int day = (d[0] - '0') * 10 + (d[1] - '0');
    int mon = (d[3] - '0') * 10 + (d[4] - '0');
    int yr  = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');

    if (yr < 1900 || yr > 2100) return false;
    if (mon < 1 || mon > 12) return false;
    if (day < 1) return false;

    int daysInMonth = 31;
    if (mon == 4 || mon == 6 || mon == 9 || mon == 11) {
        daysInMonth = 30;
    } else if (mon == 2) {
        bool isLeap = ((yr % 4 == 0 && yr % 100 != 0) || (yr % 400 == 0));
        daysInMonth = isLeap ? 29 : 28;
    }
    return (day <= daysInMonth);
}

static bool isValidID(const string& rawId, int& outId) {
    string s = trimString(rawId);
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit((unsigned char)c)) return false;
    }
    try {
        long long val = stoll(s);
        if (val <= 0 || val > 2147483647) return false;
        outId = (int)val;
        return true;
    } catch (...) {
        return false;
    }
}

static bool isValidPrice(const string& rawPrice, double& outPrice) {
    string s = trimString(rawPrice);
    if (s.empty()) return false;
    int dotCount = 0;
    for (char c : s) {
        if (c == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        } else if (!isdigit((unsigned char)c)) {
            return false;
        }
    }
    try {
        double val = stod(s);
        if (val <= 0.0) return false;
        outPrice = val;
        return true;
    } catch (...) {
        return false;
    }
}

static bool isValidQuantity(const string& rawQty, int& outQty) {
    string s = trimString(rawQty);
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit((unsigned char)c)) return false;
    }
    try {
        long long val = stoll(s);
        if (val < 0 || val > 1000000) return false;
        outQty = (int)val;
        return true;
    } catch (...) {
        return false;
    }
}

static bool isValidPhone(const string& rawPhone) {
    string s = trimString(rawPhone);
    if (s.length() != 10) return false;
    for (char c : s) {
        if (!isdigit((unsigned char)c)) return false;
    }
    return true;
}

int main() {
    cout << "Running validation tests..." << endl;

    // Date validation
    assert(isValidCalendarDate("25-12-2026") == true);
    assert(isValidCalendarDate("01-01-2025") == true);
    assert(isValidCalendarDate("29-02-2024") == true); // leap year 2024
    assert(isValidCalendarDate("29-02-2026") == false); // not leap year
    assert(isValidCalendarDate("31-02-2026") == false); // impossible date
    assert(isValidCalendarDate("31-04-2026") == false); // april has 30 days
    assert(isValidCalendarDate("30-04-2026") == true);
    assert(isValidCalendarDate("abc") == false);
    assert(isValidCalendarDate("12/34/2026") == false);
    assert(isValidCalendarDate("2026-10-17") == false);
    assert(isValidCalendarDate(" 25-12-2026 ") == true); // trims

    // Phone validation
    assert(isValidPhone("9876543210") == true);
    assert(isValidPhone("0123456789") == true); // preserves leading zero
    assert(isValidPhone("987654321") == false); // 9 digits
    assert(isValidPhone("98765432100") == false); // 11 digits
    assert(isValidPhone("987654321a") == false); // contains letter
    assert(isValidPhone("987 654321") == false); // contains space
    assert(isValidPhone("+9198765432") == false); // symbol

    // Price validation
    double price = 0;
    assert(isValidPrice("12.50", price) == true && price == 12.50);
    assert(isValidPrice("100", price) == true && price == 100.0);
    assert(isValidPrice("0", price) == false); // must be > 0
    assert(isValidPrice("-5.0", price) == false);
    assert(isValidPrice("12.50.2", price) == false);
    assert(isValidPrice("abc", price) == false);
    assert(isValidPrice("12.5a", price) == false);

    // Quantity validation
    int qty = -1;
    assert(isValidQuantity("50", qty) == true && qty == 50);
    assert(isValidQuantity("0", qty) == true && qty == 0); // 0 is valid stock
    assert(isValidQuantity("-10", qty) == false);
    assert(isValidQuantity("10.5", qty) == false);
    assert(isValidQuantity("abc", qty) == false);

    // ID validation
    int id = 0;
    assert(isValidID("101", id) == true && id == 101);
    assert(isValidID("0", id) == false); // ID must be > 0
    assert(isValidID("-1", id) == false);
    assert(isValidID("101a", id) == false);

    cout << "All validation tests passed successfully!" << endl;
    return 0;
}
