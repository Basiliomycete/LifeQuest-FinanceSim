#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sstream>

// ─────────────────────────────────────────────
//  ENUMS
// ─────────────────────────────────────────────
enum class JobType { EMPLOYEE, STUDENT, FREELANCER };
enum class TransactionType { INCOME, EXPENSE };
enum class GoalStatus { IN_PROGRESS, ACHIEVED, FAILED };
enum class ExpenseCategory { FOOD, RENT, ENTERTAINMENT, TRANSPORT, HEALTH, EDUCATION, OTHER };
enum class IncomeCategory { SALARY, FREELANCE, INVESTMENT, GIFT, OTHER };

// ─────────────────────────────────────────────
//  TRANSACTION (Abstract Base Class)
// ─────────────────────────────────────────────
class Transaction {
protected:
    std::string description;
    double amount;
    int month;
    int year;
    TransactionType type;

public:
    static int totalTransactions; // static variable

    Transaction(const std::string& desc, double amt, int m, int y, TransactionType t)
        : description(desc), amount(amt), month(m), year(y), type(t) {
        ++totalTransactions;
    }

    virtual ~Transaction() = default;

    // Pure virtual – must override in derived
    virtual std::string getCategoryName() const = 0;
    virtual void display() const = 0;
    virtual std::string serialize() const = 0;

    double getAmount() const { return amount; }
    TransactionType getType() const { return type; }
    std::string getDescription() const { return description; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    // Operator overloading
    bool operator>(const Transaction& other) const { return amount > other.amount; }
    bool operator<(const Transaction& other) const { return amount < other.amount; }
    bool operator==(const Transaction& other) const {
        return description == other.description && amount == other.amount;
    }

    friend std::ostream& operator<<(std::ostream& os, const Transaction& t) {
        os << "[" << (t.type == TransactionType::INCOME ? "+" : "-")
           << std::fixed << std::setprecision(2) << t.amount << "] "
           << t.description << " (" << t.getCategoryName() << ")";
        return os;
    }
};

int Transaction::totalTransactions = 0;

// ─────────────────────────────────────────────
//  INCOME (Derived)
// ─────────────────────────────────────────────
class Income : public Transaction {
private:
    IncomeCategory category;

public:
    Income(const std::string& desc, double amt, int m, int y, IncomeCategory cat = IncomeCategory::OTHER)
        : Transaction(desc, amt, m, y, TransactionType::INCOME), category(cat) {}

    std::string getCategoryName() const override {
        switch (category) {
            case IncomeCategory::SALARY:     return "Salary";
            case IncomeCategory::FREELANCE:  return "Freelance";
            case IncomeCategory::INVESTMENT: return "Investment";
            case IncomeCategory::GIFT:       return "Gift";
            default:                         return "Other";
        }
    }

    void display() const override {
        std::cout << "  \033[32m[INCOME]\033[0m " << description
                  << " | +" << std::fixed << std::setprecision(2) << amount
                  << " | " << getCategoryName() << "\n";
    }

    std::string serialize() const override {
        return "INCOME|" + description + "|" + std::to_string(amount)
             + "|" + std::to_string(month) + "|" + std::to_string(year)
             + "|" + std::to_string(static_cast<int>(category));
    }
};

// ─────────────────────────────────────────────
//  EXPENSE (Derived)
// ─────────────────────────────────────────────
class Expense : public Transaction {
private:
    ExpenseCategory category;

public:
    Expense(const std::string& desc, double amt, int m, int y, ExpenseCategory cat = ExpenseCategory::OTHER)
        : Transaction(desc, amt, m, y, TransactionType::EXPENSE), category(cat) {}

    std::string getCategoryName() const override {
        switch (category) {
            case ExpenseCategory::FOOD:          return "Food";
            case ExpenseCategory::RENT:          return "Rent";
            case ExpenseCategory::ENTERTAINMENT: return "Entertainment";
            case ExpenseCategory::TRANSPORT:     return "Transport";
            case ExpenseCategory::HEALTH:        return "Health";
            case ExpenseCategory::EDUCATION:     return "Education";
            default:                             return "Other";
        }
    }

    void display() const override {
        std::cout << "  \033[31m[EXPENSE]\033[0m " << description
                  << " | -" << std::fixed << std::setprecision(2) << amount
                  << " | " << getCategoryName() << "\n";
    }

