#include "Lex.h"

#define BORDERED(a) "\\b"##a"\\b"


const std::vector<std::string> Lex::_keywordsList = {
  BORDERED("if"),
  BORDERED("for"),
  BORDERED("while"),
  BORDERED("match"),
  BORDERED("for each"),
  BORDERED("struct"),
  BORDERED("fn"),
  BORDERED("return"),
  BORDERED("read"),
  BORDERED("print")
};

const std::vector<std::string> Lex::_typesList = {
  BORDERED("si8"),
  BORDERED("si16"),
  BORDERED("si32"),
  BORDERED("si64"),
  BORDERED("si128"),
  BORDERED("ui8"),
  BORDERED("ui16"),
  BORDERED("ui32"),
  BORDERED("ui64"),
  BORDERED("ui128"),
  BORDERED("f32"),
  BORDERED("f64"),
  BORDERED("f128"),
  BORDERED("char"),
  BORDERED("string"),
  BORDERED("void")

};

const std::vector<std::string> Lex::_operatorsList = {
  _addSleshes("++"),
  _addSleshes("--"),
  _addSleshes("*"),
  _addSleshes("/"),
  _addSleshes("-"),
  _addSleshes("+"),
  _addSleshes("+="),
  _addSleshes("-="),
  _addSleshes("/="),
  _addSleshes("*="),
  _addSleshes("^="),
  _addSleshes("%="),
  _addSleshes("|="),
  _addSleshes("&="),
  _addSleshes("&"),
  _addSleshes("|"),
  _addSleshes(","),
  _addSleshes("~"),
  _addSleshes("and"),
  _addSleshes("or"),
  _addSleshes("=="),
  _addSleshes("!="),
  _addSleshes("["),
  _addSleshes("]")
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

Lex::Lex(char * input) {
  for (int i = 0; i < _typeTemplates.size(); ++i) {
    _fullRegex += "(" + _typeTemplates[i]._template + ")" + (i == _typeTemplates.size() - 1 ? "" : "|");
  }
  //std::cout << _fullRegex;
  if (input == 0) {
      std::cout << "NO FILE";
      exit(0);
  }
  std::string tmp = "";
  while (*input != 0) {
      tmp += *input;
      ++input;
  }
  _file.open(_filePath + tmp);
  if (!_file.is_open()) {
      std::cout << "CRINGE FILE DRUJE";
      exit(0);
  }
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
                index = it._type;
                break;
            }
        }
        if (match.str() == " ") continue;
        std::cout << match.str() << "   " << _debug(index) << "\n";
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

std::string Lex::_debug(Type type) noexcept{
    switch (type) {
        case Type::KEYWORD:
            return "KEYWORD";
            break;
        case Type::ID:
            return "ID";
            break;
        case Type::TYPE:
            return "TYPE";
            break;
        case Type::OPERATOR:
            return "OPERATOR";
            break;
        case Type::NUMBER:
            return "NUMBER";
            break;
        case Type::LITERAL:
            return "LITERAL";
            break;
        case Type::PUNKTUATION:
            return "PUNKTUATION";
            break;
        case Type::UNEXPECTED:
            return "UNEXPECTED";
            break;
        default:
            return "ABOBA";
    }
}
