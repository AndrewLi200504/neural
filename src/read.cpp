#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cmath>
#include "read_helpers.h"


void print_sample_data(const std::vector<std::string>& classLabels,
                      const std::unordered_map<std::string, double>& vocabulary) {
    std::cout << "Parsed " << classLabels.size() << " labeled reviews." << std::endl;
    std::cout << "Sample labels: ";
    for (size_t i = 0; i < std::min((size_t)5, classLabels.size()); ++i) {
        std::cout << classLabels[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Vocabulary size: " << vocabulary.size() << " unique words" << std::endl;

    // Print first 10 vocabulary items
    std::cout << "Sample vocabulary (first 10 items):" << std::endl;
    int count = 0;
    for (const auto& entry : vocabulary) {
        if (count++ < 10) {
            std::cout << "  " << entry.first << " -> " << entry.second << std::endl;
        } else {
            break;
        }
    }
}

Network setup_network(size_t vocabulary_size) {
    Network network;
    network.add_layer(vocabulary_size, 50);  // Input size should match vocabulary size
    network.add_layer(50, 50);
    network.add_layer(50, 50);
    network.add_layer(50, 1);
    return network;
}

int main() {
    // Read reviews and labels from file
    auto [review_texts, classLabels] = read_reviews_and_labels("data/TestReviews.csv");
    
    if (review_texts.empty()) {
        std::cerr << "No reviews loaded. Exiting." << std::endl;
        return 1;
    }
    
    // Tokenize all reviews
    std::vector<std::vector<std::string>> reviews = tokenize_reviews(review_texts);
    
    // Build vocabulary with IDF values
    std::unordered_map<std::string, double> vocabulary = build_vocabulary(reviews);
    
    // Build TF-IDF vectors
    // std::vector<LabeledVector> labeledvectors = build_tfidf_vectors(reviews, classLabels, vocabulary);

    std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> labeledvectors = build_tfidf_vectors(reviews, classLabels, vocabulary);
    
    // Print sample data for verification
    print_sample_data(classLabels, vocabulary);
    
    // Setup the neural network
    Network network = setup_network(vocabulary.size());
    // network.train(labeledvectors.first, labeledvectors.second, 5, 0.01);
    return 0;
}
