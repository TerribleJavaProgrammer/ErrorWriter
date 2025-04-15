#pragma once

#include <iostream>
#include <conio.h>

// Enum for special keys (Exapnsible)
enum class SpecialKey {
    None,
    Enter,
    Backspace,
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    Escape
};

// Struct for capturing input events
struct InputEvent {
    bool isChar;          // True if the event is a character key press, false if it's a special key
    char character;       // The character for normal key presses (e.g., 'a', 'b', etc.)
    SpecialKey specialKey; // The special key (if applicable)
};

// Function declarations
InputEvent pollInput();         // Capture user input and return it as an InputEvent
bool isSpecialKey(InputEvent e); // Check if the input is a special key
char inputToChar(InputEvent e);  // Convert InputEvent to a character (returns '\0' for special keys)
