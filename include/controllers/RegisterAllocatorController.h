#ifndef DA_PROJ2_REGISTERALLOCATORCONTROLLER_H
#define DA_PROJ2_REGISTERALLOCATORCONTROLLER_H

#include "structures/Structures.h"
#include "structures/Graph.h"
#include <vector>
#include <string>

/**
 * @class RegisterAllocatorController
 * @brief Orchestrates the full register allocation pipeline.
 *
 * Holds the parsed data, builds webs and the interference graph,
 * runs the selected coloring algorithm, and outputs the result.
 */
class RegisterAllocatorController {
public:
    std::vector<Web>  webs;
    Graph<int>        interferenceGraph;
    RegisterConfig    config;
    AllocationResult  result;
    bool              loaded = false;
    bool              allocated = false;

    /**
     * @brief Loads live ranges and register config, then builds webs and the interference graph.
     * @param ranges Parsed live ranges.
     * @param cfg    Parsed register configuration.
     * @note Time complexity: O(R^2 * P + W^2) where R = ranges, P = points, W = webs.
     */
    void load(const std::vector<LiveRange>& ranges, const RegisterConfig& cfg);

    /**
     * @brief Runs the allocation algorithm selected in config.
     * @note Must call load() first.
     * @note Time complexity: O(W^2) for basic/DSatur; O(K * W^2) for spilling/splitting.
     */
    void run();

    /**
     * @brief Writes the allocation result to a file in the specified format.
     * @param filename Output file path.
     */
    void writeOutput(const std::string& filename) const;

    /**
     * @brief Prints the allocation result to stdout (for interactive mode).
     */
    void printResult() const;
};

#endif // DA_PROJ2_REGISTERALLOCATORCONTROLLER_H
