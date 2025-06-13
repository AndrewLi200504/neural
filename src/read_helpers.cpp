
#include "read_helpers.h"

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

    std::string current_review;
    std::string current_label;
    bool inside_quoted_field = false;
    bool reading_review = true; // true for review, false for label
    
    while (std::getline(file, line)) {
        if (line.empty() && !inside_quoted_field) {
            continue;
        }
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (!inside_quoted_field) {
                if (c == '"') {
                    inside_quoted_field = true;
                    reading_review = true;
                    current_review.clear();
                    current_label.clear();
                } else if (c == ',') {
                    // Switch from review to label
                    reading_review = false;
                } else if (!reading_review) {
                    current_label += c;
                }
            } else {
                // Inside quoted field
                if (c == '"') {
                    // Check if this is an escaped quote
                    if (i + 1 < line.length() && line[i + 1] == '"') {
                        current_review += '"';
                        ++i; // Skip the next quote
                    } else {
                        // End of quoted field
                        inside_quoted_field = false;
                        reading_review = false;
                        
                        // Look for comma after the quote
                        size_t comma_pos = i + 1;
                        while (comma_pos < line.length() && (line[comma_pos] == ' ' || line[comma_pos] == '\t')) {
                            comma_pos++;
                        }
                        
                        if (comma_pos < line.length() && line[comma_pos] == ',') {
                            // Extract label from rest of line
                            current_label = line.substr(comma_pos + 1);
                            // Trim whitespace from label
                            current_label.erase(0, current_label.find_first_not_of(" \t\r\n"));
                            current_label.erase(current_label.find_last_not_of(" \t\r\n") + 1);
                            
                            // Save the review and label
                            if (!current_review.empty()) {
                                reviews.push_back(current_review);
                                labels.push_back(current_label);
                            }
                            
                            // Reset for next record
                            current_review.clear();
                            current_label.clear();
                            break;
                        }
                    }
                } else {
                    current_review += c;
                }
            }
        }
        
        // Add newline if we're still inside a quoted field
        if (inside_quoted_field) {
            current_review += '\n';
        }
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
    std::unordered_map<std::string, double> vocabulary;
    
    for (const auto& review : reviews) {
        // Count unique words for IDF calculation
        std::unordered_set<std::string> uniqueWordsInReview;
        for (const std::string& word : review) {
            uniqueWordsInReview.insert(word);
        }
        for (const std::string& word : uniqueWordsInReview) {
            vocabulary[word]++;
        }
    }
    
    // Calculate IDF values
    int numReviews = reviews.size();
    for (auto& entry : vocabulary) {
        vocabulary[entry.first] = log((double)numReviews / entry.second);
    }
    
    return vocabulary;
}

std::vector<LabeledVector> build_tfidf_vectors(const std::vector<std::vector<std::string>>& reviews,
                                               const std::vector<std::string>& classLabels,
                                               const std::unordered_map<std::string, double>& vocabulary) {
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
            double idf = vocabulary.at(word);
            tfidf.push_back(tf * idf);
        }

        labeledvectors.emplace_back(tfidf, classLabels[i]);
    }
    
    return labeledvectors;
}
