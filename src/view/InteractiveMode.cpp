#include "view/InteractiveMode.h"
#include "view/Menu.h"
#include "view/ResultView.h"
#include "view/WebVisualizer.h"
#include "view/AllocationVisualizer.h"
#include "controllers/LoadRangesController.h"
#include "controllers/RegisterAllocatorController.h"

#include <iostream>
#include <string>

using std::cout;
using std::string;

static LoadRangesController        gLoader;
static RegisterAllocatorController gAlloc;

void InteractiveMode::run() {
    while (true) {
        Menu::showMenu();
        int choice = Menu::getSafeInteger();
        if (choice == 0) break;

        switch (choice) {
            case 1:
                try {
                    gLoader.loadRangesFromFile(promptFilename("Enter filename or its number (eg. ranges1 or 1)", "datasets/basic/ranges"));
                    cout << "Loaded " << gLoader.getRangeCount() << " live range(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 2:
                try {
                    gLoader.loadConfigFromFile(promptFilename("Enter filename or its number (eg. registers1 or 1)", "datasets/basic/registers"));
                    cout << "Config loaded: " << gLoader.getNumRegisters() << " register(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 3:
                try {
                    if (!gLoader.isReady())
                        throw std::runtime_error("Load ranges and config first (options 1 and 2)");
                    buildWithVisualization(gAlloc, gLoader.getLiveRanges(), gLoader.getConfig());
                    cout << "Built " << gAlloc.getWebCount() << " web(s) and interference graph.\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 4:
                try {
                    runWithVisualization(gAlloc);
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 5:
                printAllocationResult(gAlloc);
                break;

            case 6: {
                string outFile = promptFilename("Output file");
                try {
                    gAlloc.writeOutput(outFile);
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
