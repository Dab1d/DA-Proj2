#ifndef DA_PROJ2_REGISTERALLOCATORCONTROLLER_H
#define DA_PROJ2_REGISTERALLOCATORCONTROLLER_H

#include "structures/Structures.h"
#include "structures/Graph.h"
#include <vector>
#include <string>

class RegisterAllocatorController {
public:
    std::vector<Web>   webs;
    Graph<int>         interferenceGraph;
    RegisterConfig     config;
    AllocationResult   result;
    bool               loaded    = false;
    bool               allocated = false;

    /**
     * @brief Builds webs and the interference graph from the provided live ranges and config.
     * @note Time complexity: O(R^2 * P + W^2) where R = ranges, P = points, W = webs.
     */
    void build(const std::vector<LiveRange>& ranges, const RegisterConfig& cfg);

    /**
     * @brief Runs the allocation algorithm selected in config.
     * @note Must call build() first.
     * @note Time complexity: O(W^2) for basic/DSatur; O(K * W^2) for spilling/splitting.
     */
    void run();

    /**
     * @brief Writes the allocation result to a file in the specified format.
     * @param filename Output file path.
     */
    void writeOutput(const std::string& filename) const;

    size_t getWebCount()                const { return webs.size(); }
    int    getNumRegisters()             const { return config.numRegisters; }
    bool   isFeasible()                  const { return result.feasible; }
    const AllocationResult& getResult()  const { return result; }
};

#endif // DA_PROJ2_REGISTERALLOCATORCONTROLLER_H
