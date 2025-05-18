#include "network.h"

void Network::add_layer(int input_size, int output_size) {
    Layer layer = Layer(input_size, output_size);
    layers.push_back(layer);
}

double Network::predict(const std::vector<double>& input) {
    
}

void Network::train(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets, int repeats, double learning_rate) {

}

// Network::Network(std::vector<float> initLayer) {
//     net.push_back(initLayer);
// }

// void Network::add_layer(int amt) {
//     srand(time(0));
//     net.resize(net.size() + 1);
//     for (int i = 0; i < amt; i++) {
//         float w = static_cast <float>(rand()) / static_cast <float>(RAND_MAX); // normalize to [0, 1]
//         net[net.size() - 1].push_back(w);
//     }
// }

// void Network::add_node(int layer) {
//     srand(time(0));
//     float w = static_cast <float>(rand()) / static_cast <float>(RAND_MAX); // normalize to [0, 1]
//     net[layer].push_back(w);
// }

// int Network::layer_count() const {
//     return net.size();
// }

// int Network::total_node_count() const {
//     int total = 0;
//     for (auto layer : net) {
//         for (auto node : layer) {
//             total++;
//         }
//     }
//     return total;
// }

// int Network::node_count(int layer) const {
//     return net[layer].size();
// }

// float Network::get_weight(int layer, int node) const {
//     return net[layer][node];
// }

// void Network::assign(int layer, int node, float weight) {
//     net[layer][node] = weight;
// }
