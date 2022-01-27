#include "Syntax.h"

Syntax::Syntax(Lex* lex) {
  this->lex = lex;
  program = new TProgram();
  try {
  _parseGeneral();
  } catch (SyntaxError error) {
    error.What();
    system("pause");
	}
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
				program->program.push_back(_parseFunctionWithStruct(token));
			} else {
				lex->decrementTokenItern(2);
        throw SyntaxError(token->_line, "unexpecte id");
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

void Syntax::_parseExpression(Exp& exp) {
}

Syntax::TInit* Syntax::_parseInit() {
	TInit* init = new TInit();
	init->type = _parseType();

	do {
		Token* token = lex->getNextToken();
		if (token->type != Type::ID) {
			throw SyntaxError(token->_line, "expected id");
		}
		Token* name = token;
		token = lex->getNextToken();
		if (token->lexem == ",") {
			init->variables.push_back(new TInit::_variable{ name, Exp() });
			continue;
		}

		if (token->lexem == "=") {
			init->variables.push_back(new TInit::_variable{ name, Exp() });
			_parseExpression(init->variables[init->variables.size() - 1]->exp);
			token = lex->getNextToken();
			if (token->lexem == ",") {
				continue;
			}
			if (token->lexem == ";") {
				break;
			}
		}

		if (token->lexem != ";") {
			throw SyntaxError(token->_line, "expected ;");
		}
		init->variables.push_back(new TInit::_variable{ name, Exp() });
		break;
	} while (true);

	return init;
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

void Syntax::_parseParameters(std::vector<_parameter*>& parameters) {
	Token* token;
	bool flag = true; // check default params
	do {
		token = lex->getNextToken();
		if (token->type != Type::TYPE) {
			throw SyntaxError(token->_line, "expected Type");
		}
		lex->decrementTokenItern();
		TType* type = _parseType();
		token = lex->getNextToken();
		if (token->type != Type::ID) {
			throw SyntaxError(token->_line, "expected id");
		}
		Token* name = token;
		if (flag) {
			token = lex->getNextToken();
			if (token->lexem != "=") {
				flag = false;
				parameters.push_back(new _parameter{ type, name, Exp() });
				continue;
			}
			parameters.push_back(new _parameter{ type, name, Exp() });
			_parseExpression(parameters[parameters.size() - 1]->exp);
		} else {
			parameters.push_back(new _parameter{ type, name, Exp() });
		}
		token = lex->getNextToken();
	} while (token->lexem == ",");
	lex->decrementTokenItern();
}

Syntax::TFunction* Syntax::_parseFunction(TFunction* function) {
	if (function == nullptr) {
		function = new TFunction();
	}

	Token* token = lex->getNextToken();
	if (token->type != Type::ID) {
		throw SyntaxError(token->_line, "expected id");
	}
	function->nameFunction = token; // add in table with pointer

	token = lex->getNextToken();
	if (token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}
	token = lex->getNextToken();
	if (token->lexem != ")") {
		lex->decrementTokenItern();
		_parseParameters(function->parameters);
		token = lex->getNextToken();
		if (token->lexem != ")") {
			throw SyntaxError(token->_line, "expected )");
		}
	}

	token = lex->getNextToken();
	if (token->lexem != "->") {
		throw SyntaxError(token->_line, "expected ->");
	}
	
	token = lex->getNextToken();
	if (token->type != Type::TYPE) {
		throw SyntaxError(token->_line, "expected Type");
	}
	lex->decrementTokenItern();
	function->type = _parseType();

	token = lex->getNextToken();
	if (token->lexem == ";") {
		return function; // TODO add in table that function not definition
	}

	function->body = _parseBlock();

	return function;
}

Syntax::TFunction* Syntax::_parseFunctionWithStruct(Token* name) {
	TFunction* function = new TFunction();
	function->nameStruct = name;
	return _parseFunction(function);
}

Syntax::TStruct* Syntax::_parseStruct() {
	TStruct* tstruct = new TStruct();
	Token* token = lex->getNextToken();
	if (token->type != Type::ID) {
		throw SyntaxError(token->_line, "expected id");
	}

	tstruct->name = token; // TODO add to table
	
	token = lex->getNextToken();
	if (token->lexem == ";") {
		return tstruct;
	}

	if (token->lexem != "<") {
		throw SyntaxError(token->_line, "expected <");
	}
	token = lex->getNextToken();
	if (token->lexem != ">") {
		lex->decrementTokenItern();
		_parseParameters(tstruct->parameters);
		token = lex->getNextToken();
		if (token->lexem != ">") {
			throw SyntaxError(token->_line, "expected >");
		}
	}

	return tstruct;
}

