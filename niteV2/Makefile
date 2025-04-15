# === CONFIGURATION ===
CC       := g++  # Use g++ for C++ files
CFLAGS   := -Wall -Wextra -O2 -I./include  # Include the include/ directory relative to the Makefile
LDFLAGS  := 
OUT_DIR  := build
TARGET   := $(OUT_DIR)/nite.exe

# === SOURCE FILES ===
SRC_DIRS := src src/core src/config src/filesystem src/syntax src/ui
SRC      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ      := $(patsubst src/%.cpp,$(OUT_DIR)/%.o,$(SRC))

# === DEFAULT TARGET ===
all: $(TARGET)

# === LINKING ===
$(TARGET): $(OBJ)
	@echo Linking $@
	@$(CC) $(OBJ) -o $@ $(LDFLAGS)

# === COMPILING ===
$(OUT_DIR)/%.o: src/%.cpp
	@echo Creating directory $(dir $@)
	@if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@echo Compiling $<
	@$(CC) $(CFLAGS) -I$(PWD)/include -c $< -o $@

# === CLEAN ===
clean:
	@echo Cleaning...
	@rd /s /q $(OUT_DIR) 2>nul || echo Clean.

# === PHONY TARGETS ===
.PHONY: all clean
