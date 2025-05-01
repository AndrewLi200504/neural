#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::ifstream file("data/TestReviews.csv");  // Relative path
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }

    file.close();
    return 0;
}