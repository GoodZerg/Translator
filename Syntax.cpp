#include "Syntax.h"

Syntax::Syntax(Lex* lex) {
  this->lex = lex;
  program = new TProgram();
  _parseGeneral();
}

void Syntax::_parseGeneral() {  // parse lexem to syntax tree
	Token* token = nullptr;
	while (token = lex->getNextToken(), token != nullptr) {
		if (token->type == Type::KEYWORD) {
			if (token->lexem == "fn") {
				program->program.push_back(_parseFunction());
			} else if (token->lexem == "struct") {
				program->program.push_back(_parseStruct());
			}
		} else if (token->type == Type::TYPE) {
			lex->decrementTokenItern();
			program->program.push_back(_parseInit());
		} else if (token->type == Type::ID) {
			Token* token1 = lex->getNextToken();
			Token* token2 = lex->getNextToken();
			if (token1->lexem == "::" && token2->lexem == "fn") {
				lex->decrementTokenItern(3);
				program->program.push_back(_parseFunctionWithStruct());
			} else {
				lex->decrementTokenItern(2);  //TODO throw error unexpecte id
			}
		}
	}
}

Syntax::TSingleKeyWord* Syntax::_parseSingleKeyWord() {
	return nullptr;
}

Syntax::TType* Syntax::_parseType() {
  return nullptr;
}

Syntax::TBlock* Syntax::_parseBlock() {
  return nullptr;
}

Syntax::TExp* Syntax::_parseExpression() {
  return nullptr;
}

Syntax::TInit* Syntax::_parseInit() {
  return nullptr;
}

Syntax::TIf* Syntax::_parseIf() {
  return nullptr;
}

Syntax::TWhile* Syntax::_parseWhile() {
  return nullptr;
}

Syntax::TFor* Syntax::_parseFor() {
  return nullptr;
}

Syntax::TForEach* Syntax::_parseForEach() {
  return nullptr;
}

Syntax::TPrint* Syntax::_parsePrint() {
  return nullptr;
}

Syntax::TRead* Syntax::_parseRead() {
  return nullptr;
}

Syntax::TMatch* Syntax::_parseMatch() {
  return nullptr;
}

Syntax::TFunction* Syntax::_parseFunction() {
	Token* token = lex->getNextToken();
	TFunction* function = new TFunction();

	if (token->type != Type::ID) {
		; // TODO throw error
	}
	function->nameFunction = token; // add in table with pointer

}

Syntax::TFunction* Syntax::_parseFunctionWithStruct() {
	return nullptr;
}

Syntax::TStruct* Syntax::_parseStruct() {
  return nullptr;
}

