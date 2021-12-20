#pragma once

#include <string>

struct Token;
enum class Type;

class Lex {
public:
	Lex();
	Token getNextToken();
private:
	void scan();
	void lexAnalyse();
};


struct Token {
	Type type;
	std::string Lexem = "";
	void* atribute = nullptr;
};

enum class Type {
	KEYWORD,
	ID,
	OPERATOR,
	NUMBER,
	LITERAL
};
