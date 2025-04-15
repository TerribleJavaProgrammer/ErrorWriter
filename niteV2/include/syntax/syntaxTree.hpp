#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "token.hpp"

// SyntaxTreeNode: Represents a single node in the syntax tree
class SyntaxTreeNode {
    public:
        SyntaxTreeNode(std::shared_ptr<Token> token)
            : token(token), left(nullptr), right(nullptr) {}

        // Set the left and right child nodes
        void setLeftChild(std::shared_ptr<SyntaxTreeNode> node) { left = node; }
        void setRightChild(std::shared_ptr<SyntaxTreeNode> node) { right = node; }

        // Getters for node properties
        std::shared_ptr<Token> getToken() const { return token; }
        std::shared_ptr<SyntaxTreeNode> getLeftChild() const { return left; }
        std::shared_ptr<SyntaxTreeNode> getRightChild() const { return right; }

    private:
        std::shared_ptr<Token> token;   // The token at this node
        std::shared_ptr<SyntaxTreeNode> left;   // Left child node
        std::shared_ptr<SyntaxTreeNode> right;  // Right child node
};

// SyntaxTree: Represents the entire syntax tree
class SyntaxTree {
    public:
        SyntaxTree() : root(nullptr) {}

        // Build the syntax tree from a series of tokens
        void buildTree(const std::vector<std::shared_ptr<Token>>& tokens);

        // Get the root node of the tree
        std::shared_ptr<SyntaxTreeNode> getRoot() const { return root; }

    private:
        std::shared_ptr<SyntaxTreeNode> root;  // Root node of the syntax tree
};