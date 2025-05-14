#include "node.h"
Node::Node() {
    bias = 0.0;
    weights = std::vector<float>();
}

Node::Node(float bias, const std::vector<float>& weights) {
    this->bias = bias;
    this->weights = weights;
}

float Node::getBias() const {
    return bias;
}

std::vector<float> Node::getWeights() const {
    return weights;
}