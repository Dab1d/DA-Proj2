#include "view/WebVisualizer.h"
#include "algorithms/WebBuilder.h"
#include "structures/Graph.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <map>

using std::cout;

static const char* CYAN    = "\033[36m";
static const char* YELLOW  = "\033[33m";
static const char* GREEN   = "\033[1;32m";
static const char* MAGENTA = "\033[35m";
static const char* RESET   = "\033[0m";

static constexpr double PI = 3.14159265358979323846;

static std::string fmtLines(const std::set<int>& lines) {
    if (lines.empty()) return "{}";
    std::string s = "{";
    for (int l : lines) { s += std::to_string(l); s += ' '; }
    s.back() = '}';
    return s;
}

// ---------------------------------------------------------------------------
// Circular ASCII graph renderer
// ---------------------------------------------------------------------------

static void printGraphVisualization(const Graph<int>& g, const std::vector<Web>& webs) {
    const int N = (int)webs.size();
    if (N == 0) return;

    // Fallback for dense graphs
    if (N > 12) {
        cout << "\033[1;36m\nInterference Graph (adjacency list):\n" << RESET;
        for (const auto& w : webs) {
            cout << "  web" << w.id << "(" << w.varName << ") -> ";
            auto* v = g.findVertex(w.id);
            bool any = false;
            if (v) for (const auto& e : v->getAdj()) {
                int nb = e.getDest()->getInfo();
                for (const auto& w2 : webs) if (w2.id == nb) {
                    cout << "web" << nb << "(" << w2.varName << ") ";
                    break;
                }
                any = true;
            }
            if (!any) cout << "(isolated)";
            cout << '\n';
        }
        return;
    }

    const int W = 70, H = 22;
    const int cx = W / 2, cy = H / 2;
    const double Rcol = N == 1 ? 0.0 : 26.0;
    const double Rrow = N == 1 ? 0.0 : 8.5;

    // Canvas: each cell = {character, color}
    using Cell = std::pair<std::string, std::string>;
    std::vector<std::vector<Cell>> canvas(H, std::vector<Cell>(W, {" ", ""}));

    auto safeSet = [&](int r, int c, const std::string& ch, const std::string& col) {
        if (r >= 0 && r < H && c >= 0 && c < W)
            canvas[r][c] = {ch, col};
    };

    // Node positions (circular layout, start at top)
    std::vector<std::pair<int,int>> pos(N);
    for (int i = 0; i < N; i++) {
        double angle = 2.0 * PI * i / N - PI / 2.0;
        int col = (int)std::round(cx + Rcol * std::cos(angle));
        int row = (int)std::round(cy + Rrow * std::sin(angle));
        pos[i] = {row, col};
    }

    // id → index map
    std::map<int,int> idxOf;
    for (int i = 0; i < N; i++) idxOf[webs[i].id] = i;

    const std::string EDGE_COLOR = "\033[2;37m"; // dim white for edges

    // Draw edges with Bresenham
    for (int i = 0; i < N; i++) {
        auto* v = g.findVertex(webs[i].id);
        if (!v) continue;
        for (const auto& e : v->getAdj()) {
            int jId = e.getDest()->getInfo();
            if (!idxOf.count(jId)) continue;
            int j = idxOf[jId];
            if (j <= i) continue; // draw each undirected edge once

            auto [r1, c1] = pos[i];
            auto [r2, c2] = pos[j];

            int dx = c2 - c1, dy = r2 - r1;
            int adx = dx < 0 ? -dx : dx;
            int ady = dy < 0 ? -dy : dy;

            // Single character chosen by overall slope
            std::string edgeCh;
            if (adx == 0) {
                edgeCh = "|";
            } else {
                double slope = (double)ady / adx;
                if      (slope < 0.4)                edgeCh = "-";
                else if (slope > 2.5)                edgeCh = "|";
                else if ((dx > 0) == (dy > 0))       edgeCh = "\\";
                else                                 edgeCh = "/";
            }

            // Bresenham line
            int x0 = c1, y0 = r1, x1 = c2, y1 = r2;
            int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
            int err = adx - ady;
            while (true) {
                safeSet(y0, x0, edgeCh, EDGE_COLOR);
                if (x0 == x1 && y0 == y1) break;
                int e2 = 2 * err;
                if (e2 > -ady) { err -= ady; x0 += sx; }
                if (e2 < adx)  { err += adx; y0 += sy; }
            }
        }
    }

    // Per-variable colors (nodes of the same variable share a color)
    static const char* COLORS[] = {
        "\033[1;31m", "\033[1;32m", "\033[1;33m",
        "\033[1;34m", "\033[1;35m", "\033[1;36m"
    };
    std::map<std::string, std::string> varCol;
    int ci = 0;
    for (const auto& w : webs)
        if (!varCol.count(w.varName))
            varCol[w.varName] = COLORS[ci++ % 6];

    // Draw node labels (overwrite edges so nodes appear on top)
    for (int i = 0; i < N; i++) {
        auto [row, col] = pos[i];
        const Web& w = webs[i];
        std::string label = "web" + std::to_string(w.id) + "(" + w.varName + ")";
        int startCol = col - (int)label.size() / 2;
        const std::string& color = varCol[w.varName];
        for (int k = 0; k < (int)label.size(); k++)
            safeSet(row, startCol + k, std::string(1, label[k]), color);
    }

    // Print canvas
    cout << "\033[1;36m\nInterference Graph:\n\n" << RESET;
    for (const auto& row : canvas) {
        for (const auto& [ch, col] : row) {
            if (!col.empty()) cout << col;
            cout << ch;
            if (!col.empty()) cout << RESET;
        }
        cout << '\n';
    }
    cout << '\n';
}

