#ifndef CLANGCPP_HPP
#define CLANGCPP_HPP
#include "core/buffer.hpp"
#include "syntax/syntaxTree.hpp"
#include "syntax/token.hpp"
#include <sstream>
#include <cctype>
#include <functional>

void parseCppSyntax(Buffer& buf, SyntaxTree& tree);

Token& getTokens(SyntaxTree& tree);

#endif //CLANGCPP_HPP