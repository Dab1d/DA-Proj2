#include "algorithms/GraphColoring.h"
#include "algorithms/WebBuilder.h"
#include <stack>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <climits>

using std::vector;
using std::set;
using std::map;
using std::stack;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Compute active degree: number of neighbors that are still in activeNodes
static int activeDegree(int node, const Graph<int>& g, const set<int>& active) {
    auto* v = g.findVertex(node);
    if (!v) return 0;
    int deg = 0;
    for (const auto& e : v->getAdj())
        if (active.count(e.getDest()->getInfo())) deg++;
    return deg;
}

// Pick the node to spill: highest active degree, tie-break by most live lines
static int pickSpill(const set<int>& active, const Graph<int>& g,
                     const map<int, const Web*>& webMap) {
    int best = -1, bestDeg = -1, bestLines = -1;
    for (int n : active) {
        int deg = activeDegree(n, g, active);
        int lines = (int)webMap.at(n)->liveLines.size();
        if (deg > bestDeg || (deg == bestDeg && lines > bestLines)) {
            best = n; bestDeg = deg; bestLines = lines;
        }
    }
    return best;
}

// Core greedy coloring: given active nodes and the full graph, try to color with K colors.
// Returns webToRegister map (-1 = spilled).
static map<int,int> greedyColor(const Graph<int>& g, const vector<Web>& webs, int K,
                                 set<int> forcedSpills = {}) {
    map<int, const Web*> webMap;
    for (const auto& w : webs) webMap[w.id] = &w;

    set<int> active;
    for (const auto& w : webs)
        if (!forcedSpills.count(w.id)) active.insert(w.id);

    stack<int> S;
    set<int> spilled = forcedSpills;

    // Phase 1: reduction
    bool progress = true;
    while (!active.empty() && progress) {
        progress = false;
        vector<int> toRemove;
        for (int n : active) {
            if (activeDegree(n, g, active) < K) {
                toRemove.push_back(n);
                progress = true;
            }
        }
        for (int n : toRemove) {
            S.push(n);
            active.erase(n);
        }
        if (!progress && !active.empty()) {
            // Must spill one node
            int k = pickSpill(active, g, webMap);
            spilled.insert(k);
            active.erase(k);
            progress = true;
        }
    }

    // Phase 2: coloring
    map<int,int> webToReg;
    for (int s : spilled) webToReg[s] = -1;

    while (!S.empty()) {
        int n = S.top(); S.pop();
        auto* v = g.findVertex(n);

        set<int> usedColors;
        if (v) {
            for (const auto& e : v->getAdj()) {
                int nb = e.getDest()->getInfo();
                auto it = webToReg.find(nb);
                if (it != webToReg.end() && it->second >= 0)
                    usedColors.insert(it->second);
            }
        }
        int color = 0;
        while (usedColors.count(color)) color++;
        webToReg[n] = color;
    }

    return webToReg;
}

// Build AllocationResult from a webToRegister map
static AllocationResult makeResult(const map<int,int>& reg, const vector<Web>& webs, int K) {
    AllocationResult res;
    res.webs = webs;
    res.webToRegister = reg;

    // Check feasibility: any spilled? any color >= K?
    res.feasible = true;
    int maxColor = -1;
    for (auto& [id, c] : reg) {
        if (c < 0) { res.feasible = false; break; }
        if (c >= K) { res.feasible = false; break; }
        maxColor = std::max(maxColor, c);
    }

    if (res.feasible) {
        res.registersUsed = maxColor + 1;
    } else {
        res.feasible = false;
        res.registersUsed = 0;
        for (auto& [id, c] : res.webToRegister) c = -1;
    }
    return res;
}

// ---------------------------------------------------------------------------
// T2.1 — Basic allocation
// ---------------------------------------------------------------------------

AllocationResult GraphColoring::basicAllocation(const Graph<int>& graph,
                                                 const vector<Web>& webs, int K) {
    auto reg = greedyColor(graph, webs, K);
    return makeResult(reg, webs, K);
}

// ---------------------------------------------------------------------------
// T2.2 — Spilling allocation
// ---------------------------------------------------------------------------

