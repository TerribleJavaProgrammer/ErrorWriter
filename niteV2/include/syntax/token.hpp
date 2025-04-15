#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>

enum class TokenType {
    Keyword,
    Operator,
    Identifier,
    Literal,
    Punctuation,
    Unknown
};
// Expandable!

class Token {
    public:
        // Constructor
        Token(std::string value, TokenType type, int line = -1, int column = -1);

        // Getters
        std::string getValue() const;
        TokenType getType() const;
        int getLine() const;
        int getColumn() const;

        // Helper functions to make token type readable
        std::string typeToString() const;

        // Utility method to print a token (for debugging purposes)
        void print() const;

    private:
        std::string value;   // The actual string of the token (e.g., "if", "+", etc.)
        TokenType type;      // The type of the token (Keyword, Identifier, etc.)
        int line;            // The line number in the source code (optional)
        int column;          // The column number in the source code (optional)
};

#endif // TOKEN_HPP