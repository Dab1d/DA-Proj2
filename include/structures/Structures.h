#ifndef DA_PROJ2_STRUCTURES_H
#define DA_PROJ2_STRUCTURES_H

#include <string>
#include <vector>

/**
 * @enum AlgorithmType
 * @brief Available register allocation algorithm variants.
 */
enum class AlgorithmType {
    BASIC,      ///< Simple greedy graph coloring
    SPILLING,   ///< Graph coloring with web spilling
    SPLITTING,  ///< Graph coloring with web splitting
    FREE        ///< Custom algorithm
};

/**
 * @struct ProgramPoint
 * @brief A single program point in a live range.
 */
struct ProgramPoint {
    int line;           ///< Program line number
    bool isDefinition;  ///< true if marked with '+'
    bool isLastUse;     ///< true if marked with '-'
};

/**
 * @struct LiveRange
 * @brief A single live range entry for a variable.
 */
struct LiveRange {
    std::string varName;
    std::vector<ProgramPoint> points;
};

/**
 * @struct RegisterConfig
 * @brief Configuration parsed from the register input file.
 */
struct RegisterConfig {
    int numRegisters;
    AlgorithmType algorithm;
    int algorithmParam;  ///< K for spilling/splitting, -1 otherwise
};

#endif // DA_PROJ2_STRUCTURES_H