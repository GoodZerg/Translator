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
  {"cast", 2},  //done + +
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
  {"(", 14}   //done 🤡
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
    lex->decrementTokenItern(2);
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
    lex->decrementTokenItern(2);
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
    lex->decrementTokenItern(2);
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

void ExpParser::_detectAction(Token*& currentToken, Token*& previosToken, std::deque<Token*>& deque) {
  if (previosToken == nullptr) {
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID || 
      currentToken->type == Type::LITERAL || currentToken->type == Type::CHAR) {
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
    if (currentToken->type == Type::NUMBER || currentToken->type == Type::ID ||
      currentToken->type == Type::LITERAL || currentToken->type == Type::CHAR) {
      _addToPolis(currentToken);
    } else if (currentToken->type == Type::TYPE) {
      _addToPolis(new Token{ Type::TYPE , currentToken->lexem, 0, 0, nullptr });
      _pushToDeque(new Token{ Type::OPERATOR , "cast", 0, 0, nullptr }, deque);
    } else if (currentToken->lexem == "+" || currentToken->lexem == "-" || currentToken->lexem == "*" ||
      currentToken->lexem == "&") {
      if ((currentToken->lexem == "*" || currentToken->lexem == "&") && previosToken->type == Type::TYPE) {
        _addToPolis(currentToken->lexem);
        currentToken = previosToken;
        return;
      }
      _checkUnary(currentToken, previosToken);
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == "++" || currentToken->lexem == "--") {
      _checkPrefix(currentToken, previosToken);
      _pushToDeque(currentToken, deque);
    } else if (currentToken->lexem == ")") {
      --_brackets;
      if (!(previosToken->type == Type::TYPE)) {
        _descentToBracket(currentToken, deque);
      }
    } else if (currentToken->lexem == "(") {
      ++_brackets;
      if (previosToken->type == Type::ID) {
        polis.back()->lexem = "0_" + polis.back()->lexem;
        _pushToDeque(new Token{ Type::OPERATOR , "fn", 0, 0, nullptr }, deque);
        //currentToken->type == Type::UNEXPECTED;
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

void ExpParser::_addToPolis(std::string s) {
  polis.back()->lexem += s;
}

void ExpParser::_pushToDeque(Token* token, std::deque<Token*>& deque) {
  if (token->lexem == "(") {
    deque.push_back(token);
    return;
  }
  if (token->lexem[0] == 'u') {
    while (!deque.empty()) {
      if (_priorityTable.at(token->lexem) > _priorityTable.at(deque.back()->lexem)) {
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
  } else {
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
}

void ExpParser::_checkErrors(Token* currentToken, Token* previosToken) {
  if (currentToken->type == Type::TYPE) {
    if (previosToken->lexem != "(") {
      throw SyntaxError(currentToken, "wrong syntax cast");
    }
  } else if (currentToken->type == Type::NUMBER || currentToken->type == Type::CHAR) {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::NUMBER || previosToken->type == Type::ID ||
      previosToken->type == Type::TYPE || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) ||
      previosToken->lexem == "]" || previosToken->lexem == "s++" ||
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
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::NUMBER || previosToken->type == Type::ID ||
      previosToken->type == Type::TYPE || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) ||
      previosToken->lexem == "]" || previosToken->lexem == "s++" ||
      previosToken->lexem == "s--") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "+") {
    if (previosToken->type == Type::TYPE || previosToken->lexem == "u*" || 
      previosToken->lexem == "u&" || previosToken->lexem == "p++" || 
      previosToken->lexem == "p--" || previosToken->lexem == "." ||
      (previosToken->lexem == ")" && polis.back()->type == Type::TYPE)) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "-") {
    if (previosToken->type == Type::LITERAL || previosToken->lexem == "u*" || 
      previosToken->lexem == "u&" || previosToken->lexem == "p++" || 
      previosToken->lexem == "p--" || previosToken->lexem == "." ||
      (previosToken->lexem == ")" && polis.back()->type == Type::TYPE)) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "*") {
    if (previosToken->type == Type::LITERAL || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "/" || currentToken->lexem == "%") {
    if (!(previosToken->type == Type::NUMBER || previosToken->type == Type::ID || 
      previosToken->type == Type::CHAR || previosToken->lexem == "]" || 
      previosToken->lexem == "s++" || previosToken->lexem == "s--" || 
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE))) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "++") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::NUMBER || previosToken->type == Type::TYPE || 
      previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "--") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::NUMBER || previosToken->type == Type::TYPE || 
      previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "^") {
    if (!(previosToken->type == Type::NUMBER || previosToken->type == Type::ID || 
      previosToken->type == Type::CHAR || previosToken->lexem == "]" || 
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) ||
      previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "(") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::NUMBER || previosToken->type == Type::TYPE || 
      previosToken->lexem == "s++" || previosToken->lexem == "s--" || 
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) || 
      previosToken->lexem == "]" || previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "[") {
    if (!(previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::ID || previosToken->lexem == "s++" || 
      previosToken->lexem == "s--" || previosToken->lexem == "]" || 
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE))) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == ")") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->type == Type::UNEXPECTED || previosToken->type == Type::TYPE || 
      previosToken->lexem == "]" || previosToken->lexem == ")" ||
      previosToken->lexem == "s++" || previosToken->lexem == "s--" || 
      previosToken->lexem == "(")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "]") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->lexem == "]" || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) ||
      previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == ",") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) || previosToken->lexem == "]" || 
      previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "=" || currentToken->lexem == "^=" || currentToken->lexem == "%=" ||
    currentToken->lexem == "/=" || currentToken->lexem == "*=" || currentToken->lexem == "|=" || 
    currentToken->lexem == "&=" || currentToken->lexem == "+=" || currentToken->lexem == "-=") {
    if (!(previosToken->type == Type::ID || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) ||
      previosToken->lexem == "]")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "==" || currentToken->lexem == ">" || currentToken->lexem == ">=" ||
    currentToken->lexem == "<" || currentToken->lexem == "<=" || currentToken->lexem == "!=") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) || previosToken->lexem == "]" || 
      previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "and" || currentToken->lexem == "or") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::CHAR || previosToken->type == Type::LITERAL ||
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) || previosToken->lexem == "]" ||
      previosToken->lexem == "s++" || previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "|") {
    if (!(previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::CHAR || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) ||
      previosToken->lexem == "]" || previosToken->lexem == "s++" || 
      previosToken->lexem == "s--")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "&") {
    if (previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      previosToken->lexem == "]" || previosToken->lexem == "u&" || 
      previosToken->lexem == "u-" || previosToken->lexem == "p++" || 
      previosToken->lexem == "p--" || previosToken->lexem == "s--" || 
      previosToken->lexem == "s++" || previosToken->lexem == "~" || 
      previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == "!") {
    if (previosToken->type == Type::ID || previosToken->type == Type::NUMBER || 
      previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR ||
      (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) || previosToken->lexem == "]" || 
      previosToken->lexem == "u&" || previosToken->lexem == "u*" || 
      previosToken->lexem == "p++" || previosToken->lexem == "p--" ||
      previosToken->lexem == "s--" || previosToken->lexem == "s++" || 
      previosToken->lexem == ".") {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  } else if (currentToken->lexem == ".") {
    if (!(previosToken->type == Type::ID || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE) || 
      previosToken->lexem == "]")) {
      throw SyntaxError(currentToken, "unexpected operation"); // TODO rename error
    }
  }
}

void ExpParser::_checkUnary(Token* currentToken, Token* previosToken) {
  if ((previosToken->type == Type::NUMBER || previosToken->type == Type::ID || previosToken->lexem == ")" ||
    previosToken->lexem == "]" || previosToken->lexem == "s++" || previosToken->lexem == "s--" || 
    previosToken->type == Type::LITERAL || previosToken->type == Type::CHAR) && polis.back()->type != Type::TYPE) {
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
  if (previosToken->type == Type::ID || (previosToken->lexem == ")" && polis.back()->type != Type::TYPE)
    || previosToken->lexem == "]") {
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
  if (deque.back()->lexem == "(") {
    deque.pop_back();
    if (deque.back()->lexem != "fn" && polis.back()->type != Type::TYPE) {
      throw SyntaxError(token, "empty brackets");
    }
  } else {
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
}
