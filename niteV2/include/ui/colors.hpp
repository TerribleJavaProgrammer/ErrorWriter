#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>

enum class Color {
    Red,
    Green,
    Blue,
    Yellow,
    White,
    Black,
    Default
};

// Function to get the color associated with a token type (you can expand this based on your needs)
Color getColorFromTheme(const std::string tokenType);

// Set the foreground color based on the platform
void setForegroundColor(Color c);

// Reset to the default terminal color
void resetColor();

#endif // COLORS_HPP