#include "syntax/token.hpp"

Token::Token(std::string value, TokenType type, int line, int column)
    : value(value), type(type), line(line), column(column) {
    // Constructor initializes the token with its value, type, line, and column information
}

std::string Token::getValue() const {
    return value;
}

TokenType Token::getType() const {
    return type;
}

int Token::getLine() const {
    return line;
}

int Token::getColumn() const {
    return column;
}

std::string Token::typeToString() const {
    switch (type) {
        case TokenType::Keyword:         return "Keyword";
        case TokenType::Type:            return "Type";
        case TokenType::Function:        return "Function";
        case TokenType::Control:         return "Control";
        case TokenType::Modifier:        return "Modifier";

        case TokenType::Operator:        return "Operator";
        case TokenType::Assignment:      return "Assignment";
        case TokenType::Comparison:      return "Comparison";
        case TokenType::Logical:         return "Logical";
        case TokenType::Bitwise:         return "Bitwise";
        case TokenType::Arithmetic:      return "Arithmetic";

        case TokenType::Identifier:      return "Identifier";
        case TokenType::Literal:         return "Literal";
        case TokenType::Integer:         return "Integer";
        case TokenType::FloatingPoint:   return "FloatingPoint";
        case TokenType::String:          return "String";
        case TokenType::Character:       return "Character";
        case TokenType::Boolean:         return "Boolean";
        case TokenType::Null:            return "Null";
        case TokenType::Array:           return "Array";

        case TokenType::Punctuation:     return "Punctuation";
        case TokenType::Parentheses:     return "Parentheses";
        case TokenType::Braces:          return "Braces";
        case TokenType::Comma:           return "Comma";
        case TokenType::Semicolon:       return "Semicolon";
        case TokenType::Colon:           return "Colon";
        case TokenType::Period:          return "Period";

        case TokenType::FlowControl:     return "FlowControl";
        case TokenType::LoopControl:     return "LoopControl";
        case TokenType::Jump:            return "Jump";

        case TokenType::Comment:         return "Comment";
        case TokenType::Preprocessor:    return "Preprocessor";

        case TokenType::WhiteSpace:      return "WhiteSpace";
        case TokenType::NewLine:         return "NewLine";
        case TokenType::EndOfFile:       return "EndOfFile";

        case TokenType::Unknown:         return "Unknown";

        default:                         return "Unknown";
    }
}

void Token::print() const {
    std::cout << "Token: " << value << ", Type: " << typeToString() 
              << ", Line: " << line << ", Column: " << column << std::endl;
}