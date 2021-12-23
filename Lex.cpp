#include "Lex.h"

#define BORDERED(a) "\\b"##a"\\b"

const std::vector<std::string> Lex::_keywordsList = {
  BORDERED("if"),
  BORDERED("for"),
  BORDERED("while"),
  BORDERED("this")
};

const std::vector<std::string> Lex::_typesList = {
  BORDERED("int"),
  BORDERED("dounble"),
  BORDERED("banana"),
  BORDERED("monkey")
};

const std::vector<std::string> Lex::_operatorsList = {
  _addSleshes("++"),
  _addSleshes("**"),
  _addSleshes("--"),
  _addSleshes("*"),
  _addSleshes("/"),
  _addSleshes("-"),
  _addSleshes("+")
};

const std::vector<std::string> Lex::_punctuationList = {
  _addSleshes(";"),
  _addSleshes(":"),
  _addSleshes("("),
  _addSleshes(")"),
  _addSleshes("{"),
  _addSleshes("}"),
  _addSleshes("=>") 
};

const std::vector<Lex::_TypesTemplates> Lex::_typeTemplates = {
   {Type::KEYWORD,     Lex::_MakeRegexTemplateByList(Lex::_keywordsList)},
   {Type::TYPE,        Lex::_MakeRegexTemplateByList(Lex::_typesList)},
   {Type::OPERATOR,    Lex::_MakeRegexTemplateByList(Lex::_operatorsList)},
   {Type::PUNKTUATION, Lex::_MakeRegexTemplateByList(Lex::_punctuationList)},
   {Type::ID,          R"(_*[a-zA-Z][_a-zA-Z0-9]*)"},  // put id regex - template
   {Type::NUMBER,      R"([0-9]+\.[0-9]+|[0-9]+)"}, // put number regex - template
   {Type::LITERAL,     R"(\"([^\"])*\")"}, // put literal regex - template
   {Type::UNEXPECTED,  R"(.)"}
};

Lex::Lex() {
  for (int i = 0; i < _typeTemplates.size(); ++i) {
    _fullRegex += "(" + _typeTemplates[i]._template + ")" + (i == _typeTemplates.size() - 1 ? "" : "|");
  }
  //std::cout << _fullRegex;
  _file.open(_filePath);
  std::string line = " ";
  while (!_file.eof()) {
    std::getline(_file, line);
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

  // delete spaces
  static const auto r = std::regex(R"(\s+)");
  line = std::regex_replace(line.data(), r, " ");
  lexAnalyse(line);
}

void Lex::lexAnalyse(std::string& line) {
  // parse to tokens
    std::regex Regex(_fullRegex);
    auto words_begin =
        std::sregex_iterator(line.begin(), line.end(), Regex);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        Type index = Type::UNEXPECTED;
        for (auto& it : _typeTemplates) {
            //std::cout << it._template << "\n";
            if (std::regex_match(match.str(), std::regex(it._template))) {
                std::cout << match.str() << "   " << (int)it._type << "\n";
                index = it._type;
                break;
            }
        }
        _tokens.push_back(new Token{ index, match.str() });
    }
}

std::string Lex::_MakeRegexTemplateByList(const std::vector<std::string>& list) noexcept {
  std::string answ = "";
  for (int i = 0; i < (static_cast<int>(list.size()) - 1); i++) {
    answ += list[i] + "|";
  }
  if(list.size() != 0) answ += list[list.size() - 1];
  return answ;
}

std::string Lex::_addSleshes(const std::string& ar) noexcept {
    std::string str("");
    for (auto i : ar) {
        str += "\\";
        str += i;
    }
    return str;
}
