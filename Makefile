# Makefile for Tiny Editor Project (Windows only)

# Compiler settings
CXX      = g++.exe
CXXFLAGS = -std=c++17 -Wall -Wextra

# Linker flags (static + filesystem)
LDFLAGS  = -static -static-libgcc -static-libstdc++ -lstdc++fs

# Target executable name
TARGET   = nite.exe

# Source files
SRC      = nite.cpp

# Object files
OBJ      = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Link the object files into the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Compile .cpp files into .o object files
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up compiled files (Windows compatible)
clean:
	del /f $(OBJ) $(TARGET)

# Run the editor (Windows compatible)
run: $(TARGET)
	./$(TARGET)