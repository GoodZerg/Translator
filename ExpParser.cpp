#include "ExpParser.h"

const std::map<std::string, int64_t> ExpParser::_priorityTable = {
  {"fn", 0},  //done +
  {"s++", 1}, //done + +
  {"s--", 1}, //done + +
  {".", 1},   //done +
  {"[]", 1},  //done +
  {"p++", 2}, //done + +
  {"p--", 2}, //done + +
  {"u+", 2},  //done + +
  {"u-", 2},  //done + +
  {"!", 2},   //done +
  {"~", 2},   //done +
  {"u*", 2},  //done + +
  {"u&", 2},  //done + +
  {"^", 3},   //done +
  {"b*", 4},  //done +
  {"/", 4},   //done +
  {"%", 4},   //done +
  {"b+", 5},  //done +
  {"b-", 5},  //done +
  {">", 6},   //done +
  {"<", 6},   //done +
  {">=", 6},  //done +
  {"<=", 6},  //done +
  {"==", 7},  //done +
  {"!=", 7},  //done +
  {"b&", 8},  //done +
  {"|", 9},   //done +
  {"and", 10},//done + +
  {"or", 11}, //done + +
  {"+=", 12}, //done +
  {"-=", 12}, //done +
  {"*=", 12}, //done +
  {"/=", 12}, //done +
  {"%=", 12}, //done +
  {"&=", 12}, //done +
  {"|=", 12}, //done +
  {"^=", 12}, //done +
  {"=", 12},  //done +
  {",", 13},  //done +
  {"(", 14}   //done -
};

ExpParser::ExpParser(Lex* lex, std::string end_symbol) :_lex(lex) {
  std::deque<Token*> deque;
  Token *previosToken = nullptr, *currentToken = nullptr;
  if (end_symbol == "exp") {
    while (currentToken = _getNextToken(), currentToken->lexem != ";") {
      _detectAction(currentToken, previosToken, deque);
      previosToken = currentToken;
    }
  } else if (end_symbol == "fn") {
    Token* tmp = nullptr;
    while (currentToken = _getNextToken(), tmp = _getNextToken(), currentToken->lexem != "," &&
      (currentToken->lexem != ")" || tmp->lexem != "->")) {
      lex->decrementTokenItern();
      _detectAction(currentToken, previosToken, deque);
      previosToken = currentToken;
    }
    lex->decrementTokenItern();
  } else if (end_symbol == "if") {
    Token* tmp = nullptr;
    while (currentToken = _getNextToken(), tmp = _getNextToken(), currentToken->lexem != ")" && tmp->lexem != "{") {
      lex->decrementTokenItern();
      _detectAction(currentToken, previosToken, deque);
      previosToken = currentToken;
    }
    lex->decrementTokenItern();
  } else if (end_symbol == "init") {
    while (currentToken = _getNextToken(), (currentToken->lexem != "," || _brackets != 0) && currentToken->lexem != ";") {
      _detectAction(currentToken, previosToken, deque);
      previosToken = currentToken;
    }
  } else if (end_symbol == "print") {
    Token* tmp = nullptr;
    while (currentToken = _getNextToken(), tmp = _getNextToken(), currentToken->lexem != ")" && tmp->lexem != ";") {
      lex->decrementTokenItern();
      _detectAction(currentToken, previosToken, deque);
      previosToken = currentToken;
    }
    lex->decrementTokenItern();
  } else if (end_symbol == "struct") {
    Token* tmp = nullptr;
    while (currentToken = _getNextToken(), tmp = _getNextToken(), currentToken->lexem != "," &&
      (currentToken->lexem != ">" || tmp->lexem != ";")) {
      lex->decrementTokenItern();
      _detectAction(currentToken, previosToken, deque);
      previosToken = currentToken;
    }
    lex->decrementTokenItern();
  }

  deque.push_front(new Token{ Type::PUNKTUATION , "(", 0, 0, nullptr });

  while (!deque.empty()) {
    if (deque.back()->lexem != "(") {
      polis.push_back(deque.back());
      previosToken = deque.back();
      deque.pop_back();
      continue;
    }
    break;
  }
  if (deque.empty()) {
    throw SyntaxError(previosToken, "expected ("); // TODO rename error
  }
  deque.pop_back();

  lex->decrementTokenItern();
}

