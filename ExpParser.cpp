#include "ExpParser.h"

const std::map<std::string, int64_t> ExpParser::_priorityTable = {
  {"fn", 0}, //done
  {"s++", 1},//done
  {"s--", 1}, //done
  {".", 1}, 
  {"[]", 1}, 
  {"p++", 2}, //done
  {"p--", 2}, //done
  {"u+", 2}, //done
  {"u-", 2}, //done
  {"!", 2}, 
  {"~", 2}, 
  {"u*", 2}, //done
  {"u&", 2}, 
  {"^", 3}, //done
  {"b*", 4}, //done
  {"/", 4}, //done
  {"%", 4}, 
  {"b+", 5}, // done
  {"b-", 5}, //done
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
  // TODO check code in pastebin
  lex->decrementTokenItern();
}

void ExpParser::_detectAction(Token* currentToken, Token* previosToken, std::deque<Token*>& deque) {
  if (previosToken == nullptr) {
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID || currentToken->type == Type::LITERAL) {
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
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID || currentToken->type == Type::LITERAL) {
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
    } else if (currentToken->type == Type::OPERATOR) {
      _pushToDeque(currentToken, deque);
    } else {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
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
  if (currentToken->type == Type::NUMBER) {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::NUMBER || previosToken->type == Type::ID ||
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "s++" ||
      previosToken->lexem == "s--" || previosToken->lexem == "p++" || previosToken->lexem == "p--" ||
      previosToken->lexem == "." || previosToken->lexem == "u*" || previosToken->lexem == "u&") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->type == Type::LITERAL) {
    if (!(previosToken->lexem == "b+" || previosToken->lexem == "=" || previosToken->lexem == "==" || 
      previosToken->lexem == "," || previosToken->lexem == "+=" || previosToken->lexem == "!=" ||
      previosToken->lexem == "and" || previosToken->lexem == "or" || previosToken->lexem == "u&")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->type == Type::ID) {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::NUMBER || previosToken->type == Type::ID ||
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "s++" ||
      previosToken->lexem == "s--") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "+") {
    if (previosToken->lexem == "u*" || previosToken->lexem == "u&" || previosToken->lexem == "p++" ||
      previosToken->lexem == "p--" || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "-") {
    if (previosToken->type == Type::LITERAL || previosToken->lexem == "u*" || previosToken->lexem == "u&" ||
      previosToken->lexem == "p++" || previosToken->lexem == "p--" || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "*") {
    if (previosToken->type == Type::LITERAL || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "/") {
    if (!(previosToken->type == Type::NUMBER || previosToken->type == Type::ID || previosToken->lexem == "]" || 
      previosToken->lexem == ")" || previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "++") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::NUMBER || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "--") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::NUMBER || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "^") {
    if (!(previosToken->type == Type::NUMBER || previosToken->type == Type::ID || previosToken->lexem == "]" ||
      previosToken->lexem == ")" || previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "(") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::NUMBER || previosToken->lexem == "s++" || 
      previosToken->lexem == "s--" || previosToken->lexem == ")" || previosToken->lexem == "]" || 
      previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "[") {
    if (!(previosToken->type == Type::LITERAL || previosToken->type == Type::ID || previosToken->lexem == "s++" ||
      previosToken->lexem == "s--" || previosToken->lexem == "]" || previosToken->lexem == ")")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == ")") {
    if (true) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "]") {
    if (true) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "+") {
    if (true) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  }
}

void ExpParser::_checkUnary(Token* currentToken, Token* previosToken) {
  if (previosToken->type == Type::NUMBER || previosToken->type == Type::ID || previosToken->lexem == ")" ||
    previosToken->lexem == "]" || previosToken->lexem == "s++" || previosToken->lexem == "s--" || 
    previosToken->type == Type::LITERAL) {
    if (currentToken->lexem == "+") {
      currentToken->lexem = "b+";
    } else if (currentToken->lexem == "-") {
      currentToken->lexem = "b-";
    } else if (currentToken->lexem == "*") {
      currentToken->lexem = "b*";
    } else {
      currentToken->lexem = "b&";
    }
  } else {
    if (currentToken->lexem == "+") {
      currentToken->lexem = "u+";
    } else if (currentToken->lexem == "-") {
      currentToken->lexem = "u-";
    } else if (currentToken->lexem == "*") {
      currentToken->lexem = "u*";
    } else {
      currentToken->lexem = "u&";
    }
  }
}

void ExpParser::_checkPrefix(Token* currentToken, Token* previosToken) {
  if (previosToken->type == Type::ID || previosToken->lexem == ")" || previosToken->lexem == "]") {
    if (currentToken->lexem == "++") {
      currentToken->lexem == "s++";
    } else {
      currentToken->lexem == "s--";
    }
  } else {
    if (currentToken->lexem == "++") {
      currentToken->lexem == "p++";
    } else {
      currentToken->lexem == "p--";
    }
  }
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
