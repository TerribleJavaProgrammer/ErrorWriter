#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>
#include <iostream>

// Struct to represent a color with RGB components
struct Color {
    int r;  // Red component (0-255)
    int g;  // Green component (0-255)
    int b;  // Blue component (0-255)

    // Constructor to initialize RGB values
    Color(int red, int green, int blue) : r(red), g(green), b(blue) {}
};

// Set the foreground color based on the Color struct
void setForegroundColor(const Color& color);

// Reset to the default terminal color
void resetColor();

// Function to convert Color to a terminal color code (ANSI escape code)
std::string colorToAnsiCode(const Color& color);

// Helper function to get the string representation of a Color (for debugging)
std::string colorToString(const Color& color);

#endif // COLORS_HPP