#include "core/input.hpp"

// Poll for input from the user (keyboard)
InputEvent pollInput() {
    InputEvent event = {false, '\0', SpecialKey::None};  // Initialize event

    if (_kbhit()) {  // If a key is pressed
        char ch = _getch();  // Get the key

        // Add backspace handling
        if (ch == 8) {  // ASCII 8 is backspace
            event.isChar = false;
            event.specialKey = SpecialKey::Backspace;
            return event;
        }
        
        // Check if the key is a special key (arrow keys, etc.)
        if (ch == 0 || ch == 224) {  // Special key prefix (e.g., arrow keys)
            ch = _getch();  // Get the actual special key

            // Map special keys to the SpecialKey enum
            switch (ch) {
                case 72: event.specialKey = SpecialKey::ArrowUp; break;
                case 80: event.specialKey = SpecialKey::ArrowDown; break;
                case 75: event.specialKey = SpecialKey::ArrowLeft; break;
                case 77: event.specialKey = SpecialKey::ArrowRight; break;
                case 13: event.specialKey = SpecialKey::Enter; break;
                case 27: event.specialKey = SpecialKey::Escape; break;
                default: event.specialKey = SpecialKey::None; break;
            }
            event.isChar = false;
        } else {  // Normal character key press
            event.isChar = true;
            event.character = ch;
        }
    }

    return event;
}

// Check if the input event corresponds to a special key
bool isSpecialKey(InputEvent e) {
    return !e.isChar;  // If it's not a char, it is a special key
}

// Convert an InputEvent to a character (if it's a regular key press)
char inputToChar(InputEvent e) {
    if (e.isChar) {
        return e.character;  // Return the character for normal key presses
    }
    return '\0';  // Return null for special keys
}