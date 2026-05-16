#ifndef DA_PROJ2_GRAPHCOLORINGHELPERS_H
#define DA_PROJ2_GRAPHCOLORINGHELPERS_H

#include "structures/Structures.h"
#include "structures/Graph.h"
#include <set>
#include <map>
#include <vector>

int activeDegree(int node, const Graph<int>& g, const std::set<int>& active);

int pickSpill(const std::set<int>& active, const Graph<int>& g,
              const std::map<int, const Web*>& webMap);

std::map<int,int> greedyColor(const Graph<int>& g, const std::vector<Web>& webs, int K,
                               std::set<int> forcedSpills = {});

AllocationResult makeResult(const std::map<int,int>& reg,
                            const std::vector<Web>& webs, int K);

#endif // DA_PROJ2_GRAPHCOLORINGHELPERS_H
