#include "algorithms/GraphColoring.h"
#include <set>
#include <map>
#include <vector>
#include <algorithm>

using std::set;
using std::map;
using std::vector;

AllocationResult GraphColoring::freeAllocation(const Graph<int>& graph,
                                                const vector<Web>& webs, int K) {
    map<int, int> degree;
    for (const auto& w : webs) {
        auto* v = graph.findVertex(w.id);
        degree[w.id] = v ? (int)v->getAdj().size() : 0;
    }

    vector<int> order;
    order.reserve(webs.size());
    for (const auto& w : webs) order.push_back(w.id);
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return degree[a] > degree[b];
    });

    map<int, int> reg;
    for (const auto& w : webs) reg[w.id] = -2;

    for (int id : order) {
        auto* v = graph.findVertex(id);

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

        int color = 0;
        while (usedByNeighbours.count(color)) color++;

        if (color < K) { reg[id] = color; continue; }

        bool rescued = false;
        for (int nb : colouredNeighbours) {
            if (rescued) break;
            int nbColor = reg[nb];
            auto* nbv   = graph.findVertex(nb);

            set<int> lockedForNb;
            if (nbv)
                for (const auto& e2 : nbv->getAdj()) {
                    int nb2 = e2.getDest()->getInfo();
                    if (nb2 != id && reg[nb2] >= 0)
                        lockedForNb.insert(reg[nb2]);
                }

            for (int alt = 0; alt < K; alt++) {
                if (alt == nbColor || lockedForNb.count(alt)) continue;

                bool nbColorStillUsed = false;
                for (int other : colouredNeighbours)
                    if (other != nb && reg[other] == nbColor) { nbColorStillUsed = true; break; }

                if (!nbColorStillUsed) {
                    reg[nb] = alt;
                    reg[id] = nbColor;
                    rescued  = true;
                    break;
                }
            }
        }

        if (!rescued) reg[id] = -1;
    }

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
