#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
using namespace std;
struct Account {
    string accountNumber;
    string accountHolder;
    double balance;
    string accountType;
    string createdDate;
    Account() : balance(0.0) {}  
    Account(const string& number, const string& holder, 
            double bal, const string& type, const string& date)
        : accountNumber(number), accountHolder(holder), balance(bal), 
          accountType(type), createdDate(date) {}
};
struct Transaction {
    string accountNumber;
    string transactionType;
    double amount;
    double balanceAfter;
    string transactionDate;
    string description; 
    Transaction() : amount(0.0), balanceAfter(0.0) {} 
    Transaction(const string& accNum, const string& type, 
                double amt, double balAfter, const string& date, const string& desc)
        : accountNumber(accNum), transactionType(type), amount(amt), 
          balanceAfter(balAfter), transactionDate(date), description(desc) {}
};
class BankManagementSystem {
private:
    const string ACCOUNTS_FILE = "accounts.csv";
    const string TRANSACTIONS_FILE = "transactions.csv";
    vector<Account> accounts;
    vector<Transaction> transactions; 
    string getCurrentTimestamp() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string timestamp(dt);
        timestamp.pop_back();
        return timestamp;
    }
    vector<string> splitCSV(const string& line) {
        vector<string> result;
        stringstream ss(line);
        string cell; 
        while (getline(ss, cell, ',')) {
            if (cell.front() == '"' && cell.back() == '"') {
                cell = cell.substr(1, cell.length() - 2);
            }
            result.push_back(cell);
        }
        return result;
    }
    string escapeCSV(const string& field) {
        if (field.find(',') != string::npos || field.find('"') != string::npos) {
            string escaped = field;
            // Replace " with ""
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != string::npos) {
                escaped.replace(pos, 1, "\"\"");
                pos += 2;
            }
            return "\"" + escaped + "\"";
        }
        return field;
    }   
    void loadAccounts() {
        accounts.clear();
        ifstream file(ACCOUNTS_FILE);
        
        if (!file.is_open()) {
            ofstream outFile(ACCOUNTS_FILE);
            if (outFile.is_open()) {
                outFile << "Account Number,Account Holder,Balance,Account Type,Created Date\n";
                outFile.close();
            }
            return;
        }      
        string line;
        bool firstLine = true;
        
        while (getline(file, line)) {
            if (firstLine) {
                firstLine = false;
                continue;
            }     
            if (line.empty()) continue; 
            auto fields = splitCSV(line);
            if (fields.size() >= 5) {
                Account acc(fields[0], fields[1], stod(fields[2]), fields[3], fields[4]);
                accounts.push_back(acc);
            }
        }   
        file.close();
    }
    void saveAccounts() {
        ofstream file(ACCOUNTS_FILE);  
        if (!file.is_open()) {
            cerr << "Error: Could not open accounts file for writing!" << endl;
            return;
        }
        file << "Account Number,Account Holder,Balance,Account Type,Created Date\n";
        for (const auto& acc : accounts) {
            file << escapeCSV(acc.accountNumber) << ","
                 << escapeCSV(acc.accountHolder) << ","
                 << fixed << setprecision(2) << acc.balance << ","
                 << escapeCSV(acc.accountType) << ","
                 << escapeCSV(acc.createdDate) << "\n";
        }
        file.close();
    }
    void loadTransactions() {
        transactions.clear();
        ifstream file(TRANSACTIONS_FILE);
        if (!file.is_open()) {
            ofstream outFile(TRANSACTIONS_FILE);
            if (outFile.is_open()) {
                outFile << "Account Number,Transaction Type,Amount,Balance After,Transaction Date,Description\n";
                outFile.close();
            }
            return;
        }     
        string line;
        bool firstLine = true;
        while (getline(file, line)) {
            if (firstLine) {
                firstLine = false;
                continue;
            } 
            if (line.empty()) continue;
            auto fields = splitCSV(line);
            if (fields.size() >= 6) {
                Transaction trans(fields[0], fields[1], stod(fields[2]), 
                                stod(fields[3]), fields[4], fields[5]);
                transactions.push_back(trans);
            }
        }  
        file.close();
    }
    void saveTransactions() {
        ofstream file(TRANSACTIONS_FILE);
        if (!file.is_open()) {
            cerr << "Error: Could not open transactions file for writing!" << endl;
            return;
        }
        file << "Account Number,Transaction Type,Amount,Balance After,Transaction Date,Description\n";
        for (const auto& trans : transactions) {
            file << escapeCSV(trans.accountNumber) << ","
                 << escapeCSV(trans.transactionType) << ","
                 << fixed << setprecision(2) << trans.amount << ","
                 << fixed << setprecision(2) << trans.balanceAfter << ","
                 << escapeCSV(trans.transactionDate) << ","
                 << escapeCSV(trans.description) << "\n";
        }
        file.close();
    }
    Account* findAccount(const string& accountNumber) {
        auto it = find_if(accounts.begin(), accounts.end(),
                              [&accountNumber](const Account& acc) {
                                  return acc.accountNumber == accountNumber;
                              });
        
        return (it != accounts.end()) ? &(*it) : nullptr;
    }
