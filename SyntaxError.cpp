#include "SyntaxError.h"

void SyntaxError::What() { 
  if(token != nullptr) {
    std::cout << "In line " << token->_line << "\n";
    std::cout << "In column " << token->_column << "\n";
  }
  std::cout << messedge << "\n";
  return;
}
