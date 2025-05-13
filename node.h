#include <iostream>
#include <vector>
class Node {
    public:
    // Creates a default node
    Node();

    // Creates a node with a bias and a list of weights
    Node(float bias, const std::vector<float>& weights);

    // Returns bias
    float getBias() const;

    // Returns list of weights
    std::vector<float> getWeights() const;

    private:
    // gi
    
     float bias;
     std::vector<float> weights;
};