AllocationResult GraphColoring::spillingAllocation(const Graph<int>& graph,
                                                    const vector<Web>& webs,
                                                    int K, int maxSpills) {
    set<int> forcedSpills;
    map<int, const Web*> webMap;
    for (const auto& w : webs) webMap[w.id] = &w;

    for (int attempt = 0; attempt <= maxSpills; attempt++) {
        auto reg = greedyColor(graph, webs, K, forcedSpills);

        // Feasible iff no unforced spills and no color overflow
        bool ok = true;
        int maxColor = -1;
        for (auto& [id, c] : reg) {
            if (c < 0 && !forcedSpills.count(id)) { ok = false; break; }
            if (c >= 0 && c >= K) { ok = false; break; }
            if (c >= 0) maxColor = std::max(maxColor, c);
        }
        if (ok) {
            AllocationResult res;
            res.feasible = true;
            res.registersUsed = maxColor + 1;
            res.webs = webs;
            res.webToRegister = reg;
            return res;
        }

        if (attempt == maxSpills) break;

        // Pick the best candidate to spill next: highest degree among non-forced
        set<int> active;
        for (const auto& w : webs)
            if (!forcedSpills.count(w.id)) active.insert(w.id);

        if (active.empty()) break;
        int spill = pickSpill(active, graph, webMap);
        forcedSpills.insert(spill);
    }

    // Exhausted spill budget without achieving K-colorability: spill everything
    AllocationResult res;
    res.feasible = false;
    res.registersUsed = 0;
    res.webs = webs;
    for (const auto& w : webs) res.webToRegister[w.id] = -1;
    return res;
}

// ---------------------------------------------------------------------------
// T2.3 — Splitting allocation
// ---------------------------------------------------------------------------

