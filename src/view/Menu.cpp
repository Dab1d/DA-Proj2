//
// Created by conduto on 4/26/26.
//
#include "view/Menu.h"
#include <iostream>

using std::cout;
using std::cin;
using std::string;

void Menu::showMenu() {
    std::cout << "========================================\n";
    std::cout << "  DA Register Allocator - Spring 2026   \n";
    std::cout << "========================================\n";
    std::cout << "1. Load live ranges file                \n";
    std::cout << "2. Load register config file            \n";
    std::cout << "3. Build webs & interference graph      \n";
    std::cout << "4. Run register allocation              \n";
    std::cout << "5. Display results                      \n";
    std::cout << "6. Export output file                   \n";
    std::cout << "0. Exit                                 \n";
    std::cout << "========================================\n";
}

int Menu::getSafeInteger() {
    while (true) {
        string line;
        cout << "select an option:"<<"\n";
        getline(cin,line);

        try {
            return stoi(line);
        }
        catch (...) {
            cout<<"invalid input, chose a valid number!"<<"\n";
        }
    }
}
void Menu::waitForReturnToMenu() {
    std::string input;
    cout << "\nPress Enter to return to the menu: ";
    getline(cin, input);
}
