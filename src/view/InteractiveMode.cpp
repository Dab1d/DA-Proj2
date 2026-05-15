#include "view/InteractiveMode.h"
#include "view/Menu.h"
#include "parser/Parser.h"
#include "controllers/RegisterAllocatorController.h"

#include <iostream>
#include <string>
#include <limits>

using std::cout;
using std::cin;
using std::string;

static RegisterAllocatorController gCtrl;

static string promptFilename(const string& label) {
    cout << label << ": ";
    string path;
    cin >> path;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return path;
}

void InteractiveMode::run() {
    string rangesFile, registersFile;
    std::vector<LiveRange> liveRanges;
    RegisterConfig config;
    bool rangesLoaded = false, configLoaded = false;

    while (true) {
        Menu::showMenu();
        int choice = Menu::getSafeInteger();
        if (choice == 0) break;

        switch (choice) {
            case 1:
                rangesFile = promptFilename("Live ranges file");
                try {
                    liveRanges = Parser::parseLiveRanges(rangesFile);
                    rangesLoaded = true;
                    cout << "Loaded " << liveRanges.size() << " live range(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 2:
                registersFile = promptFilename("Register config file");
                try {
                    config = Parser::parseRegisterConfig(registersFile);
                    configLoaded = true;
                    cout << "Config loaded: " << config.numRegisters << " register(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 3:
                if (!rangesLoaded || !configLoaded) {
                    cout << "Load ranges (1) and config (2) first.\n";
                } else {
                    try {
                        gCtrl.load(liveRanges, config);
                        cout << "Built " << gCtrl.webs.size() << " web(s) and interference graph.\n";
                    } catch (const std::exception& e) {
                        cout << "Error: " << e.what() << "\n";
                    }
                }
                break;

            case 4:
                if (!gCtrl.loaded) {
                    cout << "Build webs first (option 3).\n";
                } else {
                    try {
                        gCtrl.run();
                        cout << (gCtrl.result.feasible
                            ? "Allocation successful.\n"
                            : "Allocation INFEASIBLE.\n");
                    } catch (const std::exception& e) {
                        cout << "Error: " << e.what() << "\n";
                    }
                }
                break;

            case 5:
                gCtrl.printResult();
                break;

            case 6: {
                string outFile = promptFilename("Output file");
                try {
                    gCtrl.writeOutput(outFile);
                    cout << "Result written to " << outFile << "\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }

            default:
                cout << "Invalid option.\n";
                break;
        }
        Menu::waitForReturnToMenu();
    }
    cout << "Exiting...\n";
}
