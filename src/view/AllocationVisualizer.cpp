#include "view/AllocationVisualizer.h"
#include "algorithms/GraphColoring.h"
#include "algorithms/GraphColoringHelpers.h"
#include <iostream>
#include <thread>
#include <chrono>

using std::cout;

static const char* CYAN    = "\033[1;36m";
static const char* GREEN   = "\033[1;32m";
static const char* RED     = "\033[1;31m";
static const char* YELLOW  = "\033[1;33m";
static const char* DIM     = "\033[2;37m";
static const char* WHITE   = "\033[0;37m";
static const char* RESET   = "\033[0m";

static void pause() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

static void handleEvent(const AllocEvent& e) {
    switch (e.type) {
        case AllocEventType::ALGO_START:
            cout << "\n" << CYAN << "Running allocation: " << e.label
                 << RESET << "\n\n";
            pause();
            break;

        case AllocEventType::PHASE:
            cout << "\n" << CYAN << "  ── " << e.label << " phase ──"
                 << RESET << "\n";
            pause();
            break;

        case AllocEventType::ATTEMPT:
            cout << "\n" << WHITE << "  [Attempt " << e.attempt << "]"
                 << RESET << "\n";
            pause();
            break;

        case AllocEventType::WEB_PUSHED:
            cout << DIM << "    web" << e.webId << " (" << e.varName << ")"
                 << "  pushed"
                 << (e.degree >= 0 ? "     [degree " + std::to_string(e.degree) + "]" : "")
                 << RESET << "\n";
            pause();
            break;

        case AllocEventType::WEB_SPILLED:
            cout << RED << "    web" << e.webId << " (" << e.varName << ")"
                 << "  SPILLED"
                 << (e.degree >= 0 ? "    [degree " + std::to_string(e.degree) + "]" : "")
                 << RESET << "\n";
            pause();
            break;

        case AllocEventType::WEB_COLORED:
            cout << GREEN << "    web" << e.webId << " (" << e.varName << ")"
                 << "  →  r" << e.reg
                 << RESET << "\n";
            pause();
            break;

        case AllocEventType::WEB_RESCUED:
            cout << YELLOW << "    web" << e.webId << " (" << e.varName << ")"
                 << "  →  r" << e.reg << "  (rescued)"
                 << RESET << "\n";
            pause();
            break;
    }
}

void runWithVisualization(RegisterAllocatorController& alloc) {
    if (!alloc.isLoaded())
        throw std::runtime_error("Build webs first (option 3)");

    AllocCb cb = handleEvent;
    const auto& g    = alloc.interferenceGraph;
    auto&       webs = alloc.webs;
    int         K    = alloc.config.numRegisters;

    AllocationResult res;
    switch (alloc.config.algorithm) {
        case AlgorithmType::BASIC:
            res = GraphColoring::basicAllocation(g, webs, K, cb);
            break;
        case AlgorithmType::SPILLING:
            res = GraphColoring::spillingAllocation(g, webs, K,
                      alloc.config.algorithmParam, cb);
            break;
        case AlgorithmType::SPLITTING:
            res = GraphColoring::splittingAllocation(g, webs, K,
                      alloc.config.algorithmParam, cb);
            break;
        case AlgorithmType::FREE:
            res = GraphColoring::freeAllocation(g, webs, K, cb);
            break;
    }

    alloc.result    = res;
    alloc.allocated = true;

    cout << "\n";
    if (res.feasible)
        cout << GREEN << "Allocation FEASIBLE — "
             << res.registersUsed << " register(s) used." << RESET << "\n";
    else
        cout << RED << "Allocation INFEASIBLE — spilled web(s) present." << RESET << "\n";
    cout << "\n";
}
