# === CONFIGURATION ===
CC       := clang
CFLAGS   := -Wall -Wextra -O2 -Iinclude
LDFLAGS  := 
OUT_DIR  := build
TARGET   := $(OUT_DIR)/nite.exe

# === SOURCE FILES ===
SRC_DIRS := src src/core src/config src/filesystem src/syntax src/ui
SRC      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJ      := $(patsubst src/%.c, $(OUT_DIR)/%.o, $(SRC))

# === DEFAULT TARGET ===
all: $(TARGET)

# === LINKING ===
$(TARGET): $(OBJ)
    @echo Linking $@
    @$(CC) $(OBJ) -o $@ $(LDFLAGS)

# === COMPILING ===
$(OUT_DIR)/%.o: src/%.c
    @mkdir $(OUT_DIR) 2>nul || echo.
    @mkdir $(dir $@) 2>nul || echo.
    @echo Compiling $<
    @$(CC) $(CFLAGS) -c $< -o $@

# === CLEAN ===
clean:
    @echo Cleaning...
    @rd /s /q $(OUT_DIR) 2>nul || echo Clean.

# === PHONY TARGETS ===
.PHONY: all clean
