#include "view/BatchMode.h"
#include "parser/Parser.h"
#include "controllers/RegisterAllocatorController.h"
#include "utils/PathUtils.h"

#include <iostream>
#include <stdexcept>
#include <string>

using std::string;

static bool requiresParameter(AlgorithmType algorithm) {
    return algorithm == AlgorithmType::SPILLING || algorithm == AlgorithmType::SPLITTING;
}

bool BatchMode::run(const string& ranges, const string& registers, const string& allocation) {
    try {
        const string rangesPath = resolveFilePath(ranges, "datasets/basic/ranges");
        const string registersPath = resolveFilePath(registers, "datasets/basic/registers");

        auto liveRanges = Parser::parseLiveRanges(rangesPath);
        auto config = Parser::parseRegisterConfig(registersPath);

        if (requiresParameter(config.algorithm) && config.algorithmParam <= 0) {
            throw std::invalid_argument("spilling/splitting requires a positive numeric parameter in batch mode");
        }

        RegisterAllocatorController ctrl;
        ctrl.build(liveRanges, config);
        ctrl.run();
        if (!ctrl.isFeasible()) {
            std::cerr << "Warning: The assignment to the provided number of registers was not possible." << std::endl;
        }
        ctrl.writeOutput(allocation);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return false;
    }
}
