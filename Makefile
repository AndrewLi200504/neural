# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target executable name
TARGET = read

# Source file
SRC = read.cpp network.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run the program (assumes TestReviews.csv is in ./data/)
run: $(TARGET)
	@echo "Running program with data/TestReviews.csv"
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET) *.exe