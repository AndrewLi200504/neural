//#include <Eigen/Core>
#include <iostream>
#include <vector>

class Network {
    public:
    // Constructor for a node network with 1 node and 1 layer
    Network();

    // Adds a layer to the network
    void add_layer();

    // Adds a number of layers to the network
    void add_layer(int amt);

    // Adds a node to a specific layer
    void add_node(int layer);

    // Returns number of layers in the network
    int layers() const;

    // Returns number of nodes in specified layer
    int nodes(int layer) const;

    // Returns value at a layer, node
    int value(int layer, int node) const;

    private:
    // vector (layer) of vectors (nodes in layer)
    std::vector<std::vector<int>> net;
};