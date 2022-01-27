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
	TSingleKeyWord* singleKeyWord = new TSingleKeyWord();
	Token* token = lex->getNextToken();
	if(token->lexem == "break" ||
		token->lexem == "continue"
		) {
		singleKeyWord->keyWord = token;
	} else {
		throw SyntaxError(token->_line, "expected key word");
	}
	token = lex->getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token->_line, "expected ;");
	}
	return singleKeyWord;
}

Syntax::TType* Syntax::_parseType() {
	TType* type = new TType();

	Token* token = lex->getNextToken();
	if(token->type != Type::TYPE) {
		throw SyntaxError(token->_line, "expected type");
	}

	type->typrStr.append(token->lexem);

	token = lex->getNextToken();
	if(token->lexem != "*") {
		if(token->lexem != "&") {
			return type;
		} else {
			type->typrStr.append(token->lexem);
			return type;
		}
	}
	while (token->lexem == "*") {
		type->typrStr.append(token->lexem);
	}

	token = lex->getNextToken();
	if(token->lexem != "&") {
		return type;
	} else {
		type->typrStr.append(token->lexem);
		return type;
	}

	return nullptr;
}

// TODO //
Syntax::TBlock* Syntax::_parseBlock() {
  return nullptr;
}

// TODO //
void Syntax::_parseExpression(Exp& exp) {
	return;
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
	TIf* tif = new TIf();

	Token* token = lex->getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}
	_parseExpression(tif->condition);
	token = lex->getNextToken();

	if(token->lexem != ")") {
		throw SyntaxError(token->_line, "expected )");
	}

	tif->body = _parseBlock();

	token = lex->getNextToken();
	if(token->lexem != "else") {
		return tif;
	}

	tif->elseBody = _parseBlock();

	return tif;
}

Syntax::TWhile* Syntax::_parseWhile() {
	TWhile* twhile = new TWhile();

	Token* token = lex->getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}
	_parseExpression(twhile->condition);
	token = lex->getNextToken();

	if(token->lexem != ")") {
		throw SyntaxError(token->_line, "expected )");
	}

	twhile->body = _parseBlock();

	return twhile;
}

Syntax::TFor* Syntax::_parseFor() {
	TFor* tfor = new TFor();

	Token* token = lex->getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}

	token = lex->getNextToken();
	if(token->type == Type::TYPE) {
		lex->decrementTokenItern();
		tfor->init = _parseInit();
	} else {
		_parseExpression(tfor->exp1);
	}
	token = lex->getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token->_line, "expected ;");
	}
	_parseExpression(tfor->exp2);
	token = lex->getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token->_line, "expected ;");
	}
	_parseExpression(tfor->exp3);


	token = lex->getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token->_line, "expected )");
	}

	tfor->body = _parseBlock();

	return tfor;
}

Syntax::TForEach* Syntax::_parseForEach() {
	TForEach* tfor = new TForEach();

	Token* token = lex->getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}

	token = lex->getNextToken();
	if(token->type == Type::TYPE) {
		lex->decrementTokenItern();
		tfor->init = _parseInit();
	} else {
		_parseExpression(tfor->exp1);
	}
	token = lex->getNextToken();
	if(token->lexem != "->") {
		throw SyntaxError(token->_line, "expected ->");
	}
	_parseExpression(tfor->exp2);
	
	token = lex->getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token->_line, "expected )");
	}

	tfor->body = _parseBlock();

	return tfor;

}

Syntax::TPrint* Syntax::_parsePrint() {
	TPrint* tprint = new TPrint();

	Token* token = lex->getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}

	_parseExpression(tprint->condition);

	token = lex->getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token->_line, "expected )");
	}

	token = lex->getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token->_line, "expected ;");
	}

	return tprint;
}

Syntax::TRead* Syntax::_parseRead() {
	TRead* tread = new TRead();

	Token* token = lex->getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token->_line, "expected (");
	}

	_parseExpression(tread->condition);

	token = lex->getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token->_line, "expected )");
	}

	token = lex->getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token->_line, "expected ;");
	}

	return tread;
}

// TODO //
Syntax::TMatch* Syntax::_parseMatch() {
  return nullptr;
}  

//not completely finished //
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

//not completely finished //
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

//not completely finished //
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

Syntax::TReturn* Syntax::_parseReturn() {
	TReturn* treturn = new TReturn();

	_parseExpression(treturn->exp);
	return treturn;
}