// ---------------------------------------------------------------------------

void buildWithVisualization(RegisterAllocatorController& alloc,
                            const std::vector<LiveRange>& ranges,
                            const RegisterConfig& cfg) {
    const auto delay = std::chrono::milliseconds(120);

    auto cb = [&](const WebBuildEvent& e) {
        switch (e.type) {
            case WebEventType::VAR_START:
                std::this_thread::sleep_for(delay);
                cout << '\n' << CYAN
                     << "Processing variable '" << e.varName << "'"
                     << "  (" << e.rangeCount
                     << (e.rangeCount == 1 ? " range)" : " ranges)")
                     << RESET << '\n';
                break;
            case WebEventType::MERGE:
                std::this_thread::sleep_for(delay);
                cout << YELLOW
                     << "  " << fmtLines(e.linesA)
                     << "  <-->  " << fmtLines(e.linesB)
                     << "   merged"
                     << RESET << '\n';
                break;
            case WebEventType::WEB_FORMED:
                std::this_thread::sleep_for(delay);
                cout << GREEN
                     << "  -> web" << e.webIdA
                     << "  " << e.varName
                     << "  lines: " << fmtLines(e.linesA)
                     << RESET << '\n';
                break;
            case WebEventType::EDGE_ADDED:
                std::this_thread::sleep_for(delay);
                cout << MAGENTA
                     << "  web" << e.webIdA
                     << "  <->  web" << e.webIdB
                     << "   shared: " << fmtLines(e.linesA)
                     << RESET << '\n';
                break;
        }
    };

    // Visualization pass (display only)
    Graph<int> tmpGraph;
    auto webs = WebBuilder::buildWebs(ranges, cb);
    cout << '\n' << CYAN << "Building interference graph..." << RESET << '\n';
    WebBuilder::buildInterferenceGraph(webs, tmpGraph, cb);

    // Render the final graph as a circular ASCII diagram
    printGraphVisualization(tmpGraph, webs);

    // Populate controller state for subsequent allocation
    alloc.build(ranges, cfg);
}
