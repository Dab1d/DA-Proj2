#ifndef DA_PROJ2_WEBBUILDER_H
#define DA_PROJ2_WEBBUILDER_H

#include "structures/Structures.h"
#include "structures/Graph.h"
#include <vector>

/**
 * @class WebBuilder
 * @brief Builds live webs from live ranges and constructs the interference graph.
 */
class WebBuilder {
public:
    /**
     * @brief Merges live ranges into webs using a greedy union approach.
     *
     * Two live ranges for the same variable are merged into one web if they share
     * any program line. Additionally, if one range ends at line L (isLastUse) and
     * another begins at L (isDefinition), they are fused per the "i=i+1" rule.
     *
     * @param ranges Parsed live ranges from the input file.
     * @return Vector of webs with sequential IDs.
     * @note Time complexity: O(R^2 * P) where R = number of live ranges, P = average points per range.
     */
    static std::vector<Web> buildWebs(const std::vector<LiveRange>& ranges);

    /**
     * @brief Builds the interference graph from a set of webs.
     *
     * Two webs interfere if their live line sets intersect, with the exception
     * that if the only common line L has web A defining at L and web B with last
     * use at L, they do NOT interfere.
     *
     * @param webs The webs to build the graph from.
     * @param graph Output graph where vertices are web IDs and edges are interferences.
     * @note Time complexity: O(W^2 * P) where W = number of webs, P = average live lines per web.
     */
    static void buildInterferenceGraph(const std::vector<Web>& webs, Graph<int>& graph);
};

#endif // DA_PROJ2_WEBBUILDER_H