AllocationResult GraphColoring::splittingAllocation(const Graph<int>& graph,
                                                      vector<Web>& webs,
                                                      int K, int maxSplits) {
    // Work on a mutable copy of webs and rebuild the graph after each split
    vector<Web> currentWebs = webs;
    int nextId = 0;
    for (const auto& w : currentWebs) nextId = std::max(nextId, w.id + 1);

    for (int attempt = 0; attempt <= maxSplits; attempt++) {
        // Rebuild interference graph for current webs
        Graph<int> g;
        WebBuilder::buildInterferenceGraph(currentWebs, g);

        auto reg = greedyColor(g, currentWebs, K);
        auto res = makeResult(reg, currentWebs, K);
        if (res.feasible) {
            webs = currentWebs;
            return res;
        }

        if (attempt == maxSplits) break;

        // Find the highest-degree web to split
        set<int> active;
        for (const auto& w : currentWebs) active.insert(w.id);
        map<int, const Web*> wmap;
        for (const auto& w : currentWebs) wmap[w.id] = &w;

        int victim = pickSpill(active, g, wmap);
        if (victim < 0) break;

        // Find victim web and split it
        auto it = std::find_if(currentWebs.begin(), currentWebs.end(),
                               [victim](const Web& w) { return w.id == victim; });
        if (it == currentWebs.end()) break;

        Web& vw = *it;
        vector<int> lines(vw.liveLines.begin(), vw.liveLines.end());
        if (lines.size() < 2) break; // Can't split a single-line web

        int half = lines.size() / 2;

        // Sub-web A: first half of lines
        Web wa;
        wa.id = nextId++;
        wa.varName = vw.varName;
        for (int i = 0; i < half; i++) {
            wa.liveLines.insert(lines[i]);
            for (const auto& pp : vw.points)
                if (pp.line == lines[i]) wa.points.push_back(pp);
        }

        // Sub-web B: second half
        Web wb;
        wb.id = nextId++;
        wb.varName = vw.varName;
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
    res.feasible = false;
    res.registersUsed = 0;
    res.webs = currentWebs;
    for (const auto& w : currentWebs) res.webToRegister[w.id] = -1;
    webs = currentWebs;
    return res;
}

// ---------------------------------------------------------------------------
// T2.4 – Priority-Aware Spill-Safe allocation
//
// Strategy:
//   1. Sort webs by degree descending (hardest to colour first).
//   2. For each web, assign the smallest colour not used by neighbours.
//   3. If that colour would exceed K, attempt a *local recolour*: scan
//      already-coloured neighbours and check whether any of them can be
//      moved to a different colour, freeing a slot for the current web.
//   4. Only if local recolouring also fails, spill the current web (reg = -1).
//
// This avoids the eagerly-spill behaviour of plain greedy and the full
// saturation scan of DSatur, instead investing one extra O(K) probe per
// conflict to rescue colours before giving up.
//
// Time complexity: O(W^2 * K) in the worst case, O(W^2) on typical inputs
// where conflicts are sparse.
// ---------------------------------------------------------------------------

AllocationResult GraphColoring::freeAllocation(const Graph<int>& graph,
                                                const vector<Web>& webs, int K) {

    // ------------------------------------------------------------------
    // 1. Build degree table and sort webs by degree descending
    // ------------------------------------------------------------------
    map<int, int> degree;
    for (const auto& w : webs) {
        auto* v = graph.findVertex(w.id);
        degree[w.id] = v ? (int)v->getAdj().size() : 0;
    }

    vector<int> order;
    order.reserve(webs.size());
    for (const auto& w : webs) order.push_back(w.id);

    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return degree[a] > degree[b];   // descending degree
    });

    // ------------------------------------------------------------------
    // 2. Greedy colouring with local-recolour fallback
    // ------------------------------------------------------------------
    map<int, int> reg;   // web id -> colour (-1 = spilled, -2 = uncoloured)
    for (const auto& w : webs) reg[w.id] = -2;

    for (int id : order) {
        auto* v = graph.findVertex(id);

        // Collect colours used by already-coloured neighbours
        set<int> usedByNeighbours;
        vector<int> colouredNeighbours;
        if (v) {
            for (const auto& e : v->getAdj()) {
                int nb = e.getDest()->getInfo();
                if (reg[nb] >= 0) {
                    usedByNeighbours.insert(reg[nb]);
                    colouredNeighbours.push_back(nb);
                }
            }
        }

        // Smallest colour not blocked by any neighbour
        int color = 0;
        while (usedByNeighbours.count(color)) color++;

        if (color < K) {
            // Happy path: fits within K registers
            reg[id] = color;
            continue;
        }

        // ------------------------------------------------------------------
        // 3. Local recolour attempt
        //
        // For each already-coloured neighbour nb (colour = C):
        //   – collect colours used by nb's OWN neighbours (excluding `id`)
        //   – if nb can be moved to any colour c' in [0,K) not in that set,
        //     then colour C becomes free for `id`
        //   – accept the first such rescue found
        // ------------------------------------------------------------------
        bool rescued = false;

        for (int nb : colouredNeighbours) {
            if (rescued) break;

            int nbColor = reg[nb];
            auto* nbv   = graph.findVertex(nb);

            // Colours locked for nb (by nb's neighbours, excluding `id`)
            set<int> lockedForNb;
            if (nbv) {
                for (const auto& e2 : nbv->getAdj()) {
                    int nb2 = e2.getDest()->getInfo();
                    if (nb2 != id && reg[nb2] >= 0)
                        lockedForNb.insert(reg[nb2]);
                }
            }

            // Can nb move to some other colour in [0, K)?
            for (int alt = 0; alt < K; alt++) {
                if (alt == nbColor)         continue;
                if (lockedForNb.count(alt)) continue;

                // Would moving nb actually free nbColor for id?
                bool nbColorStillUsed = false;
                for (int other : colouredNeighbours) {
                    if (other != nb && reg[other] == nbColor) {
                        nbColorStillUsed = true;
                        break;
                    }
                }

                if (!nbColorStillUsed) {
                    reg[nb] = alt;
                    reg[id] = nbColor;
                    rescued = true;
                    break;
                }
            }
        }

        if (!rescued) {
            // All rescue attempts failed → spill this web
            reg[id] = -1;
        }
    }

    // ------------------------------------------------------------------
    // 4. Build AllocationResult
    // ------------------------------------------------------------------
    bool feasible = true;
    int  maxColor = -1;

    for (const auto& [id, c] : reg) {
        if (c < 0) { feasible = false; break; }
        maxColor = std::max(maxColor, c);
    }

    AllocationResult res;
    res.webs = webs;

    if (feasible) {
        res.feasible      = true;
        res.registersUsed = maxColor + 1;
        res.webToRegister = reg;
    } else {
        res.feasible      = false;
        res.registersUsed = 0;
        for (const auto& w : webs) res.webToRegister[w.id] = -1;
    }

    return res;
}