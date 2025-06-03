#include <vector>
#include <string>
#include <unordered_map>
#include "labeledvector.h"
#include "network.h"

// Tokenizer 
std::vector<std::string> tokenize(const std::string& text);

// Robust CSV parser for review-label pairs 
std::pair<std::vector<std::string>, std::vector<std::string>> read_reviews_and_labels(const std::string& filepath);

// Tokenize each review                
std::vector<std::vector<std::string>> tokenize_reviews(const std::vector<std::string>& review_texts);
        
// Build vocabulary
std::unordered_map<std::string, double> build_vocabulary(const std::vector<std::vector<std::string>>& reviews);

// Build TF-IDF vectors 
std::vector<LabeledVector> build_tfidf_vectors(const std::vector<std::vector<std::string>>& reviews,
                                            const std::vector<std::string>& classLabels,
                                            const std::unordered_map<std::string, double>& vocabulary);
        
    
