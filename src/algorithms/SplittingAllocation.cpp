#include "algorithms/GraphColoring.h"
#include "algorithms/GraphColoringHelpers.h"
#include "algorithms/WebBuilder.h"
#include <set>
#include <map>
#include <algorithm>

using std::set;
using std::map;
using std::vector;

AllocationResult GraphColoring::splittingAllocation(const Graph<int>& graph,
                                                      vector<Web>& webs,
                                                      int K, int maxSplits) {
    vector<Web> currentWebs = webs;
    int nextId = 0;
    for (const auto& w : currentWebs) nextId = std::max(nextId, w.id + 1);

    for (int attempt = 0; attempt <= maxSplits; attempt++) {
        Graph<int> g;
        WebBuilder::buildInterferenceGraph(currentWebs, g);

        auto reg = greedyColor(g, currentWebs, K);
        auto res = makeResult(reg, currentWebs, K);
        if (res.feasible) {
            webs = currentWebs;
            return res;
        }

        if (attempt == maxSplits) break;

        set<int> active;
        for (const auto& w : currentWebs) active.insert(w.id);
        map<int, const Web*> wmap;
        for (const auto& w : currentWebs) wmap[w.id] = &w;

        int victim = pickSpill(active, g, wmap);
        if (victim < 0) break;

        auto it = std::find_if(currentWebs.begin(), currentWebs.end(),
                               [victim](const Web& w) { return w.id == victim; });
        if (it == currentWebs.end()) break;

        Web& vw = *it;
        vector<int> lines(vw.liveLines.begin(), vw.liveLines.end());
        if (lines.size() < 2) break;

        int half = lines.size() / 2;

        Web wa; wa.id = nextId++; wa.varName = vw.varName;
        for (int i = 0; i < half; i++) {
            wa.liveLines.insert(lines[i]);
            for (const auto& pp : vw.points)
                if (pp.line == lines[i]) wa.points.push_back(pp);
        }

        Web wb; wb.id = nextId++; wb.varName = vw.varName;
        for (int i = half; i < (int)lines.size(); i++) {
            wb.liveLines.insert(lines[i]);
            for (const auto& pp : vw.points)
                if (pp.line == lines[i]) wb.points.push_back(pp);
        }

        currentWebs.erase(it);
        if (!wa.liveLines.empty()) currentWebs.push_back(std::move(wa));
        if (!wb.liveLines.empty()) currentWebs.push_back(std::move(wb));
    }

    AllocationResult res;
    res.feasible      = false;
    res.registersUsed = 0;
    res.webs          = currentWebs;
    for (const auto& w : currentWebs) res.webToRegister[w.id] = -1;
    webs = currentWebs;
    return res;
}
