# Expense Tracker (C++ CLI)

A menu-driven command-line expense tracker built for the *Reliable Command-Line Utility* task.

## What it does

- **Add expense** — prompts for description, category, and amount
- **List all expenses** — prints a formatted table of everything entered
- **Show total by category** — totals all expenses in a given category (case-insensitive)
- **Show overall total** — sums every expense recorded in the session
- **Exit**

## Design / how the requirements are met

| Requirement          | How it's implemented |
|-----------------------|-----------------------|
| Variables              | `Expense` struct (`description`, `category`, `amount`) |
| Control flow            | `while` menu loop + `switch` on the chosen action |
| Functions               | One function per action (`addExpense`, `listExpenses`, `showCategoryTotal`, `showOverallTotal`), plus small input-validation helpers |
| Collections              | `std::vector<Expense>` holds all entries for the session |
| Defensive input handling | Every read goes through a validator: menu choices are range-checked, amounts are parsed and rejected if non-numeric/negative, text fields reject empty input — the program re-prompts instead of crashing |

## Files

- `main.cpp` — the full source code (single file, ~250 lines, heavily commented)
- `sample_run.txt` — a captured sample execution (see below)

## Build instructions

Requires a C++17-capable compiler (g++, clang++, or MSVC).

```bash
g++ -std=c++17 -Wall -Wextra -o expense_tracker main.cpp
./expense_tracker
```

On Windows with MinGW:

```bash
g++ -std=c++17 -Wall -Wextra -o expense_tracker.exe main.cpp
expense_tracker.exe
```

No external dependencies — only the C++ standard library.

## Sample execution

```
Welcome to the Expense Tracker!

===== Expense Tracker =====
1. Add expense
2. List all expenses
3. Show total by category
4. Show overall total
5. Exit
Enter your choice (1-5): 1

--- Add Expense ---
Description: Coffee
Category: Food
Amount: 4.50
Added "Coffee" (Food) - 4.50

===== Expense Tracker =====
...
Enter your choice (1-5): 2

--- All Expenses ---
#   Description                 Category                Amount
--------------------------------------------------------------
1   Coffee                      Food                      4.50
2   Bus ticket                  Transport                 2.25
3   Groceries                   Food                     32.10

Enter your choice (1-5): 3

--- Category Total ---
Category to total: food
2 expense(s) in "food" - Total: 36.60

Enter your choice (1-5): 4

--- Overall Total ---
Total across 3 expense(s): 38.85

Enter your choice (1-5): 5

Goodbye! Your session's expenses were not saved to disk.
```

The full captured run (including the exact terminal output) is in `sample_run.txt`.

### Invalid-input handling (tested, does not crash)

- Menu choice `99` or `abc` → *"Please enter a number between 1 and 5."* / *"Invalid input. Please enter a number."*, then re-prompts
- Amount `abc` or a negative number → *"Please enter a valid, non-negative amount."*, then re-prompts
- Empty description/category → *"This field can't be empty."*, then re-prompts

## Notes / possible portfolio upgrade

- Data is in-memory only for the session (not persisted to a file). A natural extension would be to save/load expenses from a CSV file on start/exit.