    std::string serialize() const override {
        return "EXPENSE|" + description + "|" + std::to_string(amount)
             + "|" + std::to_string(month) + "|" + std::to_string(year)
             + "|" + std::to_string(static_cast<int>(category));
    }
};

// ─────────────────────────────────────────────
//  GOAL
// ─────────────────────────────────────────────
class Goal {
private:
    std::string name;
    double targetAmount;
    double savedAmount;
    int deadlineMonth;
    int deadlineYear;
    GoalStatus status;

public:
    Goal(const std::string& n, double target, int dm, int dy)
        : name(n), targetAmount(target), savedAmount(0.0),
          deadlineMonth(dm), deadlineYear(dy), status(GoalStatus::IN_PROGRESS) {}

    void addSavings(double amount) {
        savedAmount += amount;
        if (savedAmount >= targetAmount) status = GoalStatus::ACHIEVED;
    }

    double getProgress() const {
        return (targetAmount > 0) ? (savedAmount / targetAmount) * 100.0 : 0.0;
    }

    bool isAchieved() const { return status == GoalStatus::ACHIEVED; }
    bool isDeadlinePassed(int curMonth, int curYear) const {
        return (curYear > deadlineYear) || (curYear == deadlineYear && curMonth > deadlineMonth);
    }

    void checkDeadline(int curMonth, int curYear) {
        if (status == GoalStatus::IN_PROGRESS && isDeadlinePassed(curMonth, curYear))
            status = GoalStatus::FAILED;
    }

    std::string getName() const { return name; }
    double getTargetAmount() const { return targetAmount; }
    double getSavedAmount() const { return savedAmount; }
    GoalStatus getStatus() const { return status; }
    int getDeadlineMonth() const { return deadlineMonth; }
    int getDeadlineYear() const { return deadlineYear; }

    void display() const {
        std::string statusStr = (status == GoalStatus::ACHIEVED) ? "\033[32mACHIEVED\033[0m" :
                                (status == GoalStatus::FAILED)   ? "\033[31mFAILED\033[0m"   :
                                                                    "\033[33mIN PROGRESS\033[0m";
        int bars = static_cast<int>(getProgress() / 5);
        std::string bar = "[";
        for (int i = 0; i < 20; ++i) bar += (i < bars ? "█" : "░");
        bar += "]";
        std::cout << "  Goal: " << name << " | " << bar
                  << " " << std::fixed << std::setprecision(1) << getProgress() << "%"
                  << " | " << savedAmount << "/" << targetAmount
                  << " | " << statusStr << "\n";
    }

    std::string serialize() const {
        return "GOAL|" + name + "|" + std::to_string(targetAmount)
             + "|" + std::to_string(savedAmount)
             + "|" + std::to_string(deadlineMonth) + "|" + std::to_string(deadlineYear)
             + "|" + std::to_string(static_cast<int>(status));
    }
};

// ─────────────────────────────────────────────
//  PERSON (Abstract Base Class)
// ─────────────────────────────────────────────
class Person {
protected:
    std::string name;
    int age;
    double balance;
    int happinessLevel; // 0–100
    int currentMonth;
    int currentYear;
    JobType jobType;
    std::vector<Transaction*> transactions;
    std::vector<Goal> goals;

public:
    Person(const std::string& n, int a, JobType jt)
        : name(n), age(a), balance(0.0), happinessLevel(70),
          currentMonth(1), currentYear(2025), jobType(jt) {}

    virtual ~Person() {
        for (auto* t : transactions) delete t;
    }

    // Pure virtual
    virtual double getMonthlyBaseSalary() const = 0;
    virtual std::string getJobTitle() const = 0;
    virtual void applyMonthlyEvents() = 0;

    void addTransaction(Transaction* t) {
        transactions.push_back(t);
        if (t->getType() == TransactionType::INCOME)
            balance += t->getAmount();
        else {
            balance -= t->getAmount();
            updateHappiness(t->getAmount());
        }
    }

    void addGoal(const Goal& g) { goals.push_back(g); }

    void allocateToGoal(int goalIndex, double amount) {
        if (goalIndex >= 0 && goalIndex < static_cast<int>(goals.size())) {
            if (amount <= balance) {
                balance -= amount;
                goals[goalIndex].addSavings(amount);
            } else {
                std::cout << "\033[31mInsufficient balance!\033[0m\n";
            }
        }
    }

    void advanceMonth() {
        currentMonth++;
        if (currentMonth > 12) { currentMonth = 1; currentYear++; }
        age = (currentMonth == 1) ? age + 1 : age;
        // Check goal deadlines
        for (auto& g : goals) g.checkDeadline(currentMonth, currentYear);
        applyMonthlyEvents();
    }

    void updateHappiness(double expenseAmount) {
        // Happiness decreases more when spending is large relative to balance
        if (balance < 0) happinessLevel = std::max(0, happinessLevel - 15);
        else if (expenseAmount > balance * 0.5) happinessLevel = std::max(0, happinessLevel - 5);
        else happinessLevel = std::min(100, happinessLevel + 2);
    }