public:
    BankManagementSystem() {
        loadAccounts();
        loadTransactions();
    }
    ~BankManagementSystem() {
        saveAccounts();
        saveTransactions();
    }
    bool createAccount(const string& accountNumber, const string& holderName, 
                      const string& accountType, double initialDeposit = 0.0) {
        if (findAccount(accountNumber) != nullptr) {
            cout << "Error: Account number already exists!" << endl;
            return false;
        }
        Account newAccount(accountNumber, holderName, initialDeposit, accountType, getCurrentTimestamp());
        accounts.push_back(newAccount);       
        if (initialDeposit > 0) {
            addTransaction(accountNumber, "DEPOSIT", initialDeposit, initialDeposit, "Initial deposit");
        }     
        saveAccounts();
        cout << "Account created successfully!" << endl;
        return true;
    }
    bool deposit(const string& accountNumber, double amount) {
        if (amount <= 0) {
            cout << "Invalid amount. Amount must be positive." << endl;
            return false;
        }    
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "Account not found!" << endl;
            return false;
        }    
        account->balance += amount;
        addTransaction(accountNumber, "DEPOSIT", amount, account->balance, "Cash deposit");
        
        saveAccounts();
        cout << "Deposit successful! New balance: $" << fixed << setprecision(2) << account->balance << endl;
        return true;
    } 
    bool withdraw(const string& accountNumber, double amount) {
        if (amount <= 0) {
            cout << "Invalid amount. Amount must be positive." << endl;
            return false;
        } 
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "Account not found!" << endl;
            return false;
        }
        
        if (account->balance < amount) {
            cout << "Insufficient funds! Current balance: $" << fixed << setprecision(2) << account->balance << endl;
            return false;
        }
        
        account->balance -= amount;
        addTransaction(accountNumber, "WITHDRAWAL", amount, account->balance, "Cash withdrawal");
        
        saveAccounts();
        cout << "Withdrawal successful! New balance: $" << fixed << setprecision(2) << account->balance << endl;
        return true;
    }
    
    void checkBalance(const string& accountNumber) {
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "Account not found!" << endl;
            return;
        }
        
        cout << "\n=== BALANCE INQUIRY ===" << endl;
        cout << "Account Number: " << account->accountNumber << endl;
        cout << "Account Holder: " << account->accountHolder << endl;
        cout << "Account Type: " << account->accountType << endl;
        cout << "Current Balance: $" << fixed << setprecision(2) << account->balance << endl;
        cout << "=====================" << endl;
    }
    
    void addTransaction(const string& accountNumber, const string& type, 
                       double amount, double balanceAfter, const string& description) {
        Transaction newTransaction(accountNumber, type, amount, balanceAfter, getCurrentTimestamp(), description);
        transactions.push_back(newTransaction);
        saveTransactions();
    }
    
    void viewTransactionHistory(const string& accountNumber, int limit = 10) {
        vector<Transaction> accountTransactions;
        
        // Filter transactions for the specific account
        copy_if(transactions.begin(), transactions.end(), back_inserter(accountTransactions),
                    [&accountNumber](const Transaction& trans) {
                        return trans.accountNumber == accountNumber;
                    });
        
        if (accountTransactions.empty()) {
            cout << "No transactions found for account: " << accountNumber << endl;
            return;
        }
        
        // Sort by date (most recent first) - simplified sorting
        reverse(accountTransactions.begin(), accountTransactions.end());
        
        // Limit results
        if (accountTransactions.size() > static_cast<size_t>(limit)) {
            accountTransactions.resize(limit);
        }
        
        cout << "\n=== TRANSACTION HISTORY ===" << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << left << setw(12) << "Type" << setw(12) << "Amount" 
                  << setw(15) << "Balance After" << setw(25) << "Date" << "Description" << endl;
        cout << string(80, '-') << endl;
        
        for (const auto& trans : accountTransactions) {
            cout << left << setw(12) << trans.transactionType 
                      << "$" << setw(11) << fixed << setprecision(2) << trans.amount
                      << "$" << setw(14) << fixed << setprecision(2) << trans.balanceAfter
                      << setw(25) << trans.transactionDate << trans.description << endl;
        }
        cout << "==========================" << endl;
    }
    
    void listAllAccounts() {
        if (accounts.empty()) {
            cout << "No accounts found!" << endl;
            return;
        }
        
        cout << "\n=== ALL ACCOUNTS ===" << endl;
        cout << left << setw(15) << "Account No." << setw(20) << "Account Holder" 
                  << setw(12) << "Balance" << setw(12) << "Type" << "Created Date" << endl;
        cout << string(90, '-') << endl;
        
        for (const auto& account : accounts) {
            cout << left << setw(15) << account.accountNumber 
                      << setw(20) << account.accountHolder 
                      << "$" << setw(11) << fixed << setprecision(2) << account.balance
                      << setw(12) << account.accountType 
                      << account.createdDate << endl;
        }
        cout << "===================" << endl;
    }
    
    bool deleteAccount(const string& accountNumber) {
        auto it = find_if(accounts.begin(), accounts.end(),
                              [&accountNumber](const Account& acc) {
                                  return acc.accountNumber == accountNumber;
                              });
        
        if (it == accounts.end()) {
            cout << "Account not found!" << endl;
            return false;
        }
        
        // Remove account
        accounts.erase(it);
        
        // Remove all transactions for this account
        transactions.erase(
            remove_if(transactions.begin(), transactions.end(),
                          [&accountNumber](const Transaction& trans) {
                              return trans.accountNumber == accountNumber;
                          }),
            transactions.end()
        );
        
        saveAccounts();
        saveTransactions();
        
        cout << "Account deleted successfully!" << endl;
        return true;
    }
    
    void exportToExcel() {
        // Create a comprehensive Excel report
        ofstream reportFile("bank_report.csv");
        
        if (!reportFile.is_open()) {
            cout << "Error: Could not create report file!" << endl;
            return;
        }
        
        reportFile << "BANK MANAGEMENT SYSTEM REPORT\n";
        reportFile << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        // Account Summary
        reportFile << "ACCOUNT SUMMARY\n";
        reportFile << "Account Number,Account Holder,Balance,Account Type,Created Date\n";
        
        for (const auto& acc : accounts) {
            reportFile << escapeCSV(acc.accountNumber) << ","
                      << escapeCSV(acc.accountHolder) << ","
                      << fixed << setprecision(2) << acc.balance << ","
                      << escapeCSV(acc.accountType) << ","
                      << escapeCSV(acc.createdDate) << "\n";
        }
        
        reportFile << "\n\nTRANSACTION HISTORY\n";
        reportFile << "Account Number,Transaction Type,Amount,Balance After,Date,Description\n";
        
        for (const auto& trans : transactions) {
            reportFile << escapeCSV(trans.accountNumber) << ","
                      << escapeCSV(trans.transactionType) << ","
                      << fixed << setprecision(2) << trans.amount << ","
                      << fixed << setprecision(2) << trans.balanceAfter << ","
                      << escapeCSV(trans.transactionDate) << ","
                      << escapeCSV(trans.description) << "\n";
        }
        
        reportFile.close();
        cout << "Report exported to 'bank_report.csv' successfully!" << endl;
    }
    
    void displayMenu() {
        cout << "\n========== BANK MANAGEMENT SYSTEM ==========" << endl;
        cout << "1. Create Account" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. Check Balance" << endl;
        cout << "5. View Transaction History" << endl;
        cout << "6. List All Accounts" << endl;
        cout << "7. Delete Account" << endl;
        cout << "8. Export to Excel Report" << endl;
        cout << "9. Exit" << endl;
        cout << "===========================================" << endl;
        cout << "Enter your choice: ";
    }
};

