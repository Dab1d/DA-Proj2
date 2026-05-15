#include "view/BatchMode.h"
#include "parser/Parser.h"
#include <iostream>
#include <string>

using std::string;

void BatchMode::run(const string& ranges,const string& registers,const string& allocation) {
    try {
        // 1. Parse inputs
        // auto liveRanges = Parser::parseLiveRanges(ranges);
        // auto config = Parser::parseRegisterConfig(registers);
        //
        // // 2. Build webs & interference graph
        // //RegisterAllocator allocator;
        // auto webs = allocator.buildWebs(liveRanges);
        // auto graph = allocator.buildInterferenceGraph(webs);
        //
        // // 3. Run selected algorithm
        // auto result = allocator.allocate(liveRanges, config);
        //
        // // 4. Write output
        // allocator.writeOutput(allocation, result);
        //
        // if (!result.feasible) {
        //     std::cerr << "Warning: allocation with "
        //               << config.numRegisters
        //               << " registers was not possible." << std::endl;
        // }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}