//
// Created by conduto on 4/26/26.
//

#include "parser/Parser.h"
#include "parser/parserTools.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>


using std::vector;
using std::string;
using std::to_string;
using std::ifstream;
using std::stringstream;
using std::invalid_argument;
using std::runtime_error;


static ProgramPoint parsePoint(const string& raw) {
    string t = ParserTools::trim(raw);
    if (t.empty()) throw invalid_argument("Empty program point");

    ProgramPoint pp{0, false, false};

    if (t.back() == '+') {
        pp.isDefinition = true;
        t.pop_back();
    } 
    else if (t.back() == '-') {
        pp.isLastUse = true;
        t.pop_back();
    }

    try {
        pp.line = stoi(t);
    } catch (...) {
        throw invalid_argument("Invalid line number: '" + raw + "'");
    }

    if (pp.line < 0)
        throw invalid_argument("Negative line number: " + to_string(pp.line));

    return pp;
}

vector<LiveRange> Parser::parseLiveRanges(const string& filename) {
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Cannot open file: " + filename);

    vector<LiveRange> ranges;
    string line;
    int lineNum = 0;

    while (getline(file, line)) {
        lineNum++;
        line = ParserTools::trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colon = line.find(':');
        if (colon == string::npos)
            throw invalid_argument("Line " + to_string(lineNum) + ": missing ':'");

        string varName = ParserTools::trim(line.substr(0, colon));
        string pointsStr = ParserTools::trim(line.substr(colon + 1));

        if (varName.empty())
            throw invalid_argument("Line " + to_string(lineNum) + ": empty variable name");
        if (pointsStr.empty())
            throw invalid_argument("Line " + to_string(lineNum) + ": no program points");

        LiveRange lr;
        lr.varName = varName;

        stringstream ss(pointsStr);
        string token;
        while (getline(ss, token, ',')) {
            token = ParserTools::trim(token);
            if (token.empty()) continue;
            lr.points.push_back(parsePoint(token));
        }

        if (lr.points.empty())
            throw invalid_argument("Line " + to_string(lineNum) + ": no valid points");

        ranges.push_back(lr);
    }

    if (ranges.empty())
        throw invalid_argument("No live ranges found in: " + filename);

    return ranges;
}

RegisterConfig Parser::parseRegisterConfig(const string& filename) {
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Cannot open file: " + filename);

    RegisterConfig config{-1, AlgorithmType::BASIC, -1};
    bool foundReg = false, foundAlg = false;
    string line;

    while (getline(file, line)) {
        line = ParserTools::trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colon = line.find(':');
        if (colon == string::npos) continue;

        string key = ParserTools::trim(line.substr(0, colon));
        string val = ParserTools::trim(line.substr(colon + 1));

        transform(key.begin(), key.end(), key.begin(), ::tolower);

        if (key == "registers") {
            config.numRegisters = stoi(val);
            if (config.numRegisters < 1)
                throw invalid_argument("Registers must be >= 1");
            foundReg = true;

        } else if (key == "algorithm") {
            size_t comma = val.find(',');
            string algName, paramStr;

            if (comma != string::npos) {
                algName = ParserTools::trim(val.substr(0, comma));
                paramStr = ParserTools::trim(val.substr(comma + 1));
            } else {
                algName = ParserTools::trim(val);
            }

            transform(algName.begin(), algName.end(), algName.begin(), ::tolower);

            if (algName == "basic") {
                config.algorithm = AlgorithmType::BASIC;
            } else if (algName == "spilling") {
                config.algorithm = AlgorithmType::SPILLING;
                if (!paramStr.empty()) config.algorithmParam = stoi(paramStr);
            } else if (algName == "splitting") {
                config.algorithm = AlgorithmType::SPLITTING;
                if (!paramStr.empty()) config.algorithmParam = stoi(paramStr);
            } else if (algName == "free") {
                config.algorithm = AlgorithmType::FREE;
            } else {
                throw invalid_argument("Unknown algorithm: '" + algName + "'");
            }
            foundAlg = true;
        }
    }

    if (!foundReg) throw invalid_argument("Missing 'registers:' in: " + filename);
    if (!foundAlg) throw invalid_argument("Missing 'algorithm:' in: " + filename);

    return config;
}