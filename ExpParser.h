#pragma once

#include <deque>
#include <map>
#include "Syntax.h"

class ExpParser {
public:
  ExpParser(Lex* lex);

  std::vector<Token*> polis;
private:
  void _detectAction(Token* currentToken, Token* previosToken, std::deque<Token*>& deque);

  void _addToPolis(Token* token);
  void _pushToDeque(Token* token, std::deque<Token*>& deque);
  void _checkErrors(Token* currentToken, Token* previosToken);
  void _checkUnary(Token* currentToken, Token* previosToken);
  void _checkPrefix(Token* currentToken, Token* previosToken);
  void _descentToIndex(Token* token, std::deque<Token*>& deque);
  void _descentToBracket(Token* token, std::deque<Token*>& deque);

  Lex* _lex;
  const static std::map<std::string, int64_t> _priorityTable;
};

