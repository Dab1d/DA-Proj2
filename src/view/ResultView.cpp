#include "view/ResultView.h"
#include "utils/PathUtils.h"
#include "structures/Structures.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

using std::cout;
using std::cin;
using std::string;
using std::vector;

static string fmtPoint(const ProgramPoint& pp) {
    string s = std::to_string(pp.line);
    if (pp.isDefinition) s += '+';
    else if (pp.isLastUse) s += '-';
    return s;
}

static string fmtPoints(const vector<ProgramPoint>& pts) {
    string s;
    for (size_t i = 0; i < pts.size(); i++) {
        if (i) s += ',';
        s += fmtPoint(pts[i]);
    }
    return s;
}

string promptFilename(const string& label, const char* preferredSub) {
    cout << label << ": ";
    string path;
    cin >> path;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return resolveFilePath(path, preferredSub);
}

void printAllocationResult(const RegisterAllocatorController& ctrl) {
    if (!ctrl.isAllocated()) { cout << "No allocation has been run yet.\n"; return; }

    const AllocationResult& res = ctrl.getResult();
    cout << "\n=== Allocation Result ===\n";
    cout << "webs: " << res.webs.size() << "\n";
    for (const auto& w : res.webs)
        cout << "  web" << w.id << " [" << w.varName << "]: " << fmtPoints(w.points) << "\n";

    if (res.feasible) {
        cout << "registers used: " << res.registersUsed << "\n";
        std::map<int, vector<int>> regToWebs;
        for (const auto& [wid, reg] : res.webToRegister)
            regToWebs[reg].push_back(wid);
        if (regToWebs.count(-1)) {
            auto& sp = regToWebs[-1];
            std::sort(sp.begin(), sp.end());
            for (int wid : sp) cout << "M: web" << wid << "\n";
        }
        for (auto& [reg, wids] : regToWebs) {
            if (reg < 0) continue;
            std::sort(wids.begin(), wids.end());
            for (int wid : wids) cout << "  r" << reg << " -> web" << wid << "\n";
        }
    } else {
        cout << "Allocation INFEASIBLE with " << ctrl.getNumRegisters() << " register(s).\n";
        for (const auto& w : res.webs) cout << "M: web" << w.id << "\n";
    }
    cout << "=========================\n";
}
