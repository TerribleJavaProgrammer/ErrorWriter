#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP
#include <string>
#include "colors.hpp"
#include "syntaxTree.hpp"
#include "token.hpp"

void highlightLine(const std::string line, Token& outTokens);

Color getTokenColor(Token t);

#endif HIGHLIGHTER_HPP
