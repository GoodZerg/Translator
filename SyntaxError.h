#pragma once

#include <exception>
#include <string>
#include <iostream>


class SyntaxError : std::exception {
 public:
  SyntaxError() = default;
  SyntaxError(int line,std::string messedge)
      : line(line), messedge(messedge) {};
  std::string messedge;
  int line = -1;

  void What();
};
