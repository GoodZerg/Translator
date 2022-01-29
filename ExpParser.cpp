#include "ExpParser.h"

const std::map<std::string, int64_t> ExpParser::_priorityTable = {
  {"fn", 0},
  {"s++", 1},
  {"s--", 1}, 
  {".", 1}, 
  {"[]", 1}, 
  {"p++", 2}, 
  {"p--", 2}, 
  {"u+", 2}, 
  {"u-", 2}, 
  {"!", 2}, 
  {"~", 2}, 
  {"u*", 2}, 
  {"u&", 2}, 
  {"^", 3}, 
  {"b*", 4}, 
  {"/", 4}, 
  {"%", 4}, 
  {"b+", 5}, 
  {"b-", 5}, 
  {">", 6}, 
  {"<", 6}, 
  {">=", 6}, 
  {"<=", 6}, 
  {"==", 7}, 
  {"!=", 7}, 
  {"b&", 8}, 
  {"|", 9}, 
  {"and", 10}, 
  {"or", 11}, 
  {"+=", 12}, 
  {"-=", 12}, 
  {"*=", 12}, 
  {"/=", 12}, 
  {"%=", 12}, 
  {"&=", 12}, 
  {"|=", 12}, 
  {"^=", 12}, 
  {"=", 12}, 
  {",", 13}
};

ExpParser::ExpParser(Lex* lex) :_lex(lex) {
  std::deque<Token*> deque;
  Token* previous_token = nullptr, * currentToken = nullptr;
  
  while (currentToken = _lex->getNextToken(), currentToken != nullptr || 
                                               currentToken->lexem != ";") {
    _detectAction(currentToken, previous_token, deque);
    previous_token = currentToken;
  }

}

void ExpParser::_detectAction(Token* currentToken, Token* previosToken, std::deque<Token*>& deque) {
  if (previosToken == nullptr) {
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID) {
      _addToPolis(currentToken);
    } else if (currentToken->lexem == "~" || currentToken->lexem == "!" || currentToken->lexem == "(") {
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "+") {
      currentToken->lexem = "u+";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "-") {
      currentToken->lexem = "u-";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "++") {
      currentToken->lexem = "u++";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "--") {
      currentToken->lexem = "u--";
      _pushToDeque(currentToken, deque);
    }
    throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
  } else {
    _checkErrors(currentToken, previosToken);
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID) {
      _addToPolis(currentToken);
    } else if (currentToken->lexem == "+" || currentToken->lexem == "-" || currentToken->lexem == "*" ||
      currentToken->lexem == "&") {
      _checkUnary(currentToken, previosToken);
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "++" || currentToken->lexem == "--") {
      _checkPrefix(currentToken, previosToken);
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == ")") {
      _descentToBracket(currentToken, deque);
    } else if (currentToken->lexem == "(") {
      if (previosToken->type == Type::ID) {
        currentToken->lexem = "fn";
      }
      _pushToDeque(currentToken, deque);
      _pushToDeque(new Token{ Type::PUNKTUATION , "(", 0, 0, nullptr}, deque);
    } else if (currentToken->lexem == "]") {
      _descentToIndex(currentToken, deque);
    } else if (currentToken->lexem == "[") {
      currentToken->lexem = "[]";
      _pushToDeque(currentToken, deque);
    }
    _pushToDeque(currentToken, deque);
  }
}

void ExpParser::_addToPolis(Token* token) {
  polis.push_back(token);
}

void ExpParser::_pushToDeque(Token* token, std::deque<Token*>& deque) {
  while (!deque.empty()) {
    if (_priorityTable.at(token->lexem) >= _priorityTable.at(deque.back()->lexem)) {
      polis.push_back(deque.back());
      deque.pop_back();
      continue;
    }
    deque.push_back(token);
    break;
  }
  if (deque.empty()) {
    deque.push_back(token);
  }
}

void ExpParser::_checkErrors(Token* currentToken, Token* previosToken) {

}

void ExpParser::_checkUnary(Token* currentToken, Token* previosToken) {
}

void ExpParser::_checkPrefix(Token* currentToken, Token* previosToken) {
}

void ExpParser::_descentToIndex(Token* token, std::deque<Token*>& deque) {
  while (!deque.empty()) {
    if (deque.back()->lexem != "[") {
      polis.push_back(deque.back());
      deque.pop_back();
      continue;
    }
    break;
  }
  if (deque.empty()) {
    throw SyntaxError(token, "expected ["); // TODO rename error
  }
  deque.back()->lexem = "[]";
}

void ExpParser::_descentToBracket(Token* token, std::deque<Token*>& deque) {
  while (!deque.empty()) {
    if (deque.back()->lexem != "(") {
      polis.push_back(deque.back());
      deque.pop_back();
      continue;
    }
    break;
  }
  if (deque.empty()) {
    throw SyntaxError(token, "expected ("); // TODO rename error
  }
  deque.pop_back();
}
