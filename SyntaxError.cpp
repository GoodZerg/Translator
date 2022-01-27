#include "SyntaxError.h"

void SyntaxError::What() { 
  std::cout << "In line " << line << "\n";
  std::cout << messedge << "\n";
  return;
}
