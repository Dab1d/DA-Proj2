#ifndef DA_PROJ2_STRUCTURES_H
#define DA_PROJ2_STRUCTURES_H

#include <string>
#include <vector>
#include <set>
#include <map>

/**
 * @file Structures.h
 * @brief Defines the core data structures used throughout the application.
 */

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
    std::string varName;              ///< Name of the variable
    std::vector<ProgramPoint> points; ///< Program points that make up the live range
};

/**
 * @struct RegisterConfig
 * @brief Configuration parsed from the register input file.
 */
struct RegisterConfig {
    int numRegisters;           ///< Number of available physical registers
    AlgorithmType algorithm;    ///< Selected register allocation algorithm variant
    int algorithmParam;         ///< Parameter (e.g. max splits/spills), -1 otherwise
};

/**
 * @struct Web
 * @brief A live web: the union of merged live ranges for a variable.
 */
struct Web {
    int id;                            ///< Unique sequential ID (web0, web1, ...)
    std::string varName;               ///< Variable this web belongs to
    std::vector<ProgramPoint> points;  ///< Program points of this range (sorted by line)
    std::set<int> liveLines;           ///< Set of all live line numbers (for interference checks)
};

/**
 * @struct AllocationResult
 * @brief Result of the register allocation algorithm.
 */
struct AllocationResult {
    bool feasible;                   ///< true if all webs could be assigned a register
    int registersUsed;               ///< Number of physical registers used (0 if infeasible)
    std::vector<Web> webs;           ///< Final list of webs after possible splitting
    std::map<int, int> webToRegister; ///< web.id → register index (-1 = spilled to memory)
};

#endif // DA_PROJ2_STRUCTURES_H
