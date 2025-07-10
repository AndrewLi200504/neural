#include "catch_amalgamated.hpp"
#include "../src/layer.h"
// Test empty layer constructor

bool isRandomizedWeights(std::vector<std::vector<double>> weights, int outputSize, int inputSize) {
    for (int i = 0; i < outputSize; i++) {
        for (int j = 0; j < inputSize; j++) {
            if (weights[i][j] > 1 || weights[i][j] < -1) {
                return false;
            }
        }
    }
    return true;
}
bool isRandomizedBiases(std::vector<double> biases, int outputSize) {
    for (int i = 0; i < outputSize; i++) {
        if (biases[i] > 1 || biases[i] < -1) {
            return false;
        }
    }
    return true;

}

TEST_CASE("Empty layer constructor", "[layer]") {
    Layer l = Layer(0, 0);
    REQUIRE(l.getInputSize() == 0);
    REQUIRE(l.getOutputSize() == 0);
}

TEST_CASE("Layer constructor with weights and biases", "[layer]") {
    Layer l = Layer(5, 5);
    int inputSize = l.getInputSize();
    int outputSize = l.getOutputSize();
    std::vector<std::vector<double>> weights = l.getWeights();
    std::vector<double> biases = l.getBiases();
    REQUIRE(isRandomizedWeights(weights, outputSize, inputSize));
    REQUIRE(isRandomizedBiases(biases, outputSize));
}

TEST_CASE("Layer constructor with weights and biases, different dimensions", "[layer]") {
    Layer l = Layer(5, 6);
    int inputSize = l.getInputSize();
    int outputSize = l.getOutputSize();
    std::vector<std::vector<double>> weights = l.getWeights();
    std::vector<double> biases = l.getBiases();
    REQUIRE(isRandomizedWeights(weights, outputSize, inputSize));
    REQUIRE(isRandomizedBiases(biases, outputSize));
}

TEST_CASE("Test activation function", "[layer]") {
    Layer l = Layer(5,5);
    REQUIRE(l.activate(0) ==  Catch::Approx(0.5).epsilon(0.0001));
}