int main() {
    BankManagementSystem bank;
    int choice;
    string accountNumber, holderName, accountType;
    double amount;
    
    cout << "Welcome to Bank Management System!" << endl;
    cout << "Data will be stored in CSV files (Excel compatible)" << endl;
    
    while (true) {
        bank.displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "Enter account number: ";
                cin >> accountNumber;
                cin.ignore(); // Clear the input buffer
                cout << "Enter account holder name: ";
                getline(cin, holderName);
                cout << "Enter account type (Savings/Checking/Business): ";
                getline(cin, accountType);
                cout << "Enter initial deposit amount (0 for no deposit): $";
                cin >> amount;
                
                bank.createAccount(accountNumber, holderName, accountType, amount);
                break;
            }
            case 2: {
                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter deposit amount: $";
                cin >> amount;
                
                bank.deposit(accountNumber, amount);
                break;
            }
            case 3: {
                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter withdrawal amount: $";
                cin >> amount;
                
                bank.withdraw(accountNumber, amount);
                break;
            }
            case 4: {
                cout << "Enter account number: ";
                cin >> accountNumber;
                
                bank.checkBalance(accountNumber);
                break;
            }
            case 5: {
                cout << "Enter account number: ";
                cin >> accountNumber;
                
                bank.viewTransactionHistory(accountNumber);
                break;
            }
            case 6: {
                bank.listAllAccounts();
                break;
            }
            case 7: {
                cout << "Enter account number to delete: ";
                cin >> accountNumber;
                
                cout << "Are you sure you want to delete this account? (y/n): ";
                char confirm;
                cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    bank.deleteAccount(accountNumber);
                } else {
                    cout << "Account deletion cancelled." << endl;
                }
                break;
            }
            case 8: {
                bank.exportToExcel();
                break;
            }
            case 9: {
                cout << "Thank you for using Bank Management System!" << endl;
                return 0;
            }
            default: {
                cout << "Invalid choice! Please try again." << endl;
                break;
            }
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
    return 0;
}