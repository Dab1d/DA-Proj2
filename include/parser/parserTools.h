//
// Created by conduto on 4/26/26.
//

#ifndef DA_PROJ2_PARSERTOOLS_H
#define DA_PROJ2_PARSERTOOLS_H

#include <string>
using std::string; 
/**
 * @namespace ParserTools
 * @brief Utility functions for parsing.
 */
namespace ParserTools {
    /**
     * @brief Trims leading and trailing whitespace.
     * @param s Input string.
     * @return Trimmed string.
     * @note Time complexity: O(n) where n is the string length.
     */
    string trim(const string& s);
}
#endif //DA_PROJ2_PARSERTOOLS_H