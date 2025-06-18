#ifndef NETWORK_H
#define NETWORK_H
#include "network.h"
#include "labeledvector.h"
#include <cmath>
#include <unordered_map>

void Network::add_layer(int input_size, int output_size) {
    Layer layer = Layer(input_size, output_size);
    layers.push_back(layer);
}

std::vector<double> Network::predict(const std::vector<double>& input) {
    std::vector<double> temp = input;

    // Pass the new input through each layer
    for (Layer& layer : layers) {
        temp = layer.forward(temp);
    }

    return temp; // output is either 0 or 1
}

// Overloaded predict method for LabeledVector
std::vector<double> Network::predict(const LabeledVector& labeled_input) {
    return predict(labeled_input.getTFIDF());
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

            for (int k = layers.size() - 1; k >= 0; k--) {
                gradients = layers[k].backward(gradients, learning_rate); // pass the gradients through each layer back
            }
        }

        std::cout << "Loss " << x + 1 << " - Avg loss: " << (eloss/inputs.size()) << std::endl;
    }
}

// New train method that works with LabeledVector
void Network::train(const std::vector<LabeledVector>& labeled_data, int repeats, double learning_rate) {
    // Create a mapping from string classifications to numeric targets
    std::unordered_map<std::string, double> class_to_target;
    std::vector<std::string> unique_classes;
    
    // Find all unique classifications
    for (const auto& data : labeled_data) {
        if (class_to_target.find(data.getClassification()) == class_to_target.end()) {
            unique_classes.push_back(data.getClassification());
        }
    }
    
    std::cout << "Found " << unique_classes.size() << " unique classes: ";
    for (const auto& cls : unique_classes) {
        std::cout << cls << " ";
    }
    std::cout << std::endl;
    
    // For binary classification, map to 0.0 and 1.0
    if (unique_classes.size() == 2) {
        // Sort classes to ensure consistent mapping
        std::sort(unique_classes.begin(), unique_classes.end());
        class_to_target[unique_classes[0]] = 0.0;
        class_to_target[unique_classes[1]] = 1.0;
        std::cout << "Binary classification: " << unique_classes[0] << " -> 0.0, " 
                  << unique_classes[1] << " -> 1.0" << std::endl;
    } else if (unique_classes.size() == 1) {
        // Handle case with only one class
        class_to_target[unique_classes[0]] = 1.0;
    } else {
        // This shouldn't happen with your data
        std::cerr << "ERROR: Expected binary classification but found " 
                  << unique_classes.size() << " classes!" << std::endl;
        return;
    }
    
    // Training loop
    for (int x = 0; x < repeats; x++) {
        double eloss = 0;
        
        // Shuffle data each epoch for better training
        std::vector<int> indices(labeled_data.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);
        
        for (int idx : indices) {
            std::vector<double> output = predict(labeled_data[idx].getTFIDF());

            double prediction = output[0];
            double target = class_to_target[labeled_data[idx].getClassification()];
            
            // Clamp prediction to avoid log(0)
            prediction = std::max(std::min(prediction, 0.9999), 0.0001);

            // Binary cross entropy loss
            double loss = -(target * std::log(prediction) + (1 - target) * std::log(1 - prediction));
            eloss += loss;

            // Gradient for binary cross entropy with sigmoid
            std::vector<double> gradients = {prediction - target};

            // Backpropagate
            for (int k = layers.size() - 1; k >= 0; k--) {
                gradients = layers[k].backward(gradients, learning_rate);
            }
        }

        double avg_loss = eloss / labeled_data.size();
        std::cout << "Epoch " << x + 1 << " - Avg loss: " << avg_loss << std::endl;
        
        // Early stopping if loss becomes very small
        if (avg_loss < 0.001) {
            std::cout << "Early stopping - loss converged" << std::endl;
            break;
        }
    }
    
    // Store the class mapping for future reference
    class_mapping = class_to_target;
}

// Method to get predicted class as string
std::string Network::predict_class(const LabeledVector& labeled_input) {
    std::vector<double> output = predict(labeled_input.getTFIDF());
    double prediction = output[0];
    
    // Find the closest class based on the prediction value
    std::string best_class;
    double min_distance = std::numeric_limits<double>::max();
    
    for (const auto& pair : class_mapping) {
        double distance = std::abs(prediction - pair.second);
        if (distance < min_distance) {
            min_distance = distance;
            best_class = pair.first;
        }
    }
    
    return best_class;
}

// Method to get predicted class as string from raw input
std::string Network::predict_class(const std::vector<double>& input) {
    std::vector<double> output = predict(input);
    double prediction = output[0];
    
    // Find the closest class based on the prediction value
    std::string best_class;
    double min_distance = std::numeric_limits<double>::max();
    
    for (const auto& pair : class_mapping) {
        double distance = std::abs(prediction - pair.second);
        if (distance < min_distance) {
            min_distance = distance;
            best_class = pair.first;
        }
    }
    
    return best_class;
}
#endif