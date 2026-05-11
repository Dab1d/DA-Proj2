//
// Created by conduto on 5/11/26.
//
#include <iostream>

#include "../../include/ui/InteractiveMode.h"
#include "ui/Menu.h"

using std::cout;

void InteractiveMode::run() {

    while (true) {
        Menu::showMenu();
        int choice = Menu::getSafeInteger();
        if (choice == 0) break;

        switch (choice) {
            case 1: cout << "TODO: Load ranges\n"; break;
            case 2: cout << "TODO: Load config\n"; break;
            case 3: cout << "TODO: Build webs\n"; break;
            case 4: cout << "TODO: Run allocation\n"; break;
            case 5: cout << "TODO: Display results\n"; break;
            case 6: cout << "TODO: Export output\n"; break;
            default: cout << "Invalid option.\n"; break;
        }
        Menu::waitForReturnToMenu();
    }
    cout <<"Exiting...\n";
}
