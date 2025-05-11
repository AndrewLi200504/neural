#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
//#include <Eigen/Core>
#include "network.h"

// Helper function to clean and tokenize text
std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string word;
    
    for (char c : text) {
        // If character is alphanumeric or apostrophe (for contractions), add to current word
        if (std::isalnum(c) || c == '\'') {
            word += std::tolower(c);
        } 
        // If we hit a delimiter and have a word, add it to tokens
        else if (!word.empty()) {
            tokens.push_back(word);
            word.clear();
        }
    }
    
    // Add the last word if there is one
    if (!word.empty()) {
        tokens.push_back(word);
    }
    
    return tokens;
}

int main() {
    std::ifstream file("data/TestReviews.csv");  // Relative path
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Create vocabulary map: word -> index
    std::unordered_map<std::string, int> vocabulary;
    
    // Skip header
    std::string header;
    std::getline(file, header);
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string review;
        std::string classLabel;
        
        // Read the review text (which may contain commas inside quotes)
        if (line.front() == '"') {
            // Find the closing quote and the last comma
            size_t lastQuote = line.find_last_of('"');
            if (lastQuote != std::string::npos && lastQuote + 1 < line.length()) {
                review = line.substr(1, lastQuote - 1);  // Remove the quotes
                classLabel = line.substr(lastQuote + 2); // +2 to skip quote and comma
            }
        } else {
            // Simple case: no quotes
            std::getline(ss, review, ',');
            std::getline(ss, classLabel);
        }
        
        // Tokenize the review
        std::vector<std::string> words = tokenize(review);
        
        // Add words to vocabulary
        for (const std::string& word : words) {
            if (vocabulary.find(word) == vocabulary.end()) {
                vocabulary[word] = 1;
            } else {
                vocabulary[word]++;
            }
        }
    }
    
    // Print vocabulary statistics
    std::cout << "Vocabulary size: " << vocabulary.size() << " unique words\n";
    
    // Example: print first 10 words and their indices
    int count = 0;
    for (const auto& entry : vocabulary) {
        if (count < 10) {
            std::cout << entry.first << " -> " << entry.second << std::endl;
            count++;
        } else {
            break;
        }
    }
    
    file.close();

    // Eigen::Matrix3f m1;
    // m1 << 1, 2, 3,
    //       4, 5, 6,
    //       7, 8, 9;
    // std::cout << m1 << std::endl;
    // m1.resize(4,4);
    // std::cout << m1 << std::endl;

    Network neural = Network();

    neural.add_layer(19320);

    std::cout << neural.value(0,0) << std::endl;

    return 0;
}