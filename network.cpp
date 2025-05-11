#include "network.h"

Network::Network() {
    net = {{1}};
}

void Network::add_layer() {
    int size = net.size();
    net.resize(size+1);
}

void Network::add_layer(int amt) {
    int size = net.size();
    net.resize(size+amt);
}

void Network::add_node(int layer) {
    int size = net[layer].size();
    net[layer].resize(size+1);
}

int Network::layers() const {
    return net.size();
}

int Network::nodes(int layer) const {
    return net[layer].size();
}

int Network::value(int layer, int node) const {
    return net[layer][node];
}
