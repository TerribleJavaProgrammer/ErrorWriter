#include "syntax/highlighter.hpp"

// Highlight a line by identifying tokens and applying colors
void highlightLine(const std::string line, std::vector<Token>& outTokens) {
    std::istringstream stream(line);
    std::string tokenValue;
    int column = 0;

    // Process each word (token) from the line
    while (stream >> tokenValue) {
        Token token(tokenValue, TokenType::Unknown, -1, column);  // Create a token with default values

        // Identify token type and assign color (simplified for demonstration purposes)
        Token identifiedToken = Token(token.getValue(), token.getType(), token.getLine(), token.getColumn());

        // Apply color based on token type
        Color tokenColor = getTokenColor(identifiedToken);  // Pass the identified token to getTokenColor

        // Output the token and its color (for illustration purposes)
        std::cout << "Token: " << identifiedToken.getValue() << ", Color: " << colorToString(tokenColor) << std::endl;

        // Add token to the list of output tokens
        outTokens.push_back(identifiedToken);
    }
}

// Function to check if a string is a valid keyword
bool isKeyword(const std::string& value) {
    static const std::set<std::string> keywords = {
        "if", "else", "while", "for", "return", "int", "float", "void"
        // Add more keywords as needed
    };
    return keywords.find(value) != keywords.end();
}

// Function to check if a string is a valid operator
bool isOperator(const std::string& value) {
    static const std::set<std::string> operators = {
        "+", "-", "*", "/", "=", "==", "&&", "||", "!", "<", ">", "<=", ">="
        // Add more operators as needed
    };
    return operators.find(value) != operators.end();
}

// Function to check if a string is a valid identifier
bool isIdentifier(const std::string& value) {
    return !value.empty() && std::isalpha(value[0]);
}

// Function to check if a string is a valid literal
bool isLiteral(const std::string& value) {
    return value[0] == '"' || std::isdigit(value[0]);  // Simple check for literals (strings or numbers)
}

// Get the color corresponding to the token's type
Color getTokenColor(const Token& t) {
    switch (t.getType()) {
        case TokenType::Keyword:
            return Color(0, 0, 255);  // Blue for keywords
        case TokenType::Operator:
            return Color(255, 0, 0);  // Red for operators
        case TokenType::Identifier:
            return Color(0, 255, 0);  // Green for identifiers
        case TokenType::Literal:
            return Color(255, 165, 0); // Orange for literals
        case TokenType::Unknown:
        default:
            return Color(255, 255, 255); // White for unknown tokens
    }
}