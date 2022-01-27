#include "ExpParser.h"
const std::map<std::string, int64_t> _priorityTable = { 
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
  {"b|", 9}, 
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
  Token* previous_token = nullptr, * current_token = nullptr;
  
  while (current_token = _lex->getNextToken(), current_token != nullptr || 
                                               current_token->lexem != ";") {
    _detectAction(current_token, previous_token, deque);
    previous_token = current_token;
  }

}

void ExpParser::_detectAction(Token* current_token, Token* previos_token, std::deque<Token*>& deque) {
  if (previos_token == nullptr) {
    if (current_token->type == Type::NUMBER || current_token->type == Type::ID) {
      _addToPolis(current_token);
    } else if (current_token->lexem == "-" || current_token->lexem == "+" || current_token->lexem == "++" ||
      current_token->lexem == "--" || current_token->lexem == "*" || current_token->lexem == "&" ||
      current_token->lexem == "~" || current_token->lexem == "!" || current_token->lexem == "(") {
      _pushToDeque(current_token, deque);
    } 
    ; // TODO throw error
  }
  
}

void ExpParser::_addToPolis(Token* token) {
  polis.push_back(token);
}

void ExpParser::_pushToDeque(Token* token, std::deque<Token*>& deque) {
  deque.push_back(token);
}
