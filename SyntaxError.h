#pragma once

#include <exception>
#include <string>
#include <iostream>
#include "Lex.h"


class SyntaxError : std::exception {
 public:
  SyntaxError() = default;
  SyntaxError(Token* token, std::string messedge)
      : token(token), messedge(messedge) {};
  std::string messedge;
  Token* token = nullptr;

  void What();
};
