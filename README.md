# Programming Project II — Compiler Register Allocation


## TODO:
Estruturas: adicionar Web e AllocationResult a Structures.h

WebBuilder: implementar buildWebs e buildInterferenceGraph

GraphColoring: T2.1 basicAllocation

RegisterAllocatorController: load, run, writeOutput, printResult

BatchMode: ligar ao controller

InteractiveMode: substituir TODOs por chamadas reais

CMakeLists.txt: adicionar novos .cpp

GraphColoring: T2.2 spillingAllocation

GraphColoring: T2.3 splittingAllocation

GraphColoring: T2.4 freeAllocation (DSatur)

Compilar e testar com datasets
## Quick Summary

You're building a **C++ tool that allocates registers to variables** in a compiler back-end, using **graph coloring** (an NP-complete problem tackled with heuristics).

**The pipeline:**

1. **Input** → A text file listing each variable and its *live ranges* (program lines where it's "alive"), plus a second file specifying the number of available registers `N` and which algorithm to run.
2. **Build webs** → Merge overlapping live ranges of the same variable into "webs" (a web = one logical value that needs a register).
3. **Build interference graph** → One node per web; add an edge between two webs if they're alive at the same execution point (they "interfere" and can't share a register).
4. **Color the graph** → Each color = one physical register. Two adjacent nodes can't share a color. Goal: use the **minimum** number of registers.
5. **Output** → A file listing webs and their register assignment (`r0`, `r1`, …) or `M` if spilled to memory.

**Four algorithm modes** (chosen via input file):

| Mode | What it does | Points |
|---|---|---|
| `basic` | Standard greedy coloring (Figure 9 in spec) | 4.0 |
| `spilling, K` | If basic fails, drop up to K webs to memory | 3.0 |
| `splitting, K` | If basic fails, split up to K webs into smaller pieces | 3.0 |
| `free` | Your own creative algorithm | 4.0 |

**Plus:** CLI tool + menu (1.0), input parsing using the **TP graph class** (1.0), Doxygen docs + complexity analysis (2.0), and a 10-min demo (2.0).

**Deadline:** May 24, 2026 — groups of 2–3 (3 preferred).

---

## Initial Steps (Recommended Order)

**1. Set up the project & team**
- Form your group of 3, pick a TP number, set up a Git repo.
- Decide on C++ standard, build system (CMake recommended), and folder layout (`src/`, `include/`, `tests/`, `docs/`).

**2. Get the TP graph class ready**
- Grab the graph data structure from your TP lectures — this is **mandatory** as the base for the interference graph. Don't reinvent it.
- Plan minor extensions you'll need (e.g., node "disabled" flag for the coloring stack phase, web metadata per node).

**3. Write the input parser first**
- Parser for the **ranges file**: handle comments (`#`), variable names, line numbers with `+` (definition) and `-` (last use), multiple ranges per variable.
- Parser for the **registers/algorithm file**: `registers: N` and `algorithm: basic|spilling,K|splitting,K|free`.
- Test on the two example files in the spec (Figures 7 and 8) before going further.

**4. Implement web construction**
- Greedy merge: for each variable, union live ranges that share at least one program line.
- Watch the subtle rule: if a range *ends* on line L (use) and another *starts* on line L (definition of the same variable, e.g., `i = i + 1`), **fuse them**.

**5. Build the interference graph**
- One node per web. Add edge between webs A and B if they share any program line.
- Mind the edge case from Figure 6: if A *starts* (definition) on the same line that B *ends* (last use), they do **not** interfere.

**6. Implement basic coloring (T2.1) before anything else**
- This is the largest single chunk of points (4.0) and everything else builds on it.
- Follow Figure 9 literally: simplify phase (push degree-`<N` nodes to a stack, spill if stuck), then color phase (pop and assign).
- Try `N` from 1 upward until it succeeds — that gives you the minimum.

**7. Build the CLI skeleton early**
- Both the interactive menu **and** the batch mode: `myProg -b ranges.txt registers.txt allocation.txt`.
- Even a stub version helps — you'll use it for every test.

Once basic allocation works end-to-end on the example inputs, *then* layer on spilling, splitting, and your free algorithm. Leave Doxygen + the presentation for the last week, but write good function comments as you go so docs are quick.

Essential Doxygen commands you'll need for this project:

## File & Class Level

```cpp
/**
 * @file Graph.h
 * @brief Graph data structure for interference graph representation.
 * @author Your Name
 * @date 2026
 */

/**
 * @class RegisterAllocator
 * @brief Performs register allocation via graph coloring heuristics.
 */
```

## Functions / Methods

```cpp
/**
 * @brief Colors the interference graph using at most N colors.
 *
 * Implements the greedy algorithm from Figure 9: simplify phase
 * (push low-degree nodes onto stack) followed by coloring phase.
 *
 * @param graph The interference graph to color.
 * @param N Maximum number of colors (registers).
 * @return true if coloring succeeded, false if spilling was needed.
 *
 * @note Time complexity: O(V^2) where V is the number of webs.
 * @see buildInterferenceGraph()
 */
bool colorGraph(Graph& graph, int N);
```

## Parameters, Return, Complexity

```cpp
/**
 * @brief Merges overlapping live ranges into webs.
 *
 * @param ranges Vector of raw live ranges parsed from input.
 * @return Vector of merged webs, each containing sorted program points.
 *
 * @note Time complexity: O(R * P) where R = number of ranges, P = max program points per range.
 * @throws std::invalid_argument if ranges contain invalid line numbers.
 */
std::vector<Web> buildWebs(const std::vector<LiveRange>& ranges);
```

## Most Used Tags — Cheat Sheet

| Tag | Purpose |
|---|---|
| `@brief` | One-line summary |
| `@param name` | Describe a parameter |
| `@return` | Describe return value |
| `@note` | Extra info (use for **time complexity**) |
| `@see` | Cross-reference another function/class |
| `@throws` / `@exception` | What exceptions it can throw |
| `@todo` | Mark incomplete work |
| `@deprecated` | Mark old code |
| `@file` | Document the file itself |
| `@class` | Document a class |
| `@author` | Author name |
| `@date` | Date |

## Enums & Member Variables

```cpp
/**
 * @enum Algorithm
 * @brief Available register allocation strategies.
 */
enum Algorithm {
    BASIC,      ///< Simple greedy graph coloring
    SPILLING,   ///< Graph coloring with web spilling
    SPLITTING,  ///< Graph coloring with web splitting
    FREE        ///< Custom algorithm
};

class Web {
    std::string varName;         ///< Original variable name
    std::set<int> programPoints; ///< Sorted set of live program lines
    int assignedRegister;        ///< Register index, or -1 if spilled to memory
};
```

## Generating the Docs

```bash
# Generate a default config file
doxygen -g Doxyfile

# Key settings to change in Doxyfile:
#   PROJECT_NAME = "DA Register Allocator"
#   INPUT = src/ include/
#   RECURSIVE = YES
#   EXTRACT_ALL = YES
#   GENERATE_LATEX = NO

# Build the HTML docs
doxygen Doxyfile
# Output lands in html/ folder — open html/index.html
```

The main habit: put a `@brief` + `@param` + `@return` + `@note` (complexity) block on every public function as you write it, and docs generation at the end will be painless.
