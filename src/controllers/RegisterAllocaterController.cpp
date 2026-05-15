#include "controllers/RegisterAllocatorController.h"
#include "algorithms/WebBuilder.h"
#include "algorithms/GraphColoring.h"
#include "parser/Parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <map>

using std::string;
using std::vector;
using std::cout;
using std::cerr;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static string formatPoint(const ProgramPoint& pp) {
    string s = std::to_string(pp.line);
    if (pp.isDefinition) s += '+';
    else if (pp.isLastUse) s += '-';
    return s;
}

static string formatPoints(const vector<ProgramPoint>& pts) {
    string s;
    for (size_t i = 0; i < pts.size(); i++) {
        if (i) s += ',';
        s += formatPoint(pts[i]);
    }
    return s;
}

// ---------------------------------------------------------------------------
// loadRangesFromFile / loadConfigFromFile / build
// ---------------------------------------------------------------------------

void RegisterAllocatorController::loadRangesFromFile(const string& filename) {
    liveRanges = Parser::parseLiveRanges(filename);
    rangesReady = true;
}

void RegisterAllocatorController::loadConfigFromFile(const string& filename) {
    config = Parser::parseRegisterConfig(filename);
    configReady = true;
}

void RegisterAllocatorController::build() {
    if (!rangesReady || !configReady)
        throw std::runtime_error("Load ranges and config first (options 1 and 2)");
    load(liveRanges, config);
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------

void RegisterAllocatorController::load(const vector<LiveRange>& ranges,
                                       const RegisterConfig& cfg) {
    config = cfg;
    webs.clear();
    allocated = false;

    interferenceGraph = Graph<int>();

    webs = WebBuilder::buildWebs(ranges);
    WebBuilder::buildInterferenceGraph(webs, interferenceGraph);

    loaded = true;
}

// ---------------------------------------------------------------------------
// run
// ---------------------------------------------------------------------------

void RegisterAllocatorController::run() {
    if (!loaded) throw std::runtime_error("Call load() before run()");

    int K = config.numRegisters;
    int param = config.algorithmParam;

    switch (config.algorithm) {
        case AlgorithmType::BASIC:
            result = GraphColoring::basicAllocation(interferenceGraph, webs, K);
            break;
        case AlgorithmType::SPILLING:
            result = GraphColoring::spillingAllocation(interferenceGraph, webs, K, param);
            break;
        case AlgorithmType::SPLITTING:
            result = GraphColoring::splittingAllocation(interferenceGraph, webs, K, param);
            break;
        case AlgorithmType::FREE:
            result = GraphColoring::freeAllocation(interferenceGraph, webs, K);
            break;
    }

    allocated = true;

    if (!result.feasible)
        cerr << "Warning: allocation with " << K << " registers was not possible.\n";
}

// ---------------------------------------------------------------------------
// writeOutput
// ---------------------------------------------------------------------------

void RegisterAllocatorController::writeOutput(const string& filename) const {
    if (!allocated) throw std::runtime_error("Call run() before writeOutput()");

    std::ofstream out(filename);
    if (!out.is_open())
        throw std::runtime_error("Cannot open output file: " + filename);

    const vector<Web>& ws = result.webs;

    out << "# Total number of webs followed by the listing of the program points of each one\n";
    out << "# program points in each web are sorted in ascending order\n";
    out << "webs: " << ws.size() << "\n";
    for (const auto& w : ws)
        out << "web" << w.id << ": " << formatPoints(w.points) << "\n";

    out << "# Total number of registers used, followed by assignment to webs\n";
    out << "registers: " << result.registersUsed << "\n";

    if (result.feasible) {
        // Group by register; reg=-1 means spilled to memory (allowed by spilling algorithm)
        std::map<int, vector<int>> regToWebs;
        for (const auto& [wid, reg] : result.webToRegister)
            regToWebs[reg].push_back(wid);

        // Output memory-spilled webs first, then register-allocated ones
        if (regToWebs.count(-1)) {
            auto& spilled = regToWebs[-1];
            std::sort(spilled.begin(), spilled.end());
            for (int wid : spilled)
                out << "M: web" << wid << "\n";
        }
        for (auto& [reg, wids] : regToWebs) {
            if (reg < 0) continue;
            std::sort(wids.begin(), wids.end());
            for (int wid : wids)
                out << "r" << reg << ": web" << wid << "\n";
        }
    } else {
        for (const auto& w : ws)
            out << "M: web" << w.id << "\n";
    }

    out.close();
}

