#ifndef DA_PROJ2_GRAPHCOLORING_H
#define DA_PROJ2_GRAPHCOLORING_H

#include "structures/Structures.h"
#include "structures/Graph.h"
#include <vector>

/**
 * @class GraphColoring
 * @brief Graph coloring algorithms for register allocation.
 */
class GraphColoring {
public:
    /**
     * @brief Basic greedy graph coloring (T2.1).
     *
     * Iteratively removes nodes with degree < K, pushes them onto a stack,
     * then colors them on pop. If a node cannot be removed (all degrees >= K),
     * it is spilled. If any spills occur the result is infeasible.
     *
     * @param graph Interference graph (vertices = web IDs).
     * @param webs  All webs (used for heuristics and result building).
     * @param K     Number of available physical registers.
     * @return AllocationResult with feasible=false if spills occurred.
     * @note Time complexity: O(W^2) where W = number of webs.
     */
    static AllocationResult basicAllocation(const Graph<int>& graph,
                                            const std::vector<Web>& webs,
                                            int K);

    /**
     * @brief Register allocation with web spilling (T2.2).
     *
     * Tries basic coloring; if infeasible, spills the highest-degree web and
     * retries, up to maxSpills times. Spilled webs are assigned to memory (M).
     *
     * @param graph     Interference graph.
     * @param webs      All webs.
     * @param K         Number of available physical registers.
     * @param maxSpills Maximum number of webs allowed to be spilled.
     * @return AllocationResult; feasible if coloring succeeded within spill budget.
     * @note Time complexity: O(maxSpills * W^2).
     */
    static AllocationResult spillingAllocation(const Graph<int>& graph,
                                               const std::vector<Web>& webs,
                                               int K, int maxSpills);

    /**
     * @brief Register allocation with web splitting (T2.3).
     *
     * Tries basic coloring; if infeasible, splits the highest-degree web into
     * two sub-webs (first/second half of its live lines) and rebuilds the
     * interference graph, up to maxSplits times.
     *
     * @param graph     Original interference graph.
     * @param webs      All webs (may be extended with split sub-webs).
     * @param K         Number of available physical registers.
     * @param maxSplits Maximum number of web splits allowed.
     * @return AllocationResult after splitting.
     * @note Time complexity: O(maxSplits * W^2).
     */
    static AllocationResult splittingAllocation(const Graph<int>& graph,
                                                std::vector<Web>& webs,
                                                int K, int maxSplits);

    /**
     * @brief Custom register allocation using DSatur heuristic (T2.4).
     *
     * Colors the graph by always choosing the uncolored vertex with the highest
     * saturation (number of distinct colors among its neighbors). Ties broken by
     * degree. No spilling — reports infeasible if K colors are insufficient.
     *
     * @param graph Interference graph.
     * @param webs  All webs.
     * @param K     Number of available physical registers.
     * @return AllocationResult.
     * @note Time complexity: O(W^2) where W = number of webs.
     */
    static AllocationResult freeAllocation(const Graph<int>& graph,
                                           const std::vector<Web>& webs,
                                           int K);
};

#endif // DA_PROJ2_GRAPHCOLORING_H
