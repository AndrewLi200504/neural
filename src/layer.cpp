#include "layer.h"

Layer::Layer(int in_size, int out_size) {
    input_size = in_size;
    output_size = out_size;

    // Create a matrix of randomized weights from 0 - 1
    for (int i = 0; i < output_size; i++) {
        biases.push_back((double)rand()/(double)RAND_MAX); // Create random biases from 0 - 1
        std::vector<double> temp;
        for (int j = 0; j < input_size; j++) {
            double random = ((double)rand()/(double)RAND_MAX);
            temp.push_back(random);
        }
        weights.push_back(temp);
    }

}

double Layer::activate(double input) {
    return 1/(1+exp(-input));
}

std::vector<double> Layer::forward(const std::vector<double>& input) {
    std::vector<double> output(output_size, 0);

    for (int i = 0; i < output_size; i++) {
        for (int j = 0; j < input_size; j++) {
            output[i] += weights[i][j]*input[j]; // sum of all the weight*inputs for one node in this layer
        }
        output[i]+=biases[i];
        output[i] = activate(output[i]);
    }
    return output;
}

std::vector<double> Layer::backward(const std::vector<double>& grad_output, double learning_rate) {
    std::vector<double> grad_input(input_size, 0);

    for (int i = 0; i < output_size; i++) {

        for (int j = 0; j < input_size; j++) {
            
        }
    }
}