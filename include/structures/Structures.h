#ifndef DA_PROJ2_STRUCTURES_H
#define DA_PROJ2_STRUCTURES_H

#include <string>
#include <vector>
#include <set>
#include <map>

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

// ---------------------------------------------------------------------------
// Web-build event types (used by the visualization callback)
// ---------------------------------------------------------------------------

enum class WebEventType {
    VAR_START,   ///< Starting to process a variable's live ranges
    MERGE,       ///< Two live ranges were merged (union-find step)
    WEB_FORMED,  ///< A final web was created
    EDGE_ADDED   ///< An interference edge was added to the graph
};

struct WebBuildEvent {
    WebEventType             type;
    std::string              varName;
    int                      webIdA     = -1;  ///< web id (WEB_FORMED) or first web (EDGE_ADDED)
    int                      webIdB     = -1;  ///< second web id (EDGE_ADDED)
    std::set<int>            linesA;           ///< line set of first range / web's liveLines
    std::set<int>            linesB;           ///< line set of second range (MERGE only)
    int                      rangeCount = 0;   ///< number of ranges for this variable (VAR_START)
    std::vector<ProgramPoint> pts;             ///< sorted program points with +/- markers (WEB_FORMED)
};

// ---------------------------------------------------------------------------
// Allocation event types (used by the allocation animation callback)
// ---------------------------------------------------------------------------

enum class AllocEventType {
    ALGO_START,  ///< Algorithm name + K announced
    PHASE,       ///< Phase header ("Reduction" / "Coloring")
    WEB_PUSHED,  ///< Web pushed to stack (degree < K)
    WEB_SPILLED, ///< Web selected for forced spill
    WEB_COLORED, ///< Web assigned a physical register
    WEB_RESCUED, ///< Free alloc: local recolor freed a slot
    ATTEMPT,     ///< Spilling/splitting iteration start
};

struct AllocEvent {
    AllocEventType type;
    int         webId   = -1;  ///< affected web id
    int         reg     = -1;  ///< register assigned (-1 = spill)
    int         degree  = -1;  ///< active degree at decision time
    int         attempt = -1;  ///< iteration number (ATTEMPT)
    std::string varName;       ///< variable name of the web
    std::string label;         ///< algo name (ALGO_START) or phase name (PHASE)
};

#endif // DA_PROJ2_STRUCTURES_H