#include <vector>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <algorithm>
#include "labeledvector.h"
#include "network.h"

// Tokenizer
// RETURNS: vector of words separated from a string of text
std::vector<std::string> tokenize(const std::string& text);

// Robust CSV parser for review-label pairs
// RETURNS: pair of vector strings containing the reviews and respective labels
std::pair<std::vector<std::string>, std::vector<std::string>> read_reviews_and_labels(const std::string& filepath);

// Tokenize each review
// RETURNS: 2D vector of all reviews from the data
std::vector<std::vector<std::string>> tokenize_reviews(const std::vector<std::string>& review_texts);
        
// Build vocabulary
// RETURNS: map of word and their corresponding # of appearance in all the reviews
std::unordered_map<std::string, double> build_vocabulary(
    const std::vector<std::vector<std::string>>& reviews
);
// Build TF-IDF vectors
// RETURNS: tfidf scores with their respective 
std::vector<LabeledVector> build_tfidf_vectors(const std::vector<std::vector<std::string>>& reviews,
                                            const std::vector<std::string>& classLabels,
                                            const std::unordered_map<std::string, double>& vocabulary);
    
