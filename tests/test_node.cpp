#include "catch_amalgamated.hpp"
#include "../src/node.h"

// Test empty node constructor
TEST_CASE("Empty node constructor", "[node]") {
    Node n = Node();
    REQUIRE(n.getBias() == 0);
    REQUIRE(n.getWeights().empty());
}

