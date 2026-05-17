//
// Created by conduto on 4/26/26.
//
#include "view/Menu.h"
#include <iostream>

using std::cout;
using std::cin;
using std::string;

void Menu::showMenu() {
    std::cout << "\n============================================\n";
    std::cout << "   Compiler Register Allocation Tool\n";
    std::cout << "============================================\n";
    std::cout << " 1) Load live ranges file\n";
    std::cout << " 2) Load register config file\n";
    std::cout << " 3) Build webs (merge live ranges)\n";
    std::cout << " 4) Show interference graph (edge list)\n";
    std::cout << " 5) Run register allocation\n";
    std::cout << " 6) Show final result on screen\n";
    std::cout << " 7) Set output filename\n";
    std::cout << " 8) Write final result to output file\n";
    std::cout << " 9) Run end-to-end (load -> build -> allocate -> write)\n";
    std::cout << " 0) Exit\n";
    std::cout << "============================================\n";
}

int Menu::getSafeInteger() {
    while (true) {
        string line;
        cout << "select an option:";
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
