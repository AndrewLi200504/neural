#include "catch_amalgamated.hpp"
#include "../src/layer.h"
// Test empty layer constructor
TEST_CASE("Empty layer constructor", "[layer]") {
    Layer l = Layer(0, 0);
    REQUIRE(l.getInputSize() == 0);
    REQUIRE(l.getOutputSize() == 0);
}
