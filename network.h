//#include <Eigen/Core>
#include <iostream>
#include <vector>

class Network {
    public:
    // Constructs a network w/ one layer and a number of nodes
    Network(std::vector<float> initLayer);

    // Adds a layer to the network with specified # of nodes containing randomized weights
    void add_layer(int amt);

    // Adds a node w/ randomized weight to a specific 0-indexed layer
    // CONSTRAINT: layer must exist
    void add_node(int layer);

    // Returns number of layers in the network
    int layer_count() const;

    // Returns the amount of nodes in entire network
    int total_node_count() const;

    // Returns number of nodes in specified 0-indexed layer
    // CONSTRAINT: layer must exist
    int node_count(int layer) const;

    // Returns value at a 0-indexed layer, node
    // CONSTRAINT: layer and node must exist
    float get_weight(int layer, int node) const;

    // Gives a specific node a specific weight
    // CONSTRAINT: layer and node must exist
    void assign(int layer, int node, float weight);

    private:
    // vector (layer) of vectors (nodes in layer)
    std::vector<std::vector<float>> net;
};