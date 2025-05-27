# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target executables
TARGET = read
TEST_TARGET = test_exec

# Source file
SRC = src/read.cpp src/network.cpp src/layer.cpp src/labeledvector.cpp

# Test files
TEST_SRC = tests/test_main.cpp tests/test_layer.cpp tests/catch_amalgamated.cpp src/layer.cpp


# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Build the test executable
$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

# Run the program (assumes TestReviews.csv is in ./data/)
run: $(TARGET)
	@echo "Running program with data/TestReviews.csv"
	./$(TARGET)

# Run tests
test: $(TEST_TARGET)
	@echo "Running tests..."
	./$(TEST_TARGET)


# Clean up build files
clean:
	rm -f $(TARGET)  $(TEST_TARGET) *.exe