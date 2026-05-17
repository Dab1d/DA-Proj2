#include "algorithms/GraphColoring.h"
#include "algorithms/GraphColoringHelpers.h"

AllocationResult GraphColoring::basicAllocation(const Graph<int>& graph,
                                                 const std::vector<Web>& webs,
                                                 int K, AllocCb cb) {
    if (cb) cb({AllocEventType::ALGO_START, -1, -1, -1, -1, "", "basic"});
    auto reg = greedyColor(graph, webs, K, {}, cb);
    return makeResult(reg, webs, K);
}
