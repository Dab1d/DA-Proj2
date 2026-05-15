#include "view/BatchMode.h"
#include "parser/Parser.h"
#include "controllers/RegisterAllocatorController.h"
#include <iostream>
#include <string>

using std::string;

void BatchMode::run(const string& ranges, const string& registers, const string& allocation) {
    try {
        auto liveRanges = Parser::parseLiveRanges(ranges);
        auto config     = Parser::parseRegisterConfig(registers);

        RegisterAllocatorController ctrl;
        ctrl.build(liveRanges, config);
        ctrl.run();
        ctrl.writeOutput(allocation);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
