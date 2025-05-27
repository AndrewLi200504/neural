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
#include "network.h"
#include "labeledvector.h"

// --- Tokenizer ---
std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string word;
    for (char c : text) {
        if (std::isalnum(c) || c == '\'') {
            word += std::tolower(c);
        } else if (!word.empty()) {
            tokens.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) tokens.push_back(word);
    return tokens;
}

// --- Robust CSV parser for review-label pairs ---
std::pair<std::vector<std::string>, std::vector<std::string>> read_reviews_and_labels(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return {{}, {}};
    }

    std::vector<std::string> reviews;
    std::vector<std::string> labels;
    std::string line;
    
    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "Empty file or failed to read header" << std::endl;
        return {{}, {}};
    }

    std::ostringstream current_review;
    bool inside_quoted_field = false;
    
    while (std::getline(file, line)) {
        // Handle completely empty lines
        if (line.empty() && !inside_quoted_field) {
            continue;
        }
        
        // If we're not inside a quoted field, this should be the start of a new record
        if (!inside_quoted_field) {
            if (line.empty() || line[0] != '"') {
                std::cerr << "Warning: Expected quoted field, got: " << line.substr(0, std::min(50, (int)line.length())) << std::endl;
                continue;
            }
            // Start new review
            current_review.str("");
            current_review.clear();
            inside_quoted_field = true;
        }
        
        // Process the line character by character
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (inside_quoted_field) {
                if (c == '"') {
                    // Check if this is an escaped quote (followed by another quote)
                    if (i + 1 < line.length() && line[i + 1] == '"') {
                        current_review << '"';  // Add single quote to content
                        ++i; // Skip the next quote
                    } else {
                        // This might be the end of the quoted field
                        // Check if followed by comma
                        if (i + 1 < line.length() && line[i + 1] == ',') {
                            // End of review found
                            std::string review_text = current_review.str();
                            std::string label = line.substr(i + 2);
                            
                            // Clean up label
                            label.erase(0, label.find_first_not_of(" \t\r\n"));
                            label.erase(label.find_last_not_of(" \t\r\n") + 1);
                            
                            if (!review_text.empty()) {
                                reviews.push_back(review_text);
                                labels.push_back(label);
                            }
                            
                            inside_quoted_field = false;
                            break; // Done with this line
                        } else {
                            // Quote not followed by comma, treat as regular quote
                            current_review << c;
                        }
                    }
                } else {
                    current_review << c;
                }
            }
        }
        
        // If still inside quoted field, add newline (multi-line review)
        if (inside_quoted_field) {
            current_review << '\n';
        }
    }
    
    // Handle case where file ends while still in quoted field
    if (inside_quoted_field) {
        std::cerr << "Warning: File ended while still parsing a quoted field" << std::endl;
    }
    
    std::cout << "Successfully parsed " << reviews.size() << " reviews with labels." << std::endl;
    return {reviews, labels};
}

int main() {
    // Get reviews and labels separately
    auto [review_texts, classLabels] = read_reviews_and_labels("data/TestReviews.csv");
    
    if (review_texts.empty()) {
        std::cerr << "No reviews loaded. Exiting." << std::endl;
        return 1;
    }
    
    // Tokenize reviews
    std::vector<std::vector<std::string>> reviews;
    std::unordered_map<std::string, double> vocabulary;
    
    for (const std::string& review_text : review_texts) {
        std::vector<std::string> words = tokenize(review_text);
        reviews.push_back(words);
        
        // Count unique words for IDF calculation
        std::unordered_set<std::string> uniqueWordsInReview;
        for (const std::string& word : words) {
            uniqueWordsInReview.insert(word);
        }
        for (const std::string& word : uniqueWordsInReview) {
            vocabulary[word]++;
        }
    }

    std::cout << "Parsed " << classLabels.size() << " labeled reviews." << std::endl;
    std::cout << "Sample labels: ";
    for (size_t i = 0; i < std::min((size_t)5, classLabels.size()); ++i) {
        std::cout << classLabels[i] << " ";
    }
    std::cout << std::endl;

    int numReviews = reviews.size();
    
    // Calculate IDF values
    for (auto& entry : vocabulary) {
        vocabulary[entry.first] = log((double)numReviews / entry.second);
    }

    // Build TF-IDF vectors
    std::vector<LabeledVector> labeledvectors;
    for (size_t i = 0; i < reviews.size(); ++i) {
        const auto& review = reviews[i];
        std::unordered_map<std::string, double> freqMap;
        for (const auto& word : review) {
            freqMap[word]++;
        }

        std::vector<double> tfidf;
        for (const auto& [word, freq] : freqMap) {
            double tf = freq / review.size();
            double idf = vocabulary[word];
            tfidf.push_back(tf * idf);
        }

        labeledvectors.emplace_back(tfidf, classLabels[i]);
    }

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
   
    // Setup the network
    Network network;
    network.add_layer(vocabulary.size(), 50);  // Input size should match vocabulary size
    network.add_layer(50, 50);
    network.add_layer(50, 50);
    network.add_layer(50, 1);

    return 0;
}