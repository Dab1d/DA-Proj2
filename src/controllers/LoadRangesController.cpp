#include "controllers/LoadRangesController.h"
#include "parser/Parser.h"

using std::string;

void LoadRangesController::loadRangesFromFile(const string& filename) {
    liveRanges = Parser::parseLiveRanges(filename);
    rangesReady = true;
}

void LoadRangesController::loadConfigFromFile(const string& filename) {
    config = Parser::parseRegisterConfig(filename);
    configReady = true;
}
