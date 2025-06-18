#include "labeledvector.h"
#include "layer.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <limits>
#include <algorithm>    // for sort, shuffle
#include <numeric>      // for iota
#include <random>       // for random_device, mt19937

class Network {
private:
    std::vector<Layer> layers;
    std::unordered_map<std::string, double> class_mapping; // Maps string classifications to numeric targets

public:
    void add_layer(int input_size, int output_size);
    
    // Original predict method
    std::vector<double> predict(const std::vector<double>& input);
    
    // Overloaded predict method for LabeledVector
    std::vector<double> predict(const LabeledVector& labeled_input);
    
    // Original train method
    void train(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets, int repeats, double learning_rate);
    
    // New train method for LabeledVector
    void train(const std::vector<LabeledVector>& labeled_data, int repeats, double learning_rate);
    
    // Methods to get predicted class as string
    std::string predict_class(const LabeledVector& labeled_input);
    std::string predict_class(const std::vector<double>& input);
    
    // Getter for class mapping (optional, for debugging)
    std::unordered_map<std::string, double> get_class_mapping() const { return class_mapping; }
};