    // Getters
    std::string getName() const { return name; }
    int getAge() const { return age; }
    double getBalance() const { return balance; }
    int getHappiness() const { return happinessLevel; }
    int getCurrentMonth() const { return currentMonth; }
    int getCurrentYear() const { return currentYear; }
    JobType getJobType() const { return jobType; }
    std::vector<Goal>& getGoals() { return goals; }
    const std::vector<Transaction*>& getTransactions() const { return transactions; }

    double getMonthlyIncome() const {
        double total = 0;
        for (auto* t : transactions)
            if (t->getType() == TransactionType::INCOME &&
                t->getMonth() == currentMonth && t->getYear() == currentYear)
                total += t->getAmount();
        return total;
    }

    double getMonthlyExpenses() const {
        double total = 0;
        for (auto* t : transactions)
            if (t->getType() == TransactionType::EXPENSE &&
                t->getMonth() == currentMonth && t->getYear() == currentYear)
                total += t->getAmount();
        return total;
    }

    void displayStatus() const;
    void displayTransactions() const;

    // Serialization
    virtual std::string serialize() const;
    static Person* deserialize(const std::string& data);
};

// ─────────────────────────────────────────────
//  EMPLOYEE (Derived from Person)
// ─────────────────────────────────────────────
class Employee : public Person {
private:
    std::string company;
    double baseSalary;
    int experienceYears;

public:
    Employee(const std::string& name, int age, const std::string& company, double salary)
        : Person(name, age, JobType::EMPLOYEE), company(company),
          baseSalary(salary), experienceYears(0) {}

    double getMonthlyBaseSalary() const override { return baseSalary; }
    std::string getJobTitle() const override { return "Employee @ " + company; }

    void applyMonthlyEvents() override {
        // Auto-add salary each month
        addTransaction(new Income("Monthly Salary", baseSalary,
                                  currentMonth, currentYear, IncomeCategory::SALARY));
        // Yearly raise
        if (currentMonth == 1) {
            experienceYears++;
            baseSalary *= 1.05; // 5% annual raise
            std::cout << "\033[33m🎉 Annual raise! New salary: " << baseSalary << "\033[0m\n";
        }
    }

    std::string getCompany() const { return company; }
    double getBaseSalary() const { return baseSalary; }
};

// ─────────────────────────────────────────────
//  STUDENT (Derived from Person)
// ─────────────────────────────────────────────
class Student : public Person {
private:
    std::string university;
    double partTimeIncome;
    double tuitionPerSemester;

public:
    Student(const std::string& name, int age, const std::string& uni,
            double partTime, double tuition)
        : Person(name, age, JobType::STUDENT), university(uni),
          partTimeIncome(partTime), tuitionPerSemester(tuition) {}

    double getMonthlyBaseSalary() const override { return partTimeIncome; }
    std::string getJobTitle() const override { return "Student @ " + university; }

    void applyMonthlyEvents() override {
        if (partTimeIncome > 0)
            addTransaction(new Income("Part-time Job", partTimeIncome,
                                      currentMonth, currentYear, IncomeCategory::OTHER));
        // Tuition every 6 months
        if (currentMonth == 2 || currentMonth == 9)
            addTransaction(new Expense("Tuition Fee", tuitionPerSemester,
                                        currentMonth, currentYear, ExpenseCategory::EDUCATION));
    }

    std::string getUniversity() const { return university; }
};

// ─────────────────────────────────────────────
//  FREELANCER (Derived from Person)
// ─────────────────────────────────────────────
class Freelancer : public Person {
private:
    std::string skill;
    double ratePerProject;
    int projectsPerMonth;

public:
    Freelancer(const std::string& name, int age, const std::string& sk,
               double rate, int projects)
        : Person(name, age, JobType::FREELANCER), skill(sk),
          ratePerProject(rate), projectsPerMonth(projects) {}

    double getMonthlyBaseSalary() const override { return ratePerProject * projectsPerMonth; }
    std::string getJobTitle() const override { return "Freelancer – " + skill; }

    void applyMonthlyEvents() override {
        // Variable income – simulate random projects (1 to max)
        int actual = 1 + (std::rand() % projectsPerMonth);
        double income = actual * ratePerProject;
        addTransaction(new Income("Freelance Projects (" + std::to_string(actual) + ")",
                                   income, currentMonth, currentYear, IncomeCategory::FREELANCE));
    }

