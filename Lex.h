#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#define _getNextToken() lex->getNextToken() == nullptr ? throw( SyntaxError("unexpected end of file")) : (lex->decrementTokenItern(), lex->getNextToken())

enum class Type {
	KEYWORD,
	ID,
	TYPE,
	OPERATOR,
	NUMBER,
	LITERAL,
	PUNKTUATION,
	CHAR,
	UNEXPECTED
};

struct Token {
	Type type;
	std::string lexem = "";
	int64_t _line = 0;
	int64_t _column = 0;
	void* atribute = nullptr;
};


class Lex {
public:
	Lex(char * input);
	Token* getNextToken();
	void decrementTokenItern(int64_t n = 1);
private:
	void scan(std::string line);
	void lexAnalyse(std::string& line);

	std::vector<Token*> _tokens{};
	std::vector<Token*>::iterator _nowToken = _tokens.begin();

	std::fstream _file;
	//std::string _filePath = "D:\\Translator\\";
	std::string _filePath = "C:\\Users\\Nik\\source\\repos\\Translator\\";
	std::string _fullRegex = "";

	int64_t __line = 0;

	struct _TypesTemplates {
		Type _type;
		std::string _template;
	};

	static std::string _MakeRegexTemplateByList(const std::vector<std::string>& list) noexcept;
	static std::string _addSleshes(const std::string& ar) noexcept;
	static std::string _debug(Type type) noexcept;

	const static std::vector<std::string> _keywordsList;
	const static std::vector<std::string> _typesList;
	const static std::vector<std::string> _operatorsList;
	const static std::vector<std::string> _punctuationList;

	const static std::vector<_TypesTemplates> _typeTemplates;

};
