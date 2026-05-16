#ifndef DA_PROJ2_GRAPHCOLORING_H
#define DA_PROJ2_GRAPHCOLORING_H

#include "structures/Structures.h"
#include "structures/Graph.h"
#include <vector>

/**
 * @file GraphColoring.h
 * @brief Declares graph-coloring-based register allocation algorithms.
 *
 * All methods operate on an **interference graph** whose vertices are web IDs
 * (integers) and whose (undirected) edges represent live-range conflicts between
 * webs.  Each algorithm tries to assign at most K distinct colours (physical
 * registers) to the vertices; webs that cannot be coloured are *spilled* to
 * memory and receive register index -1 in the result.
 */

/**
 * @class GraphColoring
 * @brief Static factory of graph-colouring algorithms for register allocation.
 *
 * Every public method takes the same basic inputs (an interference graph, the
 * web metadata vector and the register count K) and returns an @ref
 * AllocationResult describing the assignment.  The four methods correspond to
 * the four tasks in the project brief:
 *
 * | Method              | Task  | Strategy                          |
 * |---------------------|-------|-----------------------------------|
 * | basicAllocation     | T2.1  | Greedy / Chaitin reduction        |
 * | spillingAllocation  | T2.2  | Greedy + iterative web spilling   |
 * | splittingAllocation | T2.3  | Greedy + iterative web splitting  |
 * | freeAllocation      | T2.4  | DSatur saturation-degree ordering |
 */
class GraphColoring {
public:

    // ------------------------------------------------------------------
    // T2.1 – Basic greedy allocation
    // ------------------------------------------------------------------

    /**
     * @brief Basic greedy graph colouring (Chaitin-style).
     *
     * ### Algorithm
     * 1. **Reduction phase** – Repeatedly find any vertex whose current
     *    (active) degree is strictly less than K and push it onto a stack,
     *    removing it from the active set.  If no such vertex exists but the
     *    active set is non-empty, the vertex with the highest active degree
     *    (tie-broken by number of live program lines) is *spilled* and
     *    removed without being pushed.
     * 2. **Colouring phase** – Pop each vertex from the stack and assign the
     *    smallest non-negative colour not already used by any of its
     *    *graph* neighbours (spilled neighbours are ignored).
     *
     * The allocation is **infeasible** if at least one web was spilled during
     * the reduction phase; in that case every web receives register -1.
     *
     * @param graph Interference graph; vertices are web IDs, edges are
     *              bidirectional interference arcs.
     * @param webs  Metadata for all webs (used for the spill heuristic and
     *              for building the result).
     * @param K     Number of available physical registers (colours).
     * @return      @ref AllocationResult with @c feasible = false if any
     *              spill occurred.
     *
     * @note **Time complexity:** O(W²) where W = number of webs.
     *       Each reduction iteration scans the active set (O(W)) and checks
     *       neighbours (O(W)); the outer loop runs at most W times.
     */
    static AllocationResult basicAllocation(const Graph<int>& graph,
                                            const std::vector<Web>& webs,
                                            int K);

    // ------------------------------------------------------------------
    // T2.2 – Allocation with web spilling
    // ------------------------------------------------------------------

