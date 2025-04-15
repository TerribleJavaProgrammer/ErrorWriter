#include "syntax/clangCpp.hpp"

// Helper function to tokenize a line of code
void tokenizeLine(const std::string& line, std::vector<std::shared_ptr<Token>>& tokens) {
    std::string value;
    TokenType currentType = TokenType::Unknown;

    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];

        // Tokenize based on simple C++ syntax (e.g., keywords, operators, literals)
        if (std::isalnum(ch) || ch == '_') { // If alphanumeric or underscore, we might have an identifier or literal
            value += ch;
            currentType = TokenType::Identifier; // Initially treat as identifier
        } else if (ch == ' ' || ch == '\t') {
            if (!value.empty()) {
                tokens.push_back(std::make_shared<Token>(value, currentType));
                value.clear();
            }
        } else if (ch == ';' || ch == ',' || ch == '.' || ch == '(' || ch == ')') {
            if (!value.empty()) {
                tokens.push_back(std::make_shared<Token>(value, currentType));
                value.clear();
            }
            tokens.push_back(std::make_shared<Token>(std::string(1, ch), TokenType::Punctuation));
        }
        // Add more tokenization rules here (e.g., for operators, comments, etc.)
    }

    if (!value.empty()) {
        tokens.push_back(std::make_shared<Token>(value, currentType)); // Add remaining token
    }
}

void parseCppSyntax(Buffer& buf, SyntaxTree& tree) {
    std::vector<std::shared_ptr<Token>> tokens;

    // Iterate through each line in the buffer and tokenize it
    for (size_t i = 0; i < buf.lines.size(); ++i) {
        tokenizeLine(buf.getLine(i), tokens);  // Tokenize each line from the buffer
    }

    // Assuming tree is set up to build syntax from tokens, we can now build the tree
    tree.buildTree(tokens);
}

std::vector<Token> getTokens(SyntaxTree& tree) {
    std::vector<Token> tokens;
    auto root = tree.getRoot();

    // Traverse the tree and collect tokens
    std::function<void(std::shared_ptr<SyntaxTreeNode>)> traverse;
    traverse = [&](std::shared_ptr<SyntaxTreeNode> node) {
        if (node) {
            tokens.push_back(*node->getToken());  // Add token to the vector
            traverse(node->getLeftChild());       // Traverse left subtree
            traverse(node->getRightChild());      // Traverse right subtree
        }
    };

    traverse(root);  // Start from the root of the tree
    return tokens;   // Return the vector of tokens
}