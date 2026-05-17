# Project Setup

## Team and TP
- TP number: TP2
- Group size: 3 members
- C++ standard: C++17
- Build system: CMake

## Folder layout
- `src/` — implementation source files
- `include/` — public headers and data structures
- `tests/` — test harnesses and validation code
- `docs/` — project documentation and setup notes
- `datasets/` — example inputs and sample cases

## Build instructions
1. Create a build directory:
   ```bash
   mkdir -p build
   ```
2. Configure with CMake:
   ```bash
   cmake -S . -B build
   ```
3. Build the project and tests:
   ```bash
   cmake --build build
   ```
4. Run the parser test suite:
   ```bash
   cd build
   ctest -R ParserTest
   ```

## Current repository decisions
- Use the TP graph class from lecture materials as the core graph data structure.
- Extend the graph vertex with a `disabled` flag to support the coloring stack reduction phase.
- Keep web metadata outside the graph via the `Web` structure in `include/structures/Structures.h`.
- Use `Parser::parseLiveRanges()` and `Parser::parseRegisterConfig()` as the first parser entry points.

## Notes
- The parser test harness validates the example files in `datasets/basic/ranges/` and `datasets/basic/registers/`.
- `Graph.h` now initializes all auxiliary fields and supports runtime disabling of nodes.
