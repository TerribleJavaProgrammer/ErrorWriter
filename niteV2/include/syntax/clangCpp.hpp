#ifndef CLANGCPP_HPP
#define CLANGCPP_HPP
#include "core/buffer.hpp"
#include "syntaxTree.hpp"
#include "token.hpp"

void parseCppSyntax(Buffer& buf, SyntaxTree& tree);

Token& getTokens(SyntaxTree& tree);

#endif //CLANGCPP_HPP