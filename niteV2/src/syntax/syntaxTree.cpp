#include "syntax/syntaxTree.hpp"

// Helper functions to parse specific grammar parts
std::shared_ptr<SyntaxTreeNode> parseExpression(const std::vector<std::shared_ptr<Token>>& tokens, size_t& index);
std::shared_ptr<SyntaxTreeNode> parseTerm(const std::vector<std::shared_ptr<Token>>& tokens, size_t& index);
std::shared_ptr<SyntaxTreeNode> parseFactor(const std::vector<std::shared_ptr<Token>>& tokens, size_t& index);

// Main buildTree function
void SyntaxTree::buildTree(const std::vector<std::shared_ptr<Token>>& tokens) {
    // Convert tokens to a mutable vector for easy manipulation (e.g., advancing index)
    std::vector<std::shared_ptr<Token>> modifiableTokens = tokens;

    size_t index = 0;
    root = parseExpression(modifiableTokens, index);
}

// Parse expressions (E = T | E O T)
std::shared_ptr<SyntaxTreeNode> parseExpression(const std::vector<std::shared_ptr<Token>>& tokens, size_t& index) {
    auto left = parseTerm(tokens, index);

    while (index < tokens.size() &&
           (tokens[index]->getType() == TokenType::Operator || 
            tokens[index]->getType() == TokenType::Arithmetic || 
            tokens[index]->getType() == TokenType::Logical)) {
        auto operatorToken = tokens[index];
        index++;  // Consume operator

        auto right = parseTerm(tokens, index);
        auto operatorNode = std::make_shared<SyntaxTreeNode>(operatorToken);
        operatorNode->setLeftChild(left);
        operatorNode->setRightChild(right);
        left = operatorNode;  // Update left child for next operation
    }

    return left;
}

// Parse terms (T = F | T O F)
std::shared_ptr<SyntaxTreeNode> parseTerm(const std::vector<std::shared_ptr<Token>>& tokens, size_t& index) {
    auto left = parseFactor(tokens, index);

    while (index < tokens.size() &&
           (tokens[index]->getType() == TokenType::Arithmetic || 
            tokens[index]->getType() == TokenType::Bitwise)) {
        auto operatorToken = tokens[index];
        index++;  // Consume operator

        auto right = parseFactor(tokens, index);
        auto operatorNode = std::make_shared<SyntaxTreeNode>(operatorToken);
        operatorNode->setLeftChild(left);
        operatorNode->setRightChild(right);
        left = operatorNode;  // Update left child for next operation
    }

    return left;
}

// Parse factors (F = Literal | '(' E ')')
std::shared_ptr<SyntaxTreeNode> parseFactor(const std::vector<std::shared_ptr<Token>>& tokens, size_t& index) {
    if (tokens[index]->getType() == TokenType::Literal || 
        tokens[index]->getType() == TokenType::Integer || 
        tokens[index]->getType() == TokenType::FloatingPoint || 
        tokens[index]->getType() == TokenType::Boolean || 
        tokens[index]->getType() == TokenType::String) {
        // Handle literals like integer, floating-point, boolean, string
        auto node = std::make_shared<SyntaxTreeNode>(tokens[index]);
        index++;  // Consume literal token
        return node;
    } else if (tokens[index]->getType() == TokenType::Parentheses) {
        index++;  // Consume '('
        auto node = parseExpression(tokens, index);
        if (tokens[index]->getType() != TokenType::Parentheses) {
            throw std::runtime_error("Expected closing parenthesis");
        }
        index++;  // Consume ')'
        return node;
    } else if (tokens[index]->getType() == TokenType::Identifier) {
        // Handle identifiers (variables, function names)
        auto node = std::make_shared<SyntaxTreeNode>(tokens[index]);
        index++;  // Consume identifier token
        return node;
    }

    throw std::runtime_error("Unexpected token while parsing factor");
}