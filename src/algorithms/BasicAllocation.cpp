#include "algorithms/GraphColoring.h"
#include "algorithms/GraphColoringHelpers.h"

AllocationResult GraphColoring::basicAllocation(const Graph<int>& graph,
                                                 const std::vector<Web>& webs, int K) {
    auto reg = greedyColor(graph, webs, K);
    return makeResult(reg, webs, K);
}
