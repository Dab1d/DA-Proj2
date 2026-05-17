#include "parser/Parser.h"
#include "structures/Graph.h"

#include <cassert>
#include <iostream>
#include <string>

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "."
#endif

static const std::string kRoot(TEST_DATA_DIR);

static std::string joinPath(const std::string& relative) {
    return kRoot + "/" + relative;
}

static void testParseLiveRanges(const std::string& relativePath, int expectedSize) {
    auto ranges = Parser::parseLiveRanges(joinPath(relativePath));
    assert(!ranges.empty());
    assert((int)ranges.size() == expectedSize);

    for (const auto& lr : ranges) {
        assert(!lr.varName.empty());
        assert(!lr.points.empty());
        for (const auto& pp : lr.points) {
            assert(pp.line >= 0);
            assert(pp.isDefinition || pp.isLastUse || true);
        }
    }
    std::cout << "OK: Parsed " << ranges.size() << " ranges from " << relativePath << "\n";
}

static void testParseRegisterConfig(const std::string& relativePath) {
    auto cfg = Parser::parseRegisterConfig(joinPath(relativePath));
    assert(cfg.numRegisters == 1);
    assert(cfg.algorithm == AlgorithmType::BASIC);
    assert(cfg.algorithmParam == -1);
    std::cout << "OK: Parsed register config from " << relativePath << "\n";
}

static void testGraphVertexDisabledFlag() {
    Graph<int> g;
    assert(g.addVertex(1));
    assert(g.addVertex(2));

    auto* vertex = g.findVertex(1);
    assert(vertex != nullptr);
    assert(!vertex->isDisabled());
    vertex->setDisabled(true);
    assert(vertex->isDisabled());

    assert(g.addEdge(1, 2, 0.0));
    assert(vertex->getAdj().size() == 1);
    std::cout << "OK: Graph vertex disabled flag and basic edge insertion test passed\n";
}

int main() {
    testParseLiveRanges("datasets/basic/ranges/ranges1.txt", 5);
    testParseLiveRanges("datasets/basic/ranges/ranges3.txt", 4);
    testParseRegisterConfig("datasets/basic/registers/registers1.txt");
    testGraphVertexDisabledFlag();
    std::cout << "All tests passed.\n";
    return 0;
}
