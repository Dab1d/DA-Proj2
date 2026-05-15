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
    std::vector<LiveRange> liveRanges;
    std::vector<Web>       webs;
    Graph<int>             interferenceGraph;
    RegisterConfig         config;
    AllocationResult       result;
    bool                   rangesReady = false;
    bool                   configReady = false;
    bool                   loaded      = false;
    bool                   allocated   = false;

    /**
     * @brief Parses live ranges from a file and stores them internally.
     */
    void loadRangesFromFile(const std::string& filename);

    /**
     * @brief Parses register config from a file and stores it internally.
     */
    void loadConfigFromFile(const std::string& filename);

    /**
     * @brief Builds webs and the interference graph from the stored live ranges and config.
     * @note Call loadRangesFromFile() and loadConfigFromFile() first.
     */
    void build();

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

    size_t getRangeCount()               const { return liveRanges.size(); }
    size_t getWebCount()                 const { return webs.size(); }
    int    getNumRegisters()             const { return config.numRegisters; }
    bool   isFeasible()                  const { return result.feasible; }
    const AllocationResult& getResult()  const { return result; }
};

#endif // DA_PROJ2_REGISTERALLOCATORCONTROLLER_H
