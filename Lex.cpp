#include "Lex.h"

const std::vector<std::string> Lex::_keywordsList = {

};

const std::vector<std::string> Lex::_typesList = {

};

const std::vector<std::string> Lex::_operatorsList = {

};

const std::vector<std::string> Lex::_punctuationList = {

};

const std::vector<Lex::_TypesTemplates> Lex::_typeTemplates = {
   {Type::KEYWORD,     Lex::_MakeRegexTemplateByList(Lex::_keywordsList)},
   {Type::ID,          "////////////////"},  // put id regex - template
   {Type::TYPE,        Lex::_MakeRegexTemplateByList(Lex::_typesList)},
   {Type::OPERATOR,    Lex::_MakeRegexTemplateByList(Lex::_operatorsList)},
   {Type::NUMBER,      "////////////////"}, // put number regex - template
   {Type::LITERAL,     "////////////////"}, // put literal regex - template
   {Type::PUNKTUATION, Lex::_MakeRegexTemplateByList(Lex::_punctuationList)}
};

Lex::Lex() {
  _file.open(_filePath);
  std::string line = " ";
  while (!_file.eof()) {
  _file >> line;
  scan(line);
  }
  _file.close();
}

Token* Lex::getNextToken() {
  return *_nowToken++;
}

void Lex::scan(std::string line) {
  // delete comments
   
  // use macro

  // delete 

  lexAnalyse(line);
}

void Lex::lexAnalyse(std::string& line) {
  // parse to tokens

}

std::string Lex::_MakeRegexTemplateByList(const std::vector<std::string>& list) noexcept {
  std::string answ = "";
  for (int i = 0; i < (static_cast<int>(list.size()) - 1); i++) {
    answ += "(\\Q" + list[i] + "\\E)" + "|";
  }
  if(list.size() != 0) answ += "(\\Q" + list[list.size() - 1] + "\\E)";
  return answ;
}
