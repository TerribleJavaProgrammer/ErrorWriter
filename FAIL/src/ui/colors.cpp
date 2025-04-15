#include "ui/colors.hpp"

// Set the foreground color for the terminal using RGB values from the Color struct
void setForegroundColor(const Color& color) {
    std::string ansiCode = colorToAnsiCode(color);
    std::cout << ansiCode;  // Set the color using the ANSI escape code
}

// Reset to the default terminal color
void resetColor() {
    std::cout << "\033[39m";  // Reset to default foreground color
}

// Convert Color (RGB) to an ANSI escape code for terminal colors
std::string colorToAnsiCode(const Color& color) {
    // ANSI escape code format for RGB: \033[38;2;<r>;<g>;<b>m
    return "\033[38;2;" + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m";
}

// Helper function for debugging: Convert Color struct to a string representation
std::string colorToString(const Color& color) {
    return "Color(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")";
}