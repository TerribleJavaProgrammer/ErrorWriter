#include "token.hpp"

Token::Token(std::string value, TokenType type, int line = -1, int column = -1) {
    // Implement a constructor for the token class
}

std::string Token::getValue() const {
    // Implement a value getter for the token class
}

TokenType Token::getType() const {
    // Implement a type getter for the token class
}

int Token::getLine() const {
    // Implement a line getter for the token class
}

int Token::getColumn() const {
    // Implement a column getter for the token class
}

std::string Token::typeToString() const {
    // Implement a "toString" method for the type of the token
}

void Token::print() const {
    // Implement a debugging printer for the token class
}