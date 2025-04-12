# Makefile for Tiny Editor Project

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Target executable name
TARGET = err

# Source files
SRC = ErrorWriter.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile the .cpp files into .o object files
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC)

# Clean up compiled files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the editor
run: $(TARGET)
	./$(TARGET)