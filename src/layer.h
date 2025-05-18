#include <iostream>
#include <vector>
class Layer {
    public:
    // Creates a default node
    Layer(int in_size, int out_size);

    // activation function
    double activate(double input);

    // forward feeding algorithm
    std::vector<double> forward(const std::vector<double>& input);
    
    // backprop algorithm
    std::vector<double> backward(const std::vector<double>& grad_output, double learning_rate);

    private:
    // # of nodes before this layer
    int input_size;
    // # of nodes after this layer
    int output_size;

    // matrix of weights of size output x input ( [output][input] )
    // each node in the layer will have their own set of weights coming from the previous layer
    std::vector<std::vector<double>> weights;
    // vector of biases for the output
    std::vector<double> biases;
    
};