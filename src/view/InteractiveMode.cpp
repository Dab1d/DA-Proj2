#include "view/InteractiveMode.h"
#include "view/Menu.h"
#include "view/ResultView.h"
#include "controllers/RegisterAllocatorController.h"

#include <iostream>
#include <string>

using std::cout;
using std::string;

static RegisterAllocatorController gCtrl;

void InteractiveMode::run() {
    while (true) {
        Menu::showMenu();
        int choice = Menu::getSafeInteger();
        if (choice == 0) break;

        switch (choice) {
            case 1:
                try {
                    gCtrl.loadRangesFromFile(promptFilename("select the number of the file if it are in this format:datasets/basic/rangesX.txt, otherwise write the full path of the file", "datasets/basic/ranges"));
                    cout << "Loaded " << gCtrl.getRangeCount() << " live range(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 2:
                try {
                    gCtrl.loadConfigFromFile(promptFilename("select the number of the file if it are in this format:datasets/basic/registersX.txt, otherwise write the full path of the file", "datasets/basic/registers"));
                    cout << "Config loaded: " << gCtrl.getNumRegisters() << " register(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 3:
                try {
                    gCtrl.build();
                    cout << "Built " << gCtrl.getWebCount() << " web(s) and interference graph.\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 4:
                try {
                    gCtrl.run();
                    cout << (gCtrl.isFeasible()
                        ? "Allocation successful.\n"
                        : "Allocation INFEASIBLE.\n");
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 5:
                printAllocationResult(gCtrl);
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
