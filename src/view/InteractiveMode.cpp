#include "view/InteractiveMode.h"
#include "view/Menu.h"
#include "view/ResultView.h"
#include "controllers/LoadRangesController.h"
#include "controllers/RegisterAllocatorController.h"
#include "utils/PathUtils.h"

#include <exception>
#include <iostream>
#include <filesystem>
#include <limits>
#include <string>

using std::cout;
using std::string;

namespace {

LoadRangesController gLoader;
RegisterAllocatorController gAlloc;
string gRangesFile;
string gConfigFile;
string gOutputFile = "allocation.txt";

string askLine(const string& prompt) {
    cout << prompt;
    string value;
    std::getline(std::cin, value);
    return value;
}

string algorithmName(AlgorithmType algorithm) {
    switch (algorithm) {
        case AlgorithmType::BASIC: return "basic";
        case AlgorithmType::SPILLING: return "spilling";
        case AlgorithmType::SPLITTING: return "splitting";
        case AlgorithmType::FREE: return "free";
    }
    return "unknown";
}

bool requiresNumericParameter(AlgorithmType algorithm) {
    return algorithm == AlgorithmType::SPILLING || algorithm == AlgorithmType::SPLITTING;
}

int askPositiveInteger(const string& prompt) {
    while (true) {
        string input = askLine(prompt);
        try {
            int value = std::stoi(input);
            if (value > 0) return value;
        } catch (...) {
        }
        cout << "  invalid input, choose a positive number.\n";
    }
}

void ensureAlgorithmParameter(RegisterConfig& config) {
    if (!requiresNumericParameter(config.algorithm)) return;
    if (config.algorithmParam > 0) return;

    config.algorithmParam = askPositiveInteger(
        "  " + algorithmName(config.algorithm) + " numeric parameter: "
    );
}

string formatPoint(const ProgramPoint& pp) {
    string s = std::to_string(pp.line);
    if (pp.isDefinition) s += '+';
    else if (pp.isLastUse) s += '-';
    return s;
}

string formatPoints(const std::vector<ProgramPoint>& points) {
    string out;
    for (size_t i = 0; i < points.size(); ++i) {
        if (i) out += ',';
        out += formatPoint(points[i]);
    }
    return out;
}

string displayOutputTarget() {
    namespace fs = std::filesystem;

    fs::path output(gOutputFile);
    if (output.is_absolute()) return output.string();

    fs::path root = findProjectRoot();
    if (output.has_parent_path())
        return (root.empty() ? output : root / output).string();

    fs::path base = root.empty() ? fs::path("datasets/basic/output") : root / "datasets/basic/output";
    return (base / output).string();
}

void printLoadedState() {
    cout << "\nLoaded state:\n";
    cout << "  ranges : " << (gLoader.rangesReady ? gRangesFile : "none") << "\n";
    cout << "  config : " << (gLoader.configReady ? gConfigFile : "none") << "\n";
    cout << "  webs   : " << (gAlloc.isLoaded() ? std::to_string(gAlloc.getWebCount()) : "not built") << "\n";
    cout << "  result : " << (gAlloc.isAllocated() ? "done" : "not run") << "\n";
    cout << "  output : " << displayOutputTarget() << "\n\n";
}

void doLoadRanges() {
    string input = askLine("  ranges filename (ex. ranges1.txt or 1): ");
    if (input.empty()) { cout << "  cancelled.\n"; return; }

    try {
        string resolved = resolveFilePath(input, "datasets/basic/ranges");
        gLoader.loadRangesFromFile(resolved);
        gRangesFile = resolved;
        gAlloc = RegisterAllocatorController();
        cout << "  OK - loaded " << gLoader.getRangeCount() << " live range(s) from " << resolved << "\n";
    } catch (const std::exception& e) {
        cout << "  ERROR: " << e.what() << "\n";
    }
}

void doLoadConfig() {
    string input = askLine("  config filename (ex. registers1.txt or 1): ");
    if (input.empty()) { cout << "  cancelled.\n"; return; }

    try {
        string resolved = resolveFilePath(input, "datasets/basic/registers");
        gLoader.loadConfigFromFile(resolved);
        ensureAlgorithmParameter(gLoader.config);
        gConfigFile = resolved;
        gAlloc = RegisterAllocatorController();
        cout << "  OK - registers=" << gLoader.getConfig().numRegisters
             << "  algorithm=" << algorithmName(gLoader.getConfig().algorithm);
        if (requiresNumericParameter(gLoader.getConfig().algorithm)) {
            cout << "  parameter=" << gLoader.getConfig().algorithmParam;
        }
        cout << "\n";
    } catch (const std::exception& e) {
        cout << "  ERROR: " << e.what() << "\n";
    }
}

void doBuildWebs() {
    if (!gLoader.rangesReady) { cout << "  Load a ranges file first (option 1).\n"; return; }
    if (!gLoader.configReady) { cout << "  Load a config file first (option 2).\n"; return; }

    try {
        gAlloc.build(gLoader.getLiveRanges(), gLoader.getConfig());
        cout << "  OK - built " << gAlloc.getWebCount() << " web(s):\n";
        for (const auto& w : gAlloc.webs)
            cout << "    web" << w.id << " [" << w.varName << "]: " << formatPoints(w.points) << "\n";
    } catch (const std::exception& e) {
        cout << "  ERROR: " << e.what() << "\n";
    }
}

void doShowGraph() {
    if (!gAlloc.isLoaded()) { cout << "  Build webs first (option 3).\n"; return; }

    cout << "  Interference graph: " << gAlloc.interferenceGraph.getNumVertex() << " node(s)\n";
    bool any = false;
    for (auto* v : gAlloc.interferenceGraph.getVertexSet()) {
        for (const auto& e : v->getAdj()) {
            if (v->getInfo() < e.getDest()->getInfo()) {
                cout << "    web" << v->getInfo() << " -- web" << e.getDest()->getInfo() << "\n";
                any = true;
            }
        }
    }
    if (!any) cout << "    (no edges - no interferences)\n";
}

void doAllocate() {
    if (!gAlloc.isLoaded()) { cout << "  Build webs first (option 3).\n"; return; }

    try {
        gAlloc.run();
        if (gAlloc.isFeasible()) {
            cout << "  Allocation SUCCESSFUL.\n";
        } else {
            cout << "  Allocation INFEASIBLE.\n";
        }
        printAllocationSummary(gAlloc.getResult());
    } catch (const std::exception& e) {
        cout << "  ERROR: " << e.what() << "\n";
    }
}

void doShowResult() {
    printAllocationResult(gAlloc);
}

void doWriteResult() {
    if (!gAlloc.isAllocated()) { cout << "  Run allocation first (option 5).\n"; return; }

    try {
        gAlloc.writeOutput(gOutputFile);
        cout << "  Written to: " << displayOutputTarget() << "\n";
    } catch (const std::exception& e) {
        cout << "  ERROR: " << e.what() << "\n";
    }
}

void doSetOutput() {
    string input = askLine("  new output filename: ");
    if (input.empty()) { cout << "  cancelled.\n"; return; }
    if (std::filesystem::path(input).extension().empty()) input += ".txt";
    gOutputFile = input;
    cout << "  output file set to: " << gOutputFile << "\n";
}

void doEndToEnd() {
    cout << "\n  --- End-to-end run ---\n\n";
    doLoadRanges();
    if (!gLoader.rangesReady) return;
    doLoadConfig();
    if (!gLoader.configReady) return;

    string output = askLine("  output filename [" + displayOutputTarget() + "]: ");
    if (!output.empty()) {
        if (std::filesystem::path(output).extension().empty()) output += ".txt";
        gOutputFile = output;
        cout << "  output file set to: " << displayOutputTarget() << "\n";
    }

    doBuildWebs();
    if (!gAlloc.isLoaded()) return;
    doAllocate();
    if (!gAlloc.isAllocated()) return;
    doWriteResult();
    cout << "\n --- Done ---\n";
}

} // namespace

void InteractiveMode::run() {
    while (true) {
        Menu::showMenu();
        printLoadedState();
        int choice = Menu::getSafeInteger();

        bool pauseBeforeMenu = true;
        switch (choice) {
            case 1: doLoadRanges(); break;
            case 2: doLoadConfig(); break;
            case 3: doBuildWebs(); break;
            case 4: doShowGraph(); break;
            case 5: doAllocate(); break;
            case 6: doShowResult(); break;
            case 7: doSetOutput(); break;
            case 8: doWriteResult(); break;
            case 9:
                doEndToEnd();
                break;
            case 0:
                cout << "  Goodbye.\n";
                return;
            default:
                cout << "  Unknown option. Please enter 0-9.\n";
                break;
        }

        if (pauseBeforeMenu) Menu::waitForReturnToMenu();
    }
}
