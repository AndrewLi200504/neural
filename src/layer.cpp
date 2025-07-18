#include "layer.h"
#include <cmath>
#include <random>

Layer::Layer(int in_size, int out_size) {
    input_size = in_size;
    output_size = out_size;
    std::mt19937 gen(std::random_device{}());
    double fan_in = input_size;
    double fan_out = output_size;
    double xavier_bound = sqrt(6.0 / (fan_in + fan_out));
    std::uniform_real_distribution<double> dist(-xavier_bound, xavier_bound);
    // Create a matrix of randomized weights from 0 - 1
    for (int i = 0; i < output_size; i++) {
        biases.push_back(dist(gen)); // Create random biases from 0 - 1
        std::vector<double> temp;
        
        for (int j = 0; j < input_size; j++) {
            double random = dist(gen);
            temp.push_back(random);
        }
        weights.push_back(temp);
    }

}

double Layer::activate(double input) {
    return 1/(1+exp(-input));
}

std::vector<double> Layer::forward(const std::vector<double>& input) {
    last_input = input;
    std::vector<double> output(output_size, 0);

    for (int i = 0; i < output_size; i++) {
        for (int j = 0; j < input_size; j++) {
            output[i] += weights[i][j]*input[j]; // sum of all the weight*inputs for one node in this layer
        }
        output[i]+=biases[i];
        output[i] = activate(output[i]);
    }
    last_output = output;
    return output;
}

std::vector<double> Layer::backward(const std::vector<double>& grad_output, double learning_rate) {
    std::vector<double> grad_input(input_size, 0);

    for (int i = 0; i < output_size; i++) {
        double activation_deriv = last_output[i] * (1 - last_output[i]); // put last input through derivative of activation function (sigmoid in this case)
        double delta = grad_output[i] * activation_deriv; // change in loss to change in this input (dL/dz)
        for (int j = 0; j < input_size; j++) {
            grad_input[j] += delta*weights[i][j]; // gradient for each input node; sum of delta * weights that came from that node
            weights[i][j] -= learning_rate*delta*last_input[j]; // adjust the weight with learning rate * dL/dw
        }
        biases[i] -= learning_rate*delta; // adjust bias too
    }
    return grad_input;
}

int Layer::getInputSize() const {
    return input_size;
}

int Layer::getOutputSize() const {
    return output_size;
}

std::vector<std::vector<double>> Layer::getWeights() const {
    return weights;
}

std::vector<double> Layer::getBiases() const {
    return biases;
}


