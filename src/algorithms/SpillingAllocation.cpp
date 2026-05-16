#include "algorithms/GraphColoring.h"
#include "algorithms/GraphColoringHelpers.h"
#include <set>
#include <map>
#include <algorithm>

using std::set;
using std::map;
using std::vector;

AllocationResult GraphColoring::spillingAllocation(const Graph<int>& graph,
                                                    const vector<Web>& webs,
                                                    int K, int maxSpills) {
    set<int> forcedSpills;
    map<int, const Web*> webMap;
    for (const auto& w : webs) webMap[w.id] = &w;

    for (int attempt = 0; attempt <= maxSpills; attempt++) {
        auto reg = greedyColor(graph, webs, K, forcedSpills);

        bool ok = true;
        int maxColor = -1;
        for (auto& [id, c] : reg) {
            if (c < 0 && !forcedSpills.count(id)) { ok = false; break; }
            if (c >= 0 && c >= K)                 { ok = false; break; }
            if (c >= 0) maxColor = std::max(maxColor, c);
        }
        if (ok) {
            AllocationResult res;
            res.feasible      = true;
            res.registersUsed = maxColor + 1;
            res.webs          = webs;
            res.webToRegister = reg;
            return res;
        }

        if (attempt == maxSpills) break;

        set<int> active;
        for (const auto& w : webs)
            if (!forcedSpills.count(w.id)) active.insert(w.id);
        if (active.empty()) break;

        int spill = pickSpill(active, graph, webMap);
        forcedSpills.insert(spill);
    }

    AllocationResult res;
    res.feasible      = false;
    res.registersUsed = 0;
    res.webs          = webs;
    for (const auto& w : webs) res.webToRegister[w.id] = -1;
    return res;
}
