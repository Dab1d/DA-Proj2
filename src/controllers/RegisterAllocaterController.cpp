#include "controllers/RegisterAllocatorController.h"
#include "algorithms/WebBuilder.h"
#include "algorithms/GraphColoring.h"
#include "utils/PathUtils.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <filesystem>

using std::string;
using std::vector;

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
// build
// ---------------------------------------------------------------------------

void RegisterAllocatorController::build(const vector<LiveRange>& ranges,
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
    if (!loaded) throw std::runtime_error("Call build() before run()");

    int K     = config.numRegisters;
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
}


static std::filesystem::path resolveOutputPath(const string& filename) {
    namespace fs = std::filesystem;

    fs::path requested(filename);
    if (requested.is_absolute()) return requested;

    fs::path root = findProjectRoot();
    if (requested.has_parent_path()) {
        return root.empty() ? requested : root / requested;
    }

    fs::path outputDir = root.empty()
        ? fs::path("datasets/basic/output")
        : root / "datasets/basic/output";

    fs::create_directories(outputDir);
    return outputDir / requested;
}

// ---------------------------------------------------------------------------
// writeOutput
// ---------------------------------------------------------------------------

void RegisterAllocatorController::writeOutput(const string& filename) const {
    if (!allocated) throw std::runtime_error("Call run() before writeOutput()");

    const auto outputPath = resolveOutputPath(filename);
    std::ofstream out(outputPath);
    if (!out.is_open())
        throw std::runtime_error("Cannot open output file: " + outputPath.string());

    const vector<Web>& ws = result.webs;

    out << "# Total number of webs followed by the listing of the program points of each one\n";
    out << "# program points in each web are sorted in ascending order\n";
    out << "webs: " << ws.size() << "\n";
    for (const auto& w : ws)
        out << "web" << w.id << ": " << formatPoints(w.points) << "\n";

    out << "# Total number of registers used, followed by assignment to webs\n";
    out << "registers: " << result.registersUsed << "\n";

    if (result.feasible) {
        std::map<int, vector<int>> regToWebs;
        for (const auto& [wid, reg] : result.webToRegister)
            regToWebs[reg].push_back(wid);

        if (regToWebs.count(-1)) {
            auto& spilled = regToWebs[-1];
            std::sort(spilled.begin(), spilled.end());
            for (int wid : spilled) out << "M: web" << wid << "\n";
        }
        for (auto& [reg, wids] : regToWebs) {
            if (reg < 0) continue;
            std::sort(wids.begin(), wids.end());
            for (int wid : wids) out << "r" << reg << ": web" << wid << "\n";
        }
    } else {
        for (const auto& w : ws) out << "M: web" << w.id << "\n";
    }

    out.close();
}
