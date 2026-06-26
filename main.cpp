#include "SimLife.h"
#include <limits>

// ─────────────────────────────────────────────
//  UI HELPERS
// ─────────────────────────────────────────────
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader() {
    std::cout << "\033[1;35m";
    std::cout << R"(
  ███████╗██╗███╗   ███╗██╗     ██╗███████╗███████╗
  ██╔════╝██║████╗ ████║██║     ██║██╔════╝██╔════╝
  ███████╗██║██╔████╔██║██║     ██║█████╗  █████╗  
  ╚════██║██║██║╚██╔╝██║██║     ██║██╔══╝  ██╔══╝  
  ███████║██║██║ ╚═╝ ██║███████╗██║██║     ███████╗
  ╚══════╝╚═╝╚═╝     ╚═╝╚══════╝╚═╝╚═╝     ╚══════╝
)";
    std::cout << "\033[0m";
    std::cout << "\033[36m       Personal Finance Simulator  |  v1.0\033[0m\n\n";
}

void waitForEnter() {
    std::cout << "\n\033[90mPress Enter to continue...\033[0m";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int getIntInput(const std::string& prompt, int min, int max) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= min && val <= max) {
            std::cin.ignore();
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\033[31mInvalid input. Try again.\033[0m\n";
    }
}

double getDoubleInput(const std::string& prompt) {
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val > 0) {
            std::cin.ignore();
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\033[31mInvalid amount. Try again.\033[0m\n";
    }
}

// ─────────────────────────────────────────────
//  CHARACTER CREATION
// ─────────────────────────────────────────────
Person* createCharacter() {
    clearScreen();
    std::cout << "\033[1;33m╔═══════════════════════════════╗\n";
    std::cout << "║     CREATE YOUR CHARACTER     ║\n";
    std::cout << "╚═══════════════════════════════╝\033[0m\n\n";

    std::string name;
    std::cout << "Enter your character's name: ";
    std::getline(std::cin, name);

    int age = getIntInput("Enter age (18-60): ", 18, 60);

    std::cout << "\nChoose your career path:\n";
    std::cout << "  \033[32m1.\033[0m Employee    – Stable salary, annual raises\n";
    std::cout << "  \033[32m2.\033[0m Student     – Part-time income, tuition expenses\n";
    std::cout << "  \033[32m3.\033[0m Freelancer  – Variable income, project-based\n";

    int choice = getIntInput("\nYour choice (1-3): ", 1, 3);

    Person* p = nullptr;

    if (choice == 1) {
        std::string company;
        std::cout << "Enter company name: ";
        std::getline(std::cin, company);
        double salary = getDoubleInput("Monthly salary ($): ");
        p = new Employee(name, age, company, salary);
        std::cout << "\n\033[32m✓ Employee character created!\033[0m\n";
    }
    else if (choice == 2) {
        std::string uni;
        std::cout << "Enter university name: ";
        std::getline(std::cin, uni);
        double partTime = getDoubleInput("Monthly part-time income ($, 0 for none): ");
        double tuition  = getDoubleInput("Tuition per semester ($): ");
        p = new Student(name, age, uni, partTime, tuition);
        std::cout << "\n\033[32m✓ Student character created!\033[0m\n";
    }
    else {
        std::string skill;
        std::cout << "Enter your freelance skill (e.g. 'Web Dev'): ";
        std::getline(std::cin, skill);
        double rate = getDoubleInput("Rate per project ($): ");
        int maxProjects = getIntInput("Max projects per month (1-10): ", 1, 10);
        p = new Freelancer(name, age, skill, rate, maxProjects);
        std::cout << "\n\033[32m✓ Freelancer character created!\033[0m\n";
    }

    // Starting balance
    double startBalance = getDoubleInput("Starting balance ($): ");
    p->addTransaction(new Income("Starting Balance", startBalance, 1, 2025, IncomeCategory::GIFT));

    waitForEnter();
    return p;
}

// ─────────────────────────────────────────────
//  ADD EXPENSE MENU
// ─────────────────────────────────────────────
void addExpenseMenu(Person* p) {
    std::cout << "\n\033[1mExpense Categories:\033[0m\n";
    std::cout << "  1. Food\n  2. Rent\n  3. Entertainment\n";
    std::cout << "  4. Transport\n  5. Health\n  6. Education\n  7. Other\n";
    int cat = getIntInput("Category (1-7): ", 1, 7);

    std::string desc;
    std::cout << "Description: ";
    std::getline(std::cin, desc);
    double amount = getDoubleInput("Amount ($): ");

    ExpenseCategory ec = static_cast<ExpenseCategory>(cat - 1);
    p->addTransaction(new Expense(desc, amount, p->getCurrentMonth(), p->getCurrentYear(), ec));
    std::cout << "\033[32m✓ Expense recorded!\033[0m\n";
}

// ─────────────────────────────────────────────
//  ADD INCOME MENU
// ─────────────────────────────────────────────
void addIncomeMenu(Person* p) {
    std::cout << "\n\033[1mIncome Categories:\033[0m\n";
    std::cout << "  1. Salary\n  2. Freelance\n  3. Investment\n  4. Gift\n  5. Other\n";
    int cat = getIntInput("Category (1-5): ", 1, 5);

    std::string desc;
    std::cout << "Description: ";
    std::getline(std::cin, desc);
    double amount = getDoubleInput("Amount ($): ");

    IncomeCategory ic = static_cast<IncomeCategory>(cat - 1);
    p->addTransaction(new Income(desc, amount, p->getCurrentMonth(), p->getCurrentYear(), ic));
    std::cout << "\033[32m✓ Income recorded!\033[0m\n";
}

// ─────────────────────────────────────────────
//  ADD GOAL MENU
// ─────────────────────────────────────────────
void addGoalMenu(Person* p) {
    std::string name;
    std::cout << "Goal name (e.g. 'Buy a car'): ";
    std::getline(std::cin, name);
    double target = getDoubleInput("Target amount ($): ");
    int dm = getIntInput("Deadline month (1-12): ", 1, 12);
    int dy = getIntInput("Deadline year (2025-2030): ", 2025, 2030);

    p->addGoal(Goal(name, target, dm, dy));
    std::cout << "\033[32m✓ Goal added!\033[0m\n";
}

// ─────────────────────────────────────────────
//  SAVE TO GOAL MENU
// ─────────────────────────────────────────────
void saveToGoalMenu(Person* p) {
    auto& goals = p->getGoals();
    if (goals.empty()) {
        std::cout << "\033[31mNo goals set yet!\033[0m\n";
        return;
    }
    std::cout << "\nYour Goals:\n";
    for (int i = 0; i < static_cast<int>(goals.size()); ++i) {
        std::cout << "  " << (i+1) << ". ";
        goals[i].display();
    }
    int idx = getIntInput("Choose goal number: ", 1, static_cast<int>(goals.size()));
    double amount = getDoubleInput("Amount to allocate ($): ");
    p->allocateToGoal(idx - 1, amount);
    if (goals[idx-1].isAchieved())
        std::cout << "\033[33m🎉 GOAL ACHIEVED: " << goals[idx-1].getName() << "!\033[0m\n";
}

// ─────────────────────────────────────────────
//  MAIN GAME LOOP
// ─────────────────────────────────────────────
void gameLoop(GameManager& gm) {
    Person* p = gm.getPlayer();

    while (true) {
        clearScreen();
        printHeader();
        p->displayStatus();

        std::cout << "\n\033[1mWhat would you like to do?\033[0m\n";
        std::cout << "  \033[32m1.\033[0m Add Expense\n";
        std::cout << "  \033[32m2.\033[0m Add Income\n";
        std::cout << "  \033[32m3.\033[0m Add Financial Goal\n";
        std::cout << "  \033[32m4.\033[0m Save to a Goal\n";
        std::cout << "  \033[32m5.\033[0m View Transactions\n";
        std::cout << "  \033[32m6.\033[0m View All Goals\n";
        std::cout << "  \033[32m7.\033[0m Advance to Next Month\n";
        std::cout << "  \033[32m8.\033[0m Full Summary Report\n";
        std::cout << "  \033[32m9.\033[0m Save Game\n";
        std::cout << "  \033[31m0.\033[0m Quit\n";

        int choice = getIntInput("\n> ", 0, 9);

        switch (choice) {
            case 1: addExpenseMenu(p); break;
            case 2: addIncomeMenu(p); break;
            case 3: addGoalMenu(p); break;
            case 4: saveToGoalMenu(p); break;
            case 5: p->displayTransactions(); break;
            case 6:
                std::cout << "\nGoals:\n";
                for (const auto& g : p->getGoals()) g.display();
                if (p->getGoals().empty()) std::cout << "  No goals yet.\n";
                break;
            case 7:
                p->advanceMonth();
                std::cout << "\033[33m📅 Advanced to next month!\033[0m\n";
                break;
            case 8: gm.displaySummaryReport(); break;
            case 9: gm.saveGame(); break;
            case 0:
                std::cout << "\n\033[36mThanks for playing SimLife! 💸\033[0m\n\n";
                return;
        }
        waitForEnter();
    }
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main() {
    clearScreen();
    printHeader();

    GameManager gm;

    std::cout << "\033[1mWelcome to SimLife Finance!\033[0m\n\n";
    std::cout << "  \033[32m1.\033[0m New Game\n";
    std::cout << "  \033[32m2.\033[0m Load Game\n";
    std::cout << "  \033[31m0.\033[0m Exit\n";

    int choice = getIntInput("\n> ", 0, 2);

    if (choice == 0) return 0;

    if (choice == 2) {
        gm.loadGame();
        // After load, if no player fallback to new
    }

    if (!gm.getPlayer()) {
        Person* p = createCharacter();
        gm.setPlayer(p);
    }

    gameLoop(gm);

    std::cout << "Games played this session: " << GameManager::getTotalGamesPlayed() << "\n";
    return 0;
}
