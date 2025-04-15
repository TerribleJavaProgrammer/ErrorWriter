#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>

enum class TokenType {
    // Keywords (e.g., for, if, else, return, etc.)
    Keyword,
    Type,             // e.g., int, float, double, char
    Function,         // e.g., function name or a callable type
    Control,          // e.g., break, continue, return, etc.
    Modifier,         // e.g., public, private, static

    // Operators
    Operator,         // General operator (+, -, *, /, etc.)
    Assignment,       // Assignment operators (=, +=, -=, *=, /=, etc.)
    Comparison,       // Comparison operators (==, !=, <, <=, >, >=)
    Logical,          // Logical operators (&&, ||, !)
    Bitwise,          // Bitwise operators (&, |, ^, ~, <<, >>)
    Arithmetic,       // Arithmetic operators (+, -, *, /, %)

    // Identifiers
    Identifier,       // Variable or function name, class name, etc.

    // Literals
    Literal,          // General literal (e.g., 42, "Hello World", true, etc.)
    Integer,          // Integer literal (e.g., 42)
    FloatingPoint,    // Floating point literal (e.g., 3.14)
    String,           // String literal (e.g., "Hello")
    Character,        // Character literal (e.g., 'a')
    Boolean,          // Boolean literal (e.g., true, false)
    Null,             // Null literal (e.g., NULL, nullptr)
    Array,            // Array literal (e.g., [1, 2, 3])

    // Punctuation and delimiters
    Punctuation,      // General punctuation (e.g., ;, :, ,)
    Parentheses,      // Parentheses ((), [])
    Braces,           // Braces ({})
    Comma,            // Comma (,)
    Semicolon,        // Semicolon (;)
    Colon,            // Colon (:)
    Period,           // Period (.)

    // Keywords for flow control
    FlowControl,      // Keywords for flow control (e.g., if, else, switch, while)
    LoopControl,      // Keywords for loops (e.g., for, while, do)
    Jump,             // Keywords for jumping in code (e.g., break, continue, return)

    // Special tokens
    Comment,          // Comment (single-line or multi-line)
    Preprocessor,     // Preprocessor directives (#include, #define, etc.)

    // Miscellaneous
    WhiteSpace,       // White space (spaces, tabs)
    NewLine,          // New line character
    EndOfFile,        // End of file (EOF)

    Unknown           // Anything that doesn’t fit into the above categories
};

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