#include "view/InteractiveMode.h"
#include "view/Menu.h"
#include "controllers/RegisterAllocatorController.h"

#include <iostream>
#include <string>
#include <limits>
#include <filesystem>

using std::cout;
using std::cin;
using std::string;

static RegisterAllocatorController gCtrl;

namespace fs = std::filesystem;

// Walk up from CWD to find the directory that contains a "datasets/" folder.
static fs::path findProjectRoot() {
    fs::path current = fs::current_path();
    while (true) {
        if (fs::exists(current / "datasets") && fs::is_directory(current / "datasets"))
            return current;
        fs::path parent = current.parent_path();
        if (parent == current) break;
        current = parent;
    }
    return {};
}

static bool isAllDigits(const string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

static string resolveFilePath(const string& input, const char* preferredSub = nullptr) {
    static const fs::path root = findProjectRoot();

    static const char* kAllSubDirs[] = {
        "datasets/basic/ranges",
        "datasets/basic/registers",
        "datasets/input",
        "",
    };

    fs::path p(input);
    std::vector<fs::path> names = { p };
    if (!p.has_extension()) {
        names.push_back(fs::path(input + ".txt"));
        names.push_back(fs::path("ranges"    + input + ".txt"));
        names.push_back(fs::path("registers" + input + ".txt"));
    }
    if (isAllDigits(input)) {
        names.push_back(fs::path("ranges"    + input + ".txt"));
        names.push_back(fs::path("registers" + input + ".txt"));
    }

    // Search preferred subdir first, then the rest
    std::vector<const char*> subdirs;
    if (preferredSub) subdirs.push_back(preferredSub);
    for (const char* s : kAllSubDirs)
        if (!preferredSub || std::string(s) != preferredSub)
            subdirs.push_back(s);

    for (const char* sub : subdirs) {
        for (const fs::path& name : names) {
            fs::path candidate = fs::path(sub) / name;
            if (fs::exists(candidate)) return candidate.string();
            if (!root.empty()) {
                fs::path abs = root / sub / name;
                if (fs::exists(abs)) return abs.string();
            }
        }
    }
    return input;
}

static string promptFilename(const string& label, const char* preferredSub = nullptr) {
    cout << label << ": ";
    string path;
    cin >> path;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return resolveFilePath(path, preferredSub);
}

void InteractiveMode::run() {
    while (true) {
        Menu::showMenu();
        int choice = Menu::getSafeInteger();
        if (choice == 0) break;

        switch (choice) {
            case 1:
                try {
                    gCtrl.loadRangesFromFile(promptFilename("Live ranges file", "datasets/basic/ranges"));
                    cout << "Loaded " << gCtrl.liveRanges.size() << " live range(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 2:
                try {
                    gCtrl.loadConfigFromFile(promptFilename("Register config file", "datasets/basic/registers"));
                    cout << "Config loaded: " << gCtrl.config.numRegisters << " register(s).\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 3:
                try {
                    gCtrl.build();
                    cout << "Built " << gCtrl.webs.size() << " web(s) and interference graph.\n";
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;

            case 4:
                try {
                    gCtrl.run();
                    cout << (gCtrl.result.feasible
                        ? "Allocation successful.\n"
                        : "Allocation INFEASIBLE.\n");
                } catch (const std::exception& e) {
                    cout << "Error: " << e.what() << "\n";
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
