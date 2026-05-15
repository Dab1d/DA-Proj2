#include "algorithms/WebBuilder.h"
#include <algorithm>
#include <numeric>

using std::vector;
using std::set;

// Union-Find helpers (index-based, for merging ranges within a variable)
static int ufFind(vector<int>& uf, int x) {
    while (uf[x] != x) {
        uf[x] = uf[uf[x]];
        x = uf[x];
    }
    return x;
}

static void ufUnion(vector<int>& uf, int a, int b) {
    a = ufFind(uf, a);
    b = ufFind(uf, b);
    if (a != b) uf[b] = a;
}

// Build the liveLines set for a live range (all line numbers covered)
static set<int> makeLineSet(const LiveRange& lr) {
    set<int> s;
    for (const auto& pp : lr.points)
        s.insert(pp.line);
    return s;
}

// Check whether two live ranges should be merged:
// They merge if they share any line, OR if one ends at L (isLastUse) and the
// other starts at L (isDefinition) — the "i=i+1" adjacency rule.
static bool shouldMerge(const LiveRange& a, const set<int>& aLines,
                        const LiveRange& b, const set<int>& bLines) {
    // Check plain line intersection
    for (int ln : aLines)
        if (bLines.count(ln)) return true;

    // Check adjacency: a ends at L-, b starts at L+  (or vice versa)
    for (const auto& pa : a.points) {
        if (!pa.isLastUse) continue;
        for (const auto& pb : b.points) {
            if (pb.isDefinition && pb.line == pa.line) return true;
        }
    }
    for (const auto& pb : b.points) {
        if (!pb.isLastUse) continue;
        for (const auto& pa : a.points) {
            if (pa.isDefinition && pa.line == pb.line) return true;
        }
    }
    return false;
}

vector<Web> WebBuilder::buildWebs(const vector<LiveRange>& ranges) {
    // Group range indices by variable name
    std::map<std::string, vector<int>> byVar;
    for (int i = 0; i < (int)ranges.size(); i++)
        byVar[ranges[i].varName].push_back(i);

    vector<Web> webs;
    int nextId = 0;

    for (auto& [varName, indices] : byVar) {
        int n = indices.size();
        vector<int> uf(n);
        std::iota(uf.begin(), uf.end(), 0);

        // Pre-compute line sets
        vector<set<int>> lineSets(n);
        for (int i = 0; i < n; i++)
            lineSets[i] = makeLineSet(ranges[indices[i]]);

        // Merge overlapping/adjacent ranges
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (ufFind(uf, i) == ufFind(uf, j)) continue;
                    if (shouldMerge(ranges[indices[i]], lineSets[i],
                                    ranges[indices[j]], lineSets[j])) {
                        ufUnion(uf, i, j);
                        changed = true;
                    }
                }
            }
        }

        // Collect roots → web groups
        std::map<int, vector<int>> groups;
        for (int i = 0; i < n; i++)
            groups[ufFind(uf, i)].push_back(i);

        for (auto& [root, members] : groups) {
            // Each original live range entry becomes one Web entry (same id = same web)
            // We produce one Web per original range, all with the same id if merged.
            // Actually, we produce one Web per merged group; its points = union of all member points.
            set<int> allLines;
            vector<ProgramPoint> allPoints;

            for (int m : members) {
                const LiveRange& lr = ranges[indices[m]];
                for (const auto& pp : lr.points) {
                    allLines.insert(pp.line);
                    allPoints.push_back(pp);
                }
            }

            // Sort points by line number
            std::sort(allPoints.begin(), allPoints.end(),
                      [](const ProgramPoint& a, const ProgramPoint& b) {
                          return a.line < b.line;
            });

            // Remove duplicate lines (keep the one with a marker if present)
            vector<ProgramPoint> deduped;
            for (const auto& pp : allPoints) {
                if (!deduped.empty() && deduped.back().line == pp.line) {
                    // Keep whichever has a marker (+ or -)
                    if (!deduped.back().isDefinition && !deduped.back().isLastUse) {
                        deduped.back() = pp;
                    }
                } else {
                    deduped.push_back(pp);
                }
            }

            Web w;
            w.id = nextId++;
            w.varName = varName;
            w.points = std::move(deduped);
            w.liveLines = std::move(allLines);
            webs.push_back(std::move(w));
        }
    }

    // Sort webs by id for deterministic output
    std::sort(webs.begin(), webs.end(), [](const Web& a, const Web& b) {
        return a.id < b.id;
    });

    return webs;
}

void WebBuilder::buildInterferenceGraph(const vector<Web>& webs, Graph<int>& graph) {
    // Add one vertex per web
    for (const auto& w : webs)
        graph.addVertex(w.id);

    int n = webs.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            const Web& wi = webs[i];
            const Web& wj = webs[j];

            // Find common lines
            vector<int> common;
            std::set_intersection(wi.liveLines.begin(), wi.liveLines.end(),
                                  wj.liveLines.begin(), wj.liveLines.end(),
                                  std::back_inserter(common));
            if (common.empty()) continue;

            // Check exception: sole common line is wi-defines / wj-last-use or vice versa
            if (common.size() == 1) {
                int L = common[0];
                bool iDef = false, jLast = false, jDef = false, iLast = false;
                for (const auto& pp : wi.points)
                    if (pp.line == L) { iDef |= pp.isDefinition; iLast |= pp.isLastUse; }
                for (const auto& pp : wj.points)
                    if (pp.line == L) { jDef |= pp.isDefinition; jLast |= pp.isLastUse; }

                if ((iDef && jLast) || (jDef && iLast)) continue; // no interference
            }

            // They interfere: add bidirectional edges
            graph.addEdge(wi.id, wj.id, 0);
            graph.addEdge(wj.id, wi.id, 0);
        }
    }
}
