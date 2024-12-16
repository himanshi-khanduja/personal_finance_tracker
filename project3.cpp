#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

class Transaction {
    double amount;
    string category, date, description;
    bool isCredit;

public:
    Transaction(double amt, const string &cat, const string &dt, const string &desc, bool credit)
        : amount(amt), category(cat), date(dt), description(desc), isCredit(credit) {}

    double getAmount() const { return amount; }
    string getCategory() const { return category; }
    string getDate() const { return date; }
    string getDescription() const { return description; }
    bool getIsCredit() const { return isCredit; }

    void display() const {
        cout << "Date: " << date << ", Category: " << category
             << ", Amount: " << fixed << setprecision(2) << amount
             << ", Description: " << description
             << ", Type: " << (isCredit ? "Credit" : "Debit") << endl;
    }

    string toCSV() const {
        return date + "," + category + "," + to_string(amount) + "," + description + "," + (isCredit ? "1" : "0");
    }

    static Transaction fromCSV(const string &line) {
        stringstream ss(line);
        string date, category, description, creditStr;
        double amount;
        getline(ss, date, ',');
        getline(ss, category, ',');
        ss >> amount;
        ss.ignore();
        getline(ss, description, ',');
        getline(ss, creditStr);
        return Transaction(amount, category, date, description, creditStr == "1");
    }
};

bool isValidMonthYear(const string &monthYear) {
    int year, month;
    char sep;
    if (monthYear.length() != 7) return false;  // Ensure strict length
    stringstream ss(monthYear);
    ss >> year >> sep >> month;
    return !ss.fail() && sep == '-' && month >= 1 && month <= 12;
}

class Account {
    double balance = 0;
    vector<Transaction> transactions;

public:
    const vector<Transaction> &getTransactions() const { return transactions; }

    void addTransaction(const Transaction &t) {
        balance += t.getIsCredit() ? t.getAmount() : -t.getAmount();
        transactions.push_back(t);
        saveToFile();
    }

    void displayBalance() const {
        cout << "Current Balance: " << fixed << setprecision(2) << balance << endl;
    }

    void displayAll() const {
        displayBalance();
        cout << "All Transactions:\n";
        for (const auto &t : transactions) t.display();
    }

    void searchTransactions(const string &field, const string &value) const {
        cout << "Searching for transactions with " << field << ": " << value << "\n";
        bool found = false;
        for (const auto &t : transactions) {
            if ((field == "category" && t.getCategory() == value) ||
                (field == "date" && t.getDate() == value)) {
                t.display();
                found = true;
            }
        }
        if (!found) {
            cout << "No match found for the " << field << " entered. Please try again.\n";
        }
    }

    void displayMonthlyReport(const string &monthYear) const {
        if (!isValidMonthYear(monthYear)) {
            cout << "Invalid format for month and year. Please use YYYY-MM.\n";
            return;
        }

        cout << "Transactions for " << monthYear << ":\n";
        bool found = false;

        for (const auto &t : transactions) {
            if (t.getDate().substr(0, 7) == monthYear) {
                t.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No transactions found for the month " << monthYear << ".\n";
        }
    }

    void saveToFile() const {
        ofstream file("transactions.csv");
        for (const auto &t : transactions) file << t.toCSV() << endl;
    }

    void loadFromFile() {
        ifstream file("transactions.csv");
        string line;
        while (getline(file, line)) {
            Transaction t = Transaction::fromCSV(line);
            transactions.push_back(t);
            balance += t.getIsCredit() ? t.getAmount() : -t.getAmount();
        }
    }

    void sortTransactionsByDate() {
        sort(transactions.begin(), transactions.end(), [](const Transaction &a, const Transaction &b) {
            return a.getDate() < b.getDate();
        });
    }

    void sortTransactionsByAmount() {
        sort(transactions.begin(), transactions.end(), [](const Transaction &a, const Transaction &b) {
            return a.getAmount() > b.getAmount();
        });
    }
};

bool isValidDate(const string &date) {
    int y, m, d;
    char sep1, sep2;
    if (date.length() != 10) return false;  // Ensure strict length
    stringstream ss(date);
    ss >> y >> sep1 >> m >> sep2 >> d;
    if (ss.fail() || sep1 != '-' || sep2 != '-') return false;
    if (m < 1 || m > 12 || d < 1 || d > 31) return false;
    if (m == 2 && d > (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0) ? 29 : 28)) return false;
    return true;
}


string getInput(const string &prompt) {
    cout << prompt;
    string input;
    getline(cin >> ws, input);
    return input;
}

int main() {
    Account account;
    account.loadFromFile();

    while (true) {
        cout << "\nMenu:\n1. Add Transaction\n2. View All Transactions\n3. Search Transactions\n4. Monthly Report\n5. Sort Transactions\n6. Exit\nChoose an option: ";
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            double amount = stod(getInput("Enter amount: "));
            string category = getInput("Enter category: ");
            string date;

            do {
                date = getInput("Enter date (YYYY-MM-DD): ");
                if (!isValidDate(date)) cout << "Invalid format. Please enter a valid date in YYYY-MM-DD.\n";
            } while (!isValidDate(date));

            string description = getInput("Enter description: ");
            bool isCredit = stoi(getInput("Is this Credit (1) or Debit (0)? "));

            account.addTransaction(Transaction(amount, category, date, description, isCredit));
            cout << "Transaction added successfully.\n";

        } else if (choice == 2) {
            account.displayAll();

        } else if (choice == 3) {
            cout << "Search by: 1. Category 2. Date\nChoose an option: ";
            int searchChoice;
            cin >> searchChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            string field = (searchChoice == 1 ? "category" : "date");
            string value;

            if (searchChoice == 2) {
                do {
                    value = getInput("Enter date (YYYY-MM-DD): ");
                    if (!isValidDate(value)) cout << "Invalid format. Please enter a valid date in YYYY-MM-DD.\n";
                } while (!isValidDate(value));
            } else {
                value = getInput("Enter category: ");
            }

            account.searchTransactions(field, value);

        } else if (choice == 4) {
            string monthYear;
            do {
                monthYear = getInput("Enter month and year (YYYY-MM): ");
                if (!isValidMonthYear(monthYear)) cout << "Invalid format. Please enter in YYYY-MM format.\n";
            } while (!isValidMonthYear(monthYear));

            account.displayMonthlyReport(monthYear);

        } else if (choice == 5) {
            cout << "Sort by: 1. Date 2. Amount\nChoose an option: ";
            int sortChoice;
            cin >> sortChoice;

            if (sortChoice == 1) {
                account.sortTransactionsByDate();
                cout << "Transactions sorted by date:\n";
                account.displayAll();  // Display the sorted transactions
            } else if (sortChoice == 2) {
                account.sortTransactionsByAmount();
                cout << "Transactions sorted by amount:\n";
                account.displayAll();  // Display the sorted transactions
            } else {
                cout << "Invalid choice.\n";
            }
        } else if (choice == 6) {
            cout << "Exiting program. Goodbye!\n";
            break;
        } else {
            cout << "Invalid option. Please try again.\n";
        }
    }

    return 0;
}
