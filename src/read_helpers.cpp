#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "read_helpers.h"

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string word;
    for (char c : text) {
        if (std::isalnum(c) || c == '\'' || c == '-') { // checks for word letters + the characters ' and - in words
            word += std::tolower(c);
        } else if (!word.empty()) {
            tokens.push_back(word); // add finished word to tokens
            word.clear(); // reset for grabbing next word
        }
    }
    if (!word.empty()) tokens.push_back(word); // add final word to tokens
    return tokens;
}

std::pair<std::vector<std::string>, std::vector<std::string>> read_reviews_and_labels(const std::string& filepath) {
    std::ifstream file(filepath); // parse file
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

    std::ostringstream current_review; // string object
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
                            label.erase(label.find_last_not_of(" \t\r\n") + 1); // removes unnecessary characters from start/end of label
                            
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
                    current_review << c; // regular character
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

std::vector<std::vector<std::string>> tokenize_reviews(const std::vector<std::string>& review_texts) {
    std::vector<std::vector<std::string>> reviews;
    for (const std::string& review_text : review_texts) {
        std::vector<std::string> words = tokenize(review_text);
        reviews.push_back(words);
    }
    return reviews;
}

std::unordered_map<std::string, double> build_vocabulary(const std::vector<std::vector<std::string>>& reviews) {
    std::unordered_map<std::string, double> vocabulary; // key: word   value: count
    
    // filling vocabulary with # of reviews containing each unique word
    for (const auto& review : reviews) {
        // Count unique words for IDF calculation
        std::unordered_set<std::string> uniqueWordsInReview;
        for (const std::string& word : review) {
            uniqueWordsInReview.insert(word); // using set to get rid of duplicates
        }
        for (const std::string& word : uniqueWordsInReview) {
            vocabulary[word]++; // if this word appeared in the review, increase count in vocabulary
        }
    }
    
    // Calculate IDF values
    int numReviews = reviews.size();
    for (auto& entry : vocabulary) {
        vocabulary[entry.first] = log((double)numReviews / entry.second);
    }
    
    return vocabulary;
}

// std::vector<LabeledVector> build_tfidf_vectors(const std::vector<std::vector<std::string>>& reviews,
//                                                const std::vector<std::string>& classLabels,
//                                                const std::unordered_map<std::string, double>& vocabulary) {
//     std::vector<LabeledVector> labeledvectors;
//     std::vector<std::vector<double>> inputs;
//     std::vector<std::vector<double>> targets;
    
//     for (size_t i = 0; i < reviews.size(); ++i) {
//         const auto& review = reviews[i];
//         std::unordered_map<std::string, double> freqMap;
//         for (const auto& word : review) {
//             freqMap[word]++; // get count of every word in a review
//         }

//         std::vector<double> tfidf;
//         for (const auto& [word, freq] : freqMap) {
//             double tf = freq / review.size(); // times word appears in review / total words in review
//             double idf = vocabulary.at(word); // log (# of reviews / # of reviews where word appears)
//             tfidf.push_back(tf * idf);
//         }
    
//         labeledvectors.emplace_back(tfidf, classLabels[i]); // makes a labeled vector and puts it at the end of the vector
//     }
    
//     return labeledvectors;
// }

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> build_tfidf_vectors(const std::vector<std::vector<std::string>>& reviews,
                                               const std::vector<std::string>& classLabels,
                                               const std::unordered_map<std::string, double>& vocabulary) {
    // std::vector<LabeledVector> labeledvectors;
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> targets;
    
    for (size_t i = 0; i < reviews.size(); ++i) {
        const auto& review = reviews[i];
        std::unordered_map<std::string, double> freqMap;
        for (const auto& word : review) {
            freqMap[word]++; // get count of every word in a review
        }

        std::vector<double> tfidf;
        for (const auto& [word, freq] : freqMap) {
            double tf = freq / review.size(); // times word appears in review / total words in review
            double idf = vocabulary.at(word); // log (# of reviews / # of reviews where word appears)
            tfidf.push_back(tf * idf);
        }

        inputs.push_back(tfidf);
        targets.emplace_back(1, std::stod(classLabels[i])); // doing this in case targets become more than one value
        // labeledvectors.emplace_back(tfidf, classLabels[i]); // makes a labeled vector and puts it at the end of the vector
    }
    
    return std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>{inputs, targets};
    // return labeledvectors;
}