    /**
     * @brief Register allocation with iterative web spilling.
     *
     * ### Algorithm
     * Attempts basic greedy colouring.  If the attempt is infeasible, the
     * web with the highest interference degree (tie-broken by the number of
     * live program lines) is *forced* to memory and the colouring is retried
     * on the remaining webs.  This loop repeats until:
     * - a feasible K-colouring of the non-spilled webs is found, **or**
     * - the number of forced spills has reached @p maxSpills, **or**
     * - all webs have been spilled.
     *
     * If no feasible colouring is found within the spill budget the result
     * is marked infeasible and every web receives register -1.
     *
     * ### Spill heuristic
     * Among the still-active (non-spilled) webs, spill the one with the
     * highest number of active neighbours.  Ties are broken by choosing the
     * web whose live range covers the most program lines, since spilling it
     * frees the most interference arcs.
     *
     * @param graph     Interference graph.
     * @param webs      All webs.
     * @param K         Number of available physical registers.
     * @param maxSpills Maximum number of webs that may be spilled to memory.
     *                  Pass 0 to forbid any spill (equivalent to basicAllocation).
     * @return          @ref AllocationResult; @c feasible = true iff a valid
     *                  K-colouring was found within the spill budget.
     *                  Spilled webs have @c webToRegister[id] = -1.
     *
     * @note **Time complexity:** O(maxSpills × W²) in the worst case.
     *       Each of the (maxSpills + 1) attempts runs the O(W²) greedy
     *       reduction; selecting the best spill candidate is O(W²) per round.
     */
    static AllocationResult spillingAllocation(const Graph<int>& graph,
                                               const std::vector<Web>& webs,
                                               int K, int maxSpills);

    // ------------------------------------------------------------------
    // T2.3 – Allocation with web splitting
    // ------------------------------------------------------------------

    /**
     * @brief Register allocation with iterative web splitting.
     *
     * ### Algorithm
     * Attempts basic greedy colouring on the current web set.  If infeasible,
     * the highest-degree web is *split* into two sub-webs that cover
     * non-overlapping halves of its live program lines.  The interference
     * graph is then *rebuilt* from scratch for the new web set and the
     * colouring is retried.  The loop repeats until a feasible colouring is
     * found or @p maxSplits splits have been performed.
     *
     * A web can only be split if it spans at least two program lines; a
     * single-line web is left as-is and terminates the loop early.
     *
     * @param graph     Original interference graph (used as a starting point;
     *                  the graph is rebuilt internally after each split).
     * @param webs      All webs; **may be modified** to include the new
     *                  sub-webs produced by splitting.
     * @param K         Number of available physical registers.
     * @param maxSplits Maximum number of web splits allowed.
     * @return          @ref AllocationResult after splitting.
     *
     * @note **Time complexity:** O(maxSplits × W²) in the worst case,
     *       dominated by the repeated interference-graph construction
     *       (O(W² × P)) and greedy colouring (O(W²)) at each iteration,
     *       where P = average live lines per web.
     */
    static AllocationResult splittingAllocation(const Graph<int>& graph,
                                                std::vector<Web>& webs,
                                                int K, int maxSplits);

    // ------------------------------------------------------------------
    // T2.4 – PASS (Priority-Aware Spill-Safe) free allocation
    // ------------------------------------------------------------------

    /**
     * @brief Custom register allocation using the DSatur heuristic.
     *
     * ### Algorithm (DSatur – Brélaz 1979)
     * 1. Maintain a *saturation* counter for every uncoloured vertex: the
     *    number of *distinct* colours already used by its neighbours.
     * 2. Repeatedly colour the uncoloured vertex with the highest saturation.
     *    Ties are broken by choosing the vertex with the highest total degree.
     * 3. Assign the smallest colour not used by any neighbour.
     * 4. Update the saturation counters of all uncoloured neighbours.
     *
     * No spilling is performed; if at least one web requires a colour ≥ K the
     * allocation is reported as infeasible and every web receives register -1.
     *
     * DSatur is optimal on perfect graphs and empirically produces fewer
     * colours than plain greedy on many interference graphs.
     *
     * @param graph Interference graph.
     * @param webs  All webs.
     * @param K     Number of available physical registers.
     * @return      @ref AllocationResult; @c feasible = false if more than K
     *              colours were needed.
     *
     * @note **Time complexity:** O(W²) where W = number of webs.
     *       Each of the W colouring steps scans all uncoloured vertices to
     *       find the maximum-saturation candidate (O(W)) and then updates the
     *       saturation of its neighbours (O(W) in the worst case).
     */
    static AllocationResult freeAllocation(const Graph<int>& graph,
                                           const std::vector<Web>& webs,
                                           int K);
};

#endif // DA_PROJ2_GRAPHCOLORING_H