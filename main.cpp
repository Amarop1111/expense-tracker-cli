// =====================================================================
//  Expense Tracker - a menu-driven command-line utility
//
//  Features:
//    1. Add an expense (description, category, amount)
//    2. List all expenses
//    3. Show total spent in a given category
//    4. Show overall total spent
//    5. Exit
//
//  Design notes:
//    - All expenses are kept in a std::vector<Expense> for the lifetime
//      of the program (in-memory, per the task's "collections" requirement).
//    - Every piece of user input goes through a validation helper so the
//      program can never crash or get stuck on bad input (letters typed
//      where a number is expected, empty strings, negative amounts, an
//      out-of-range menu choice, etc.).
//    - The code is split into small, single-purpose functions so the
//      control flow is easy to follow and easy to test.
// =====================================================================

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// ---------------------------------------------------------------------
// Data model
// ---------------------------------------------------------------------
struct Expense {
    std::string description;
    std::string category;
    double amount;
};

// ---------------------------------------------------------------------
// Input helpers (defensive input handling)
// ---------------------------------------------------------------------

// Discards the rest of the current input line. Used after every read
// to clean up leftover characters (e.g. the newline after >> extraction,
// or garbage left behind by a failed extraction).
void clearInputLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Reads a non-empty line of text from the user, re-prompting until one
// is given. Works even if the previous input left a partially-read line
// in the buffer.
std::string readNonEmptyLine(const std::string& prompt) {
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        // Trim leading/trailing whitespace.
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");

        if (start == std::string::npos) {
            std::cout << "  -> This field can't be empty. Please try again.\n";
            continue;
        }
        return line.substr(start, end - start + 1);
    }
}

// Reads a positive (non-negative) monetary amount, re-prompting on any
// invalid input: non-numeric text, negative numbers, or extra trailing
// characters after the number.
double readPositiveAmount(const std::string& prompt) {
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        try {
            size_t consumed = 0;
            double value = std::stod(line, &consumed);

            // Make sure nothing but whitespace follows the number
            // (rejects things like "12abc").
            bool trailingOk = true;
            for (size_t i = consumed; i < line.size(); ++i) {
                if (!std::isspace(static_cast<unsigned char>(line[i]))) {
                    trailingOk = false;
                    break;
                }
            }

            if (!trailingOk || value < 0.0) {
                std::cout << "  -> Please enter a valid, non-negative amount (e.g. 149.99).\n";
                continue;
            }
            return value;
        } catch (const std::exception&) {
            std::cout << "  -> That doesn't look like a number. Please try again.\n";
        }
    }
}

// Reads an integer menu choice within [minChoice, maxChoice], re-prompting
// on non-numeric input or out-of-range values.
int readMenuChoice(int minChoice, int maxChoice) {
    while (true) {
        std::cout << "Enter your choice (" << minChoice << "-" << maxChoice << "): ";
        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "  -> Invalid input. Please enter a number.\n";
            clearInputLine();
            continue;
        }
        clearInputLine(); // discard trailing newline / extra tokens

        if (choice < minChoice || choice > maxChoice) {
            std::cout << "  -> Please enter a number between "
                      << minChoice << " and " << maxChoice << ".\n";
            continue;
        }
        return choice;
    }
}

// ---------------------------------------------------------------------
// Core actions
// ---------------------------------------------------------------------

void addExpense(std::vector<Expense>& expenses) {
    std::cout << "\n--- Add Expense ---\n";
    std::string description = readNonEmptyLine("Description: ");
    std::string category    = readNonEmptyLine("Category: ");
    double amount            = readPositiveAmount("Amount: ");

    expenses.push_back(Expense{description, category, amount});
    std::cout << "Added \"" << description << "\" (" << category
              << ") - " << std::fixed << std::setprecision(2)
              << amount << "\n";
}

void listExpenses(const std::vector<Expense>& expenses) {
    std::cout << "\n--- All Expenses ---\n";
    if (expenses.empty()) {
        std::cout << "No expenses recorded yet.\n";
        return;
    }

    std::cout << std::left
              << std::setw(4)  << "#"
              << std::setw(28) << "Description"
              << std::setw(18) << "Category"
              << std::right << std::setw(12) << "Amount" << "\n";
    std::cout << std::string(62, '-') << "\n";

    int index = 1;
    for (const auto& e : expenses) {
        std::cout << std::left
                  << std::setw(4)  << index++
                  << std::setw(28) << e.description
                  << std::setw(18) << e.category
                  << std::right << std::setw(12)
                  << std::fixed << std::setprecision(2) << e.amount
                  << "\n";
    }
}

void showCategoryTotal(const std::vector<Expense>& expenses) {
    std::cout << "\n--- Category Total ---\n";
    if (expenses.empty()) {
        std::cout << "No expenses recorded yet.\n";
        return;
    }

    std::string category = readNonEmptyLine("Category to total: ");

    double total = 0.0;
    int count = 0;
    for (const auto& e : expenses) {
        // Case-insensitive match so "food" and "Food" are treated the same.
        if (e.category.size() == category.size() &&
            std::equal(e.category.begin(), e.category.end(), category.begin(),
                       [](char a, char b) {
                           return std::tolower(static_cast<unsigned char>(a)) ==
                                  std::tolower(static_cast<unsigned char>(b));
                       })) {
            total += e.amount;
            ++count;
        }
    }

    if (count == 0) {
        std::cout << "No expenses found in category \"" << category << "\".\n";
    } else {
        std::cout << count << " expense(s) in \"" << category << "\" - Total: "
                  << std::fixed << std::setprecision(2) << total << "\n";
    }
}

void showOverallTotal(const std::vector<Expense>& expenses) {
    std::cout << "\n--- Overall Total ---\n";
    double total = 0.0;
    for (const auto& e : expenses) {
        total += e.amount;
    }
    std::cout << "Total across " << expenses.size() << " expense(s): "
              << std::fixed << std::setprecision(2) << total << "\n";
}

// ---------------------------------------------------------------------
// Menu / program flow
// ---------------------------------------------------------------------

void printMenu() {
    std::cout << "\n===== Expense Tracker =====\n"
              << "1. Add expense\n"
              << "2. List all expenses\n"
              << "3. Show total by category\n"
              << "4. Show overall total\n"
              << "5. Exit\n";
}

int main() {
    std::vector<Expense> expenses;

    std::cout << "Welcome to the Expense Tracker!\n";

    bool running = true;
    while (running) {
        printMenu();
        int choice = readMenuChoice(1, 5);

        switch (choice) {
            case 1:
                addExpense(expenses);
                break;
            case 2:
                listExpenses(expenses);
                break;
            case 3:
                showCategoryTotal(expenses);
                break;
            case 4:
                showOverallTotal(expenses);
                break;
            case 5:
                std::cout << "\nGoodbye! Your session's expenses were not saved to disk.\n";
                running = false;
                break;
            default:
                // Unreachable because readMenuChoice already validates the
                // range, but kept for defensive completeness.
                std::cout << "Unexpected choice.\n";
                break;
        }
    }

    return 0;
}