void ExpParser::_detectAction(Token* currentToken, Token* previosToken, std::deque<Token*>& deque) {
  if (previosToken == nullptr) {
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID || currentToken->type == Type::LITERAL) {
      _addToPolis(currentToken);
    } else if (currentToken->lexem == "~" || currentToken->lexem == "!" || currentToken->lexem == "(") {
      if (currentToken->lexem == "(") {
        ++_brackets;
      }
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "+") {
      currentToken->lexem = "u+";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "-") {
      currentToken->lexem = "u-";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "++") {
      currentToken->lexem = "p++";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "--") {
      currentToken->lexem = "p--";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "*") {
      currentToken->lexem = "u*";
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "&") {
      currentToken->lexem = "u&";
      _pushToDeque(currentToken, deque);
    } else {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
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
      --_brackets;
      _descentToBracket(currentToken, deque);
    } else if (currentToken->lexem == "(") {
      ++_brackets;
      if (previosToken->type == Type::ID) {
        _pushToDeque(new Token{ Type::KEYWORD , "fn", 0, 0, nullptr }, deque);
        currentToken->type == Type::UNEXPECTED;
      }
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "]") {
      _descentToIndex(currentToken, deque);
    } else if (currentToken->lexem == "[") {
      currentToken->lexem = "[]";
      _pushToDeque(currentToken, deque);
      _pushToDeque(new Token{ Type::PUNKTUATION , "(", 0, 0, nullptr }, deque);
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
  if (token->lexem == "(") {
    deque.push_back(token);
    return;
  }
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
  } else if (currentToken->lexem == "/" || currentToken->lexem == "%") {
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
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL ||
      previosToken->type == Type::UNEXPECTED || previosToken->lexem == "]" || previosToken->lexem == ")" ||
      previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "]") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL ||
      previosToken->lexem == "]" || previosToken->lexem == ")" || previosToken->lexem == "s++" || 
      previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == ",") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL || 
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "s++" || 
      previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "=" || currentToken->lexem == "^=" || currentToken->lexem == "%=" ||
    currentToken->lexem == "/=" || currentToken->lexem == "*=" || currentToken->lexem == "|=" || 
    currentToken->lexem == "&=" || currentToken->lexem == "+=" || currentToken->lexem == "-=") {
    if (!(previosToken->type == Type::ID || previosToken->lexem == ")" || previosToken->lexem == "]")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "==" || currentToken->lexem == ">" || currentToken->lexem == ">=" ||
    currentToken->lexem == "<" || currentToken->lexem == "<=" || currentToken->lexem == "!=") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL ||
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "s++" ||
      previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "and" || currentToken->lexem == "or") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL ||
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "s++" ||
      previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "|" || currentToken->lexem == "b&") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->lexem == ")" || 
      previosToken->lexem == "]" || previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "u&") {
    if (previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL ||
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "u&" ||
      previosToken->lexem == "u-" || previosToken->lexem == "p++" || previosToken->lexem == "p--" ||
      previosToken->lexem == "s--" || previosToken->lexem == "s++" || previosToken->lexem == "~" || 
      previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "!") {
    if (previosToken->type == Type::ID || previosToken->type == Type::NUMBER || previosToken->type == Type::LITERAL ||
      previosToken->lexem == ")" || previosToken->lexem == "]" || previosToken->lexem == "u&" ||
      previosToken->lexem == "u*" || previosToken->lexem == "p++" || previosToken->lexem == "p--" ||
      previosToken->lexem == "s--" || previosToken->lexem == "s++" || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == ".") {
    if (!(previosToken->type == Type::ID || previosToken->lexem == ")" || previosToken->lexem == "]")) {
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
      currentToken->lexem = "s++";
    } else {
      currentToken->lexem = "s--";
    }
  } else {
    if (currentToken->lexem == "++") {
      currentToken->lexem = "p++";
    } else {
      currentToken->lexem = "p--";
    }
  }
}

void ExpParser::_descentToIndex(Token* token, std::deque<Token*>& deque) {
  while (!deque.empty()) {
    if (deque.back()->lexem != "(") {
      polis.push_back(deque.back());
      deque.pop_back();
      continue;
    }
    break;
  }
  if (deque.empty()) {
    throw SyntaxError(token, "expected ["); // TODO rename error
  }
  deque.pop_back();
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
