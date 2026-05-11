//
// Created by conduto on 4/26/26.
//

#include "parser/parserTools.h"
using std::string;

string ParserTools::trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}