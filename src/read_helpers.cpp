#include "read_helpers.h"

// Memory-safe tokenization with limits and filtering
std::vector<std::vector<std::string>> tokenize_reviews(const std::vector<std::string>& review_texts) {
    std::vector<std::vector<std::string>> reviews;
    reviews.reserve(review_texts.size());
    
    // Set reasonable limits
    const int MAX_TOKENS_PER_REVIEW = 1000;  // Limit words per review
    const int MIN_WORD_LENGTH = 3;          // Skip very short words
    const int MAX_WORD_LENGTH = 20;         // Skip very long words (likely garbage)
    
    // Common stop words to skip (saves memory)
    std::unordered_set<std::string> stop_words = {
        "the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", 
        "of", "with", "by", "is", "are", "was", "were", "be", "been", "have", 
        "has", "had", "do", "does", "did", "will", "would", "could", "should",
        "this", "that", "these", "those", "i", "you", "he", "she", "it", "we", "they"
    };
    
    size_t total_tokens = 0;
    
    for (size_t i = 0; i < review_texts.size(); i++) {
        if (i % 500 == 0) {
            std::cout << "Processing review " << i + 1 << "/" << review_texts.size() 
                     << " (total tokens so far: " << total_tokens << ")" << std::endl;
        }
        
        std::vector<std::string> tokens;
        tokens.reserve(MAX_TOKENS_PER_REVIEW); // Pre-allocate
        
        std::istringstream iss(review_texts[i]);
        std::string word;
        int token_count = 0;
        
        while (iss >> word && token_count < MAX_TOKENS_PER_REVIEW) {
            // Clean the word
            std::string clean_word;
            for (char c : word) {
                if (std::isalnum(c)) {
                    clean_word += std::tolower(c);
                }
            }
            
            // Filter words
            if (clean_word.length() >= MIN_WORD_LENGTH && 
                clean_word.length() <= MAX_WORD_LENGTH &&
                stop_words.find(clean_word) == stop_words.end()) {
                
                tokens.push_back(std::move(clean_word));
                token_count++;
                total_tokens++;
            }
        }
        
        reviews.push_back(std::move(tokens));
        
        // Safety check: if we're using too much memory, stop
        if (total_tokens > 2000000) { // 2M tokens max
            std::cout << "WARNING: Reached token limit, processing only first " 
                     << i + 1 << " reviews" << std::endl;
            break;
        }
    }
    
    std::cout << "Tokenization complete: " << total_tokens << " total tokens" << std::endl;
    return reviews;
}

std::pair<std::vector<std::string>, std::vector<std::string>> 
read_reviews_and_labels(const std::string& filepath) {
     int max_per_class = 1000;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return {{}, {}};
    }

    std::vector<std::string> all_reviews;
    std::vector<std::string> all_labels;
    std::string line;
    
    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "Empty file or failed to read header" << std::endl;
        return {{}, {}};
    }

    // Read all data first (reuse your existing parsing logic)
    std::ostringstream current_review;
    bool inside_quoted_field = false;
    bool reading_review = true;
    
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
                } else if (c == ',') {
                    reading_review = false;
                } else if (!reading_review) {
                    // We're reading the label
                }
            } else {
                if (c == '"') {
                    if (i + 1 < line.length() && line[i + 1] == '"') {
                        current_review << '"';
                        ++i;
                    } else {
                        if (i + 1 < line.length() && line[i + 1] == ',') {
                            std::string review_text = current_review.str();
                            std::string label = line.substr(i + 2);
                            
                            label.erase(0, label.find_first_not_of(" \t\r\n"));
                            label.erase(label.find_last_not_of(" \t\r\n") + 1);
                            
                            if (!review_text.empty()) {
                                all_reviews.push_back(review_text);
                                all_labels.push_back(label);
                            }
                            
                            current_review.clear();
                            inside_quoted_field = false;
                            break;
                        }
                    }
                } else {
                    current_review << c;
                }
            }
        }
        
        if (inside_quoted_field) {
            current_review << '\n';
        }
    }
    
    std::cout << "Total reviews loaded: " << all_reviews.size() << std::endl;
    
    // Now balance the classes
    std::unordered_map<std::string, std::vector<int>> class_indices;
    
    // Group indices by class
    for (int i = 0; i < all_labels.size(); i++) {
        class_indices[all_labels[i]].push_back(i);
    }
    
    std::cout << "Class distribution before balancing:" << std::endl;
    for (const auto& pair : class_indices) {
        std::cout << "Class " << pair.first << ": " << pair.second.size() << " samples" << std::endl;
    }
    
    // Sample equally from each class
    std::vector<std::string> balanced_reviews;
    std::vector<std::string> balanced_labels;
    
    std::random_device rd;
    std::mt19937 g(rd());
    
    for (auto& pair : class_indices) {
        std::string class_name = pair.first;
        std::vector<int>& indices = pair.second;
        
        // Shuffle indices for this class
        std::shuffle(indices.begin(), indices.end(), g);
        
        // Take up to max_per_class samples
        int samples_to_take = std::min(max_per_class, static_cast<int>(indices.size()));
        
        for (int i = 0; i < samples_to_take; i++) {
            int idx = indices[i];
            balanced_reviews.push_back(all_reviews[idx]);
            balanced_labels.push_back(all_labels[idx]);
        }
        
        std::cout << "Selected " << samples_to_take << " samples from class " << class_name << std::endl;
    }
    
    // Shuffle the balanced dataset
    std::vector<std::pair<std::string, std::string>> combined;
    for (size_t i = 0; i < balanced_reviews.size(); i++) {
        combined.push_back({balanced_reviews[i], balanced_labels[i]});
    }
    std::shuffle(combined.begin(), combined.end(), g);
    
    // Separate back into reviews and labels
    balanced_reviews.clear();
    balanced_labels.clear();
    for (const auto& pair : combined) {
        balanced_reviews.push_back(pair.first);
        balanced_labels.push_back(pair.second);
    }
    
    std::cout << "Final balanced dataset: " << balanced_reviews.size() << " samples" << std::endl;
    
    return {balanced_reviews, balanced_labels};
}


