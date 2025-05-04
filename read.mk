# Makefile for reading CSV using ifstream

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -Ieigen

# Target executable name
TARGET = read

# Source file
SRC = read.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program (assumes TestReviews.csv is in ./data/)
run: $(TARGET)
	@echo "Running program with data/TestReviews.csv"
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)