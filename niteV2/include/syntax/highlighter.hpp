#pragma once
#include <string>
#include <sstream>
#include <cctype>
#include <sstream>
#include <cctype>
#include <set>
#include "ui/colors.hpp"
#include "syntaxTree.hpp"
#include "token.hpp"
#include "ui/colors.hpp"

void highlightLine(const std::string line, std::vector<Token>& outTokens);
Color getTokenColor(const Token& t);