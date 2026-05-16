#include "algorithms/GraphColoringHelpers.h"
#include <stack>
#include <algorithm>

using std::set;
using std::map;
using std::vector;
using std::stack;

int activeDegree(int node, const Graph<int>& g, const set<int>& active) {
    auto* v = g.findVertex(node);
    if (!v) return 0;
    int deg = 0;
    for (const auto& e : v->getAdj())
        if (active.count(e.getDest()->getInfo())) deg++;
    return deg;
}

int pickSpill(const set<int>& active, const Graph<int>& g,
              const map<int, const Web*>& webMap) {
    int best = -1, bestDeg = -1, bestLines = -1;
    for (int n : active) {
        int deg   = activeDegree(n, g, active);
        int lines = (int)webMap.at(n)->liveLines.size();
        if (deg > bestDeg || (deg == bestDeg && lines > bestLines)) {
            best = n; bestDeg = deg; bestLines = lines;
        }
    }
    return best;
}

map<int,int> greedyColor(const Graph<int>& g, const vector<Web>& webs, int K,
                          set<int> forcedSpills) {
    map<int, const Web*> webMap;
    for (const auto& w : webs) webMap[w.id] = &w;

    set<int> active;
    for (const auto& w : webs)
        if (!forcedSpills.count(w.id)) active.insert(w.id);

    stack<int> S;
    set<int> spilled = forcedSpills;

    bool progress = true;
    while (!active.empty() && progress) {
        progress = false;
        vector<int> toRemove;
        for (int n : active)
            if (activeDegree(n, g, active) < K) { toRemove.push_back(n); progress = true; }
        for (int n : toRemove) { S.push(n); active.erase(n); }
        if (!progress && !active.empty()) {
            int k = pickSpill(active, g, webMap);
            spilled.insert(k);
            active.erase(k);
            progress = true;
        }
    }

    map<int,int> webToReg;
    for (int s : spilled) webToReg[s] = -1;

    while (!S.empty()) {
        int n = S.top(); S.pop();
        auto* v = g.findVertex(n);
        set<int> usedColors;
        if (v)
            for (const auto& e : v->getAdj()) {
                int nb = e.getDest()->getInfo();
                auto it = webToReg.find(nb);
                if (it != webToReg.end() && it->second >= 0)
                    usedColors.insert(it->second);
            }
        int color = 0;
        while (usedColors.count(color)) color++;
        webToReg[n] = color;
    }

    return webToReg;
}

AllocationResult makeResult(const map<int,int>& reg, const vector<Web>& webs, int K) {
    AllocationResult res;
    res.webs = webs;
    res.webToRegister = reg;
    res.feasible = true;
    int maxColor = -1;
    for (auto& [id, c] : reg) {
        if (c < 0)  { res.feasible = false; break; }
        if (c >= K) { res.feasible = false; break; }
        maxColor = std::max(maxColor, c);
    }
    if (res.feasible) {
        res.registersUsed = maxColor + 1;
    } else {
        res.feasible      = false;
        res.registersUsed = 0;
        for (auto& [id, c] : res.webToRegister) c = -1;
    }
    return res;
}
