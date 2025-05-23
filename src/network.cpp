#include "network.h"

void Network::add_layer(int input_size, int output_size) {
    Layer layer = Layer(input_size, output_size);
    layers.push_back(layer);
}

std::vector<double> Network::predict(const std::vector<double>& input) {
    std::vector<double> temp = input;

    // Pass the new input through each layer
    for (Layer layer : layers) {
        temp = layer.forward(temp);
    }

    return temp; // output is either 0 or 1
}

void Network::train(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets, int repeats, double learning_rate) {
    for (int x = 0; x < repeats; x++) {
        double eloss = 0;
        for (int i = 0; i < inputs.size(); i++) {
            std::vector<double> output = predict(inputs[i]); // Train on each data in the set

            double prediction = output[0]; // output is only one value
            double target = targets[i][0];
            prediction = std::min(std::max(prediction, 1e-7), 1.0 - 1e-7); // avoid log(0)

            // This is optional; only to see loss
            double loss = -(target*std::log(prediction)+(1-target)*std::log(1-prediction)); // binary cross entropy formula
            eloss+=loss;

            std::vector<double> gradients = {prediction - target}; //derivative of binary cross entropy with sigmoid output

            // std::vector<double> gradients(output.size());
            // for (int j = 0; j < output.size(); j++) {
            //     gradients[j] = output[j] - targets[i][j]; // take prediction - target for derivative of the mean square error
            //     // the derivative isnt *2 here because Loss = 1/2 (predict - target)^2 so it cancels out
            // }

            for (int k = layers.size() - 1; k >= 0; k--) {
                gradients = layers[k].backward(gradients, learning_rate); // pass the gradients through each layer back
            }
        }

        std::cout << "Loss " << repeats + 1 << " - Avg loss: " << (eloss/inputs.size()) << std::endl;
    }
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
