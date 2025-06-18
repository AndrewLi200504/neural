#ifndef LABELEDVECTOR_H
#define LABELEDVECTOR_H

#include <vector>
#include <string>

class LabeledVector {
    public: 
        // Create Labelled Vector
        LabeledVector(std::vector<double> TFIDF, std::string classification);

        // Create Labelled Vector (empty)
        LabeledVector(int length, std::string classification);

        // Print Labelled Vector 
        void print();

        // Get TFIDF
        std::vector<double> getTFIDF() const;

        // Get classification
        std::string getClassification() const;
    private: 
        // Vector of TF-IDF scores (double)
        std::vector<double> TFIDF;
        // Binary classification (0 - negative, 1 - positive)
        std::string classification;
        // Length
        int length;
};
#endif