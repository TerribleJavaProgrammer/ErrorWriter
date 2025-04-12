# Makefile for Tiny Editor Project (Windows only)

# Compiler settings
CXX = g++.exe
CXXFLAGS = -std=c++11 -Wall -Wextra

# Target executable name
TARGET = nite.exe

# Source files
SRC = nite.cpp

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

# Clean up compiled files (Windows compatible)
clean:
	del /f $(OBJ) $(TARGET)

# Run the editor (Windows compatible)
run: $(TARGET)
	./$(TARGET)