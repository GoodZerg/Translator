#include "Syntax.h"

Syntax::Syntax(char* input) {
  lex = new Lex(input);
}

Syntax::TType* Syntax::_parseType() {
  return nullptr;
}

Syntax::TBlock* Syntax::_parseBlock() {
  return nullptr;
}

Syntax::TExp* Syntax::_parseExpression() {
  return nullptr;
}

Syntax::TInit* Syntax::_parseInit() {
  return nullptr;
}

Syntax::TIf* Syntax::_parseIf() {
  return nullptr;
}

Syntax::TWhile* Syntax::_parseWhile() {
  return nullptr;
}

Syntax::TFor* Syntax::_parseFor() {
  return nullptr;
}

Syntax::TForEach* Syntax::_parseForEach() {
  return nullptr;
}

Syntax::TPrint* Syntax::_parsePrint() {
  return nullptr;
}

Syntax::TRead* Syntax::_parseRead() {
  return nullptr;
}

Syntax::TMatch* Syntax::_parseMatch() {
  return nullptr;
}

Syntax::TFunction* Syntax::_parseFunction() {
  return nullptr;
}

Syntax::TStruct* Syntax::_parseStruct() {
  return nullptr;
}
