#pragma once

#include <exception>
#include <string>
#include <iostream>


class SyntaxError : std::exception {
 public:
  SyntaxError() = default;
  SyntaxError(int line, std::string filePass, std::string messedge)
      : line(line), filePass(filePass), messedge(messedge) {};
  std::string filePass;
  std::string messedge;
  int line = -1;

  void What();
};
