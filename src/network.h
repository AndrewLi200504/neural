//#include <Eigen/Core>
#include <iostream>
#include <vector>
#include "layer.h"

class Network {
    public:

    // Adds a layer to the network with # of nodes from previous layer and # of nodes in this new layer
    void add_layer(int input_size, int output_size);

    // Returns the value of the output node given a specific input vector
    std::vector<double> predict(const std::vector<double>& input);

    // Runs the data forward and backwards through the network repeat # of times, changing the weights within layers
    void train(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets,
               int repeats, double learning_rate);

    // // Adds a layer to the network with specified # of nodes containing randomized weights
    // void add_layer(int amt);

    // // Adds a node w/ randomized weight to a specific 0-indexed layer
    // // CONSTRAINT: layer must exist
    // void add_node(int layer);

    // // Returns number of layers in the network
    // int layer_count() const;

    // // Returns the amount of nodes in entire network
    // int total_node_count() const;

    // // Returns number of nodes in specified 0-indexed layer
    // // CONSTRAINT: layer must exist
    // int node_count(int layer) const;

    // // Returns value at a 0-indexed layer, node
    // // CONSTRAINT: layer and node must exist
    // float get_weight(int layer, int node) const;

    // // Gives a specific node a specific weight
    // // CONSTRAINT: layer and node must exist
    // void assign(int layer, int node, float weight);

    private:

    // vector of layers in the network
    std::vector<Layer> layers;

    // // vector (layer) of vectors (nodes in layer)
    // std::vector<std::vector<float>> net;
};