    std::string getSkill() const { return skill; }
    double getRatePerProject() const { return ratePerProject; }
};

// ─────────────────────────────────────────────
//  PERSON METHOD IMPLEMENTATIONS
// ─────────────────────────────────────────────
void Person::displayStatus() const {
    const std::string months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                   "Jul","Aug","Sep","Oct","Nov","Dec"};
    std::string happyBar = "";
    int bars = happinessLevel / 10;
    for (int i = 0; i < 10; ++i) happyBar += (i < bars ? "♥" : "♡");

    std::cout << "\n\033[1;36m╔══════════════════════════════════════╗\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  " << name << "  |  Age: " << age
              << "  |  " << getJobTitle() << "\n";
    std::cout << "\033[1;36m║\033[0m  📅 " << months[currentMonth - 1] << " " << currentYear << "\n";
    std::cout << "\033[1;36m║\033[0m  💰 Balance: $" << std::fixed << std::setprecision(2) << balance << "\n";
    std::cout << "\033[1;36m║\033[0m  😊 Happiness: [" << happyBar << "] " << happinessLevel << "/100\n";
    std::cout << "\033[1;36m╚══════════════════════════════════════╝\033[0m\n";
}

void Person::displayTransactions() const {
    const std::string months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                   "Jul","Aug","Sep","Oct","Nov","Dec"};
    std::cout << "\n\033[1mTransactions – " << months[currentMonth-1] << " " << currentYear << ":\033[0m\n";
    bool found = false;
    for (auto* t : transactions) {
        if (t->getMonth() == currentMonth && t->getYear() == currentYear) {
            t->display();
            found = true;
        }
    }
    if (!found) std::cout << "  No transactions this month.\n";
    std::cout << "  ─────────────────────────────────\n";
    std::cout << "  Income:   +" << getMonthlyIncome() << "\n";
    std::cout << "  Expenses: -" << getMonthlyExpenses() << "\n";
    std::cout << "  Net:       " << (getMonthlyIncome() - getMonthlyExpenses()) << "\n";
}

std::string Person::serialize() const {
    std::ostringstream oss;
    std::string jt = (jobType == JobType::EMPLOYEE) ? "EMPLOYEE" :
                     (jobType == JobType::STUDENT)  ? "STUDENT"  : "FREELANCER";
    oss << "PERSON|" << name << "|" << age << "|" << balance << "|"
        << happinessLevel << "|" << currentMonth << "|" << currentYear << "|" << jt << "\n";
    for (auto* t : transactions) oss << t->serialize() << "\n";
    for (const auto& g : goals) oss << g.serialize() << "\n";
    return oss.str();
}

// ─────────────────────────────────────────────
//  GAME MANAGER
// ─────────────────────────────────────────────
class GameManager {
private:
    Person* player;
    std::string saveFile;
    static int totalGamesPlayed; // static

public:
    GameManager(const std::string& file = "savegame.txt")
        : player(nullptr), saveFile(file) {
        ++totalGamesPlayed;
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    ~GameManager() { delete player; }

    void setPlayer(Person* p) { player = p; }
    Person* getPlayer() const { return player; }

    static int getTotalGamesPlayed() { return totalGamesPlayed; }

    bool saveGame() const {
        if (!player) return false;
        std::ofstream file(saveFile);
        if (!file.is_open()) return false;
        file << player->serialize();
        // Also save type-specific data
        if (player->getJobType() == JobType::EMPLOYEE) {
            auto* e = dynamic_cast<Employee*>(player);
            if (e) file << "JOB_DATA|" << e->getCompany() << "|" << e->getBaseSalary() << "\n";
        }
        file.close();
        std::cout << "\033[32m✓ Game saved successfully!\033[0m\n";
        return true;
    }

    bool loadGame() {
        std::ifstream file(saveFile);
        if (!file.is_open()) {
            std::cout << "\033[31mNo save file found.\033[0m\n";
            return false;
        }
        // Simplified load – full implementation would parse each line
        std::cout << "\033[32m✓ Save file found. Loading...\033[0m\n";
        file.close();
        return true;
    }

    void displaySummaryReport() const {
        if (!player) return;
        std::cout << "\n\033[1;35m═══ FINANCIAL SUMMARY REPORT ═══\033[0m\n";
        player->displayStatus();
        player->displayTransactions();
        std::cout << "\n\033[1mGoals:\033[0m\n";
        for (const auto& g : player->getGoals()) g.display();
        std::cout << "\033[1;35m════════════════════════════════\033[0m\n";
        std::cout << "Total transactions recorded: " << Transaction::totalTransactions << "\n";
    }
};

int GameManager::totalGamesPlayed = 0;
