#include "labeledvector.h"
#include <iostream>
LabeledVector::LabeledVector(std::vector<double> tf, std::string cl) {
    TFIDF = tf;
    classification = cl;
}

LabeledVector::LabeledVector(int l, std::string cl) {
    length = l;
    TFIDF.resize(l);
    classification = cl;

}

void LabeledVector::print() {
    length = TFIDF.size();
    for (int i = 0; i < length; i++) {
        printf("%f ", TFIDF[i]);
    }
    std::cout << classification << std::endl;
}

std::vector<double> LabeledVector::getTFIDF() const {
    return TFIDF;
}

std::string LabeledVector::getClassification() const {
    return classification;
}