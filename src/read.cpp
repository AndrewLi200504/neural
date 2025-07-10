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
#include <random>
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

// Function to evaluate model performance
void evaluate_model(Network& network, const std::vector<LabeledVector>& test_data) {
    int correct_predictions = 0;
    int total_predictions = test_data.size();
    
    std::cout << "\nEvaluating model performance..." << std::endl;
    
    for (const auto& test_sample : test_data) {
        std::string predicted_class = network.predict_class(test_sample);
        std::string actual_class = test_sample.getClassification();
        
        if (predicted_class == actual_class) {
            correct_predictions++;
        }
    }
    
    double accuracy = static_cast<double>(correct_predictions) / total_predictions * 100.0;
    std::cout << "Accuracy: " << correct_predictions << "/" << total_predictions 
              << " (" << accuracy << "%)" << std::endl;
}

int main() {
    std::cout << "Step 1: Reading reviews..." << std::endl;
    
    // Read reviews and labels from file
    auto [review_texts, classLabels] = read_reviews_and_labels("data/TestReviews.csv");
    
    if (review_texts.empty()) {
        std::cerr << "No reviews loaded. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << "Step 2: Successfully loaded " << review_texts.size() << " reviews" << std::endl;
    std::cout << "Sample review length: " << review_texts[0].length() << " characters" << std::endl;
    
    std::cout << "Step 3: Starting tokenization..." << std::endl;
    
    // Tokenize all reviews - THIS MIGHT BE THE PROBLEM
    std::vector<std::vector<std::string>> reviews;
    try {
        reviews = tokenize_reviews(review_texts);
        std::cout << "Step 4: Tokenization successful" << std::endl;
        std::cout << "Sample tokenized review size: " << reviews[0].size() << " words" << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "CRASH: Memory allocation failed during tokenization!" << std::endl;
        return 1;
    }
    
    // Check total word count
    size_t total_words = 0;
    for (const auto& review : reviews) {
        total_words += review.size();
    }
    std::cout << "Total words across all reviews: " << total_words << std::endl;
    
    std::cout << "Step 5: Building vocabulary..." << std::endl;
    
    // Build vocabulary with IDF values
    std::unordered_map<std::string, double> vocabulary;
    try {
        vocabulary = build_vocabulary(reviews); // Use your limited function
        std::cout << "Step 6: Vocabulary built successfully" << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "CRASH: Memory allocation failed during vocabulary building!" << std::endl;
        return 1;
    }
    
    std::cout << "Step 7: Building TF-IDF vectors..." << std::endl;
    
    // Build TF-IDF vectors
    std::vector<LabeledVector> labeledvectors;
    try {
        labeledvectors = build_tfidf_vectors(reviews, classLabels, vocabulary);
        std::cout << "Step 8: TF-IDF vectors built successfully" << std::endl;
        std::cout << "First vector size: " << labeledvectors[0].getTFIDF().size() << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "CRASH: Memory allocation failed during TF-IDF vector building!" << std::endl;
        return 1;
    }
    std::cout << "Step 8.5: Shuffling data..." << std::endl;

    // Shuffle the data to mix up the classes
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(labeledvectors.begin(), labeledvectors.end(), g);
    
    // Print sample data for verification
    print_sample_data(classLabels, vocabulary);
    
    std::cout << "Step 9: Setting up network..." << std::endl;
    
    // Setup the neural network with actual vocabulary size
    Network network;
    try {
        network = setup_network(vocabulary.size());
        std::cout << "Step 10: Network setup successful" << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "CRASH: Memory allocation failed during network setup!" << std::endl;
        return 1;
    }
    
    // If we get here, we can try training
    std::cout << "All setup completed successfully!" << std::endl;
    
     // Split data into training and testing sets (80/20 split)
    std::cout << "Step 11: Splitting data for training and testing..." << std::endl;
    size_t train_size = static_cast<size_t>(labeledvectors.size() * 0.8);
    
    std::vector<LabeledVector> train_data(labeledvectors.begin(), labeledvectors.begin() + train_size);
    std::vector<LabeledVector> test_data(labeledvectors.begin() + train_size, labeledvectors.end());
    
    std::cout << "Training set size: " << train_data.size() << std::endl;
    std::cout << "Test set size: " << test_data.size() << std::endl;
    
    // Train the network
    std::cout << "\nStep 12: Starting training..." << std::endl;
    std::cout << "This may take a while depending on your dataset size..." << std::endl;
    
    try {
        // Train for 10 epochs with learning rate of 0.01
        network.train(train_data, 20, 0.01);
        std::cout << "Step 13: Training completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during training: " << e.what() << std::endl;
        return 1;
    }
    
    // Evaluate the trained model
    std::cout << "\nStep 14: Evaluating model performance..." << std::endl;
    evaluate_model(network, test_data);
    
    // Test with a few sample predictions
    std::cout << "\nStep 15: Sample predictions:" << std::endl;
    for (int i = 0; i < std::min(500, static_cast<int>(test_data.size())); i++) {
        std::string predicted = network.predict_class(test_data[i]);
        std::string actual = test_data[i].getClassification();
        std::vector<double> raw_output = network.predict(test_data[i]);
        
        std::cout << "Sample " << i + 1 << ":" << std::endl;
        std::cout << "  Predicted: " << predicted << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        std::cout << "  Raw output: " << raw_output[0] << std::endl;
        std::cout << "  Correct: " << (predicted == actual ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "Training and evaluation completed!" << std::endl;
    
    return 0;
}
