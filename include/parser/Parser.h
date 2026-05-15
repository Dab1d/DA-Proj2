//
// Created by conduto on 5/11/26.
//

#ifndef DA_PROJ2_PARSER_H
#define DA_PROJ2_PARSER_H

#include <string>
#include <vector>
#include <Structures.h>

/**
 * @class Parser
 * @brief Reads and parses live ranges and register configuration files.
 */
class Parser {
public:
    /**
     * @brief Parses the live ranges input file.
     * @param filename Path to the live ranges file.
     * @return Vector of parsed live ranges.
     * @throws std::runtime_error if file cannot be opened.
     * @throws std::invalid_argument if format is invalid.
     * @note Time complexity: O(L) where L is total lines in the file.
     */
    static std::vector<LiveRange> parseLiveRanges(const std::string& filename);

    /**
     * @brief Parses the register configuration file.
     * @param filename Path to the register config file.
     * @return Parsed register configuration.
     * @throws std::runtime_error if file cannot be opened.
     * @throws std::invalid_argument if format is invalid.
     * @note Time complexity: O(1).
     */
    static RegisterConfig parseRegisterConfig(const std::string& filename);
};
#endif //DA_PROJ2_PARSER_H
