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
#include <climits>

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

// Helper function to find the amount of times a word appears in a string of words
int word_frequency(const std::string& word, const std::vector<std::string>& words) {
    int count = 0;
    for (std::string w : words) {
        if (word.compare(w) == 0) { // if words are equal, add to count
            count++;
        }
    }
    return count;
}

// Helper function to find total # of reviews that contain a given word
int doc_frequency(const std::string& word, const std::vector<std::vector<std::string>>& reviews) {
    int count = 0;
    for (std::vector<std::string> r : reviews) {
        if (word_frequency(word, r) > 0) {
            count++;
        }
    }
    return count;
}

int main() {
    std::ifstream file("data/TestReviews.csv");  // Relative path
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Create vocabulary map: word -> index
    std::unordered_map<std::string, int> vocabulary;

    std::vector<std::vector<std::string>> reviews;
    
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

        reviews.push_back(words);
        
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

    // Get all unique words in alphabetical order
    std::vector<std::string> alpha_words;
    for (const auto& entry : vocabulary) {
        alpha_words.push_back(entry.first);
    }
    sort(alpha_words.begin(), alpha_words.end());

    std::vector<std::vector<double>> samples;
    std::vector<std::vector<double>> results;

    // !!! NEEDS TO BE OPTIMIZED! (cut down to 200 reviews first; use hash map?)
    int ra = 0;
    // for all 4148 reviews, create an input vector of their weights using tf-idf
    for (unsigned long i = 0; i < reviews.size(); i++) {
        std::cout << i << std::endl;
        std::vector<double> rev(alpha_words.size(), 0); // frequencies of each review that will be part of the input vector
        for (unsigned long j = 0; j < reviews[i].size(); j++) { // go through every word in each review (will result in repeats but shouldnt affect final outcome)
            // find corresponding index of this word in the alpha list
            auto w = find(alpha_words.begin(), alpha_words.end(), reviews[i][j]);
            int index = w - alpha_words.begin();

            // ra++;
            // std::cout << ra << std::endl;
            // use term frequency/inverse document frequency to calculate frequency of given word

            // # of times the word appears in the review / total # of words in the review
            double tf = (double)word_frequency(reviews[i][j], reviews[i])/(double)reviews[i].size();
            // log(total # of reviews / # of reviews that contain the word)
            double idf = (double)log((double)reviews.size()/(double)doc_frequency(reviews[i][j], reviews));
            rev[index] = tf*idf; // multiply values as tf-idf does
        }
        samples.push_back(rev);
    }

    // Grab the target value for each review (where are the target values stored?)
    

    // network with 19321 inputs; 3 inner layers (50 outputs from those layers, each) and 1 output node
    Network network;
    network.add_layer(19321, 50);
    network.add_layer(50, 50);
    network.add_layer(50, 50);
    network.add_layer(50, 1);

    return 0;
}
    