std::unordered_map<std::string, double> build_vocabulary(
    const std::vector<std::vector<std::string>>& reviews) {
    
    std::unordered_map<std::string, int> doc_freq;
    
    // Count how many documents each word appears in
    for (const auto& review : reviews) {
        std::unordered_set<std::string> unique_words_in_doc;
        for (const auto& word : review) {
            unique_words_in_doc.insert(word);
        }
        for (const auto& word : unique_words_in_doc) {
            doc_freq[word]++;
        }
    }
    
    // Build vocabulary with IDF values
    std::unordered_map<std::string, double> vocabulary;
    int total_docs = reviews.size();
    
    for (const auto& pair : doc_freq) {
        const std::string& word = pair.first;
        int df = pair.second;
        double idf = std::log(static_cast<double>(total_docs) / df);
        vocabulary[word] = idf;
    }
    
    std::cout << "Simple vocabulary size: " << vocabulary.size() << std::endl;
    return vocabulary;
}

// Debug version of TF-IDF building to catch the error
std::vector<LabeledVector> build_tfidf_vectors_safe(
    const std::vector<std::vector<std::string>>& reviews,
    const std::vector<std::string>& labels,
    const std::unordered_map<std::string, double>& vocabulary) {
    
    std::vector<LabeledVector> result;
    
    if (vocabulary.empty()) {
        std::cerr << "ERROR: Empty vocabulary passed to TF-IDF builder!" << std::endl;
        return result;
    }
    
    std::cout << "Building TF-IDF vectors with vocabulary size: " << vocabulary.size() << std::endl;
    
    for (size_t i = 0; i < reviews.size(); i++) {
        if (i % 1000 == 0) {
            std::cout << "Building TF-IDF for review " << i + 1 << "/" << reviews.size() << std::endl;
        }
        
        const auto& review = reviews[i];
        std::vector<double> tfidf_vector(vocabulary.size(), 0.0);
        
        // Count word frequencies in this document
        std::unordered_map<std::string, int> word_count;
        for (const auto& word : review) {
            word_count[word]++;
        }
        
        // Calculate TF-IDF for each word in vocabulary
        int vocab_index = 0;
        for (const auto& vocab_pair : vocabulary) {
            const std::string& word = vocab_pair.first;
            double idf = vocab_pair.second;
            
            if (word_count.find(word) != word_count.end()) {
                double tf = static_cast<double>(word_count[word]) / review.size();
                tfidf_vector[vocab_index] = tf * idf;
            }
            vocab_index++;
        }
        
        result.push_back(LabeledVector(tfidf_vector, labels[i]));
    }
    
    return result;
}
// Fixed version of build_tfidf_vectors that creates proper-sized vectors
std::vector<LabeledVector> build_tfidf_vectors(const std::vector<std::vector<std::string>>& reviews,
                                               const std::vector<std::string>& classLabels,
                                               const std::unordered_map<std::string, double>& vocabulary) {
    std::vector<LabeledVector> labeledvectors;
    
    // Create a mapping from words to indices for consistent ordering
    std::vector<std::string> vocab_words;
    vocab_words.reserve(vocabulary.size());
    for (const auto& pair : vocabulary) {
        vocab_words.push_back(pair.first);
    }
    
    std::cout << "Building TF-IDF vectors with vocabulary size: " << vocabulary.size() << std::endl;
    
    for (size_t i = 0; i < reviews.size(); ++i) {
        if (i % 1000 == 0) {
            std::cout << "Processing review " << i + 1 << "/" << reviews.size() << std::endl;
        }
        
        const auto& review = reviews[i];
        
        // Count word frequencies in this document
        std::unordered_map<std::string, int> word_count;
        for (const auto& word : review) {
            word_count[word]++;
        }
        
        // Create TF-IDF vector with full vocabulary size
        std::vector<double> tfidf(vocabulary.size(), 0.0);
        
        // Calculate TF-IDF for each word in vocabulary
        for (size_t vocab_idx = 0; vocab_idx < vocab_words.size(); ++vocab_idx) {
            const std::string& word = vocab_words[vocab_idx];
            
            if (word_count.find(word) != word_count.end()) {
                double tf = static_cast<double>(word_count.at(word)) / review.size();
                double idf = vocabulary.at(word);
                tfidf[vocab_idx] = tf * idf;
            }
            // If word not in document, TF-IDF remains 0.0
        }
        
        labeledvectors.emplace_back(tfidf, classLabels[i]);
    }
    
    std::cout << "TF-IDF vectors created. Each vector size: " << vocabulary.size() << std::endl;
    return labeledvectors;
}