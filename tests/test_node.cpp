#include "catch_amalgamated.hpp"
#include "../src/node.h"
// Test empty node constructor
TEST_CASE("Empty node constructor", "[node]") {
    Node n = Node();
    REQUIRE(n.getBias() == 0);
    REQUIRE(n.getWeights().empty());
}

// Test constructor for node

TEST_CASE("Node constructor", "[node]") {
    Node n = Node(0.01, {0.01, 0.02, 0.03});
    REQUIRE(n.getBias() == 0.01f);
    for (int i = 0; i < 3; i++) {
        REQUIRE(n.getWeights()[i] == (i + 1) * 0.01f);
    }
}
