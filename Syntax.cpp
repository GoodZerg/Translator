#include "Syntax.h"

Syntax::Syntax(Lex* lex) {
  this->lex = lex;
  program = new TProgram();
  try {
  _parseGeneral();
  } catch (SyntaxError error) {
    error.What();
		exit(0);
	}
}

void Syntax::_parseGeneral() {  // parse lexem to syntax tree
	Token* token = nullptr;
	while (token = lex->getNextToken(), token != nullptr) {
		if (token->type == Type::KEYWORD) {
			if (token->lexem == "fn") {
				Token* token1 = _getNextToken();
				Token* token2 = _getNextToken();
				if (token1->type == Type::ID && token2->lexem == "::") {
					program->program.push_back(_parseFunctionWithStruct(token));
				} else {
					lex->decrementTokenItern(2);
					program->program.push_back(_parseFunction());
				}
			} else if (token->lexem == "struct") {
				program->program.push_back(_parseStruct());
			}
		} else if (token->type == Type::TYPE || _checkTypeStructInit(token)) {
			lex->decrementTokenItern();
			program->program.push_back(_parseInit());
		}
	}
}

Syntax::TSingleKeyWord* Syntax::_parseSingleKeyWord() {
	TSingleKeyWord* singleKeyWord = new TSingleKeyWord();
	Token* token = _getNextToken();
	if(token->lexem == "break" ||
		token->lexem == "continue"
		) {
		singleKeyWord->keyWord = token;
	} else {
		throw SyntaxError(token, "expected key word");
	}
	token = _getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token, "expected ;");
	}
	return singleKeyWord;
}

Syntax::TType* Syntax::_parseType() {
	TType* type = new TType();

	Token* token = _getNextToken();
	if(token->type != Type::TYPE && !_checkTypeStructInit(token)) {
		throw SyntaxError(token, "expected type");
	}

	type->typrStr.append(token->lexem);

	token = _getNextToken();
	if(token->lexem != "*") {
		if(token->lexem != "&") {
			lex->decrementTokenItern();
			return type;
		} else {
			type->typrStr.append(token->lexem);
			return type;
		}
	}

	while (token->lexem == "*") {
		type->typrStr.append(token->lexem);
		token = _getNextToken();
	}

	if(token->lexem != "&") {
		lex->decrementTokenItern();
		return type;
	} else {
		type->typrStr.append(token->lexem);
		return type;
	}
}

Syntax::TBlock* Syntax::_parseBlock() {
	TBlock* block = new TBlock();


	Token* token = _getNextToken();
	if(token->lexem != "{") {
		throw SyntaxError(token, "expected {");
	}
	while(true) {
		token = _getNextToken();
		if(token->type == Type::KEYWORD) {
			if(token->lexem == "if") {
				block->nodes.push_back(_parseIf());
			} else if(token->lexem == "for") {
				block->nodes.push_back(_parseFor());
			} else if(token->lexem == "while") {
				block->nodes.push_back(_parseWhile());
			} else if(token->lexem == "match") {
				;						// TODO //
			} else if(token->lexem == "foreach") {
				block->nodes.push_back(_parseForEach());
			} else if(token->lexem == "return") {
				block->nodes.push_back(_parseReturn());
			} else if(token->lexem == "read") {
				block->nodes.push_back(_parseRead());
			} else if(token->lexem == "print") {
				block->nodes.push_back(_parsePrint());
			} else if(token->lexem == "continue" || token->lexem == "break") {
				lex->decrementTokenItern();
				block->nodes.push_back(_parseSingleKeyWord());
			} else {
				throw SyntaxError(token, "unexpected keyword");
			}
		} else if(token->type == Type::TYPE || _checkSecondID(token)) {
			lex->decrementTokenItern();
			block->nodes.push_back(_parseInit());
		} else if(token->lexem == "}"){
			return block;
		} else if(token->lexem == "{") {
			lex->decrementTokenItern();
			block->nodes.push_back(_parseBlock());
		} else {
			lex->decrementTokenItern();
			Exp* exp = nullptr;
			_parseExpression(exp, "exp");
			block->nodes.push_back(new TExp{ exp });
		}
	}


  return block;
}

// TODO //
void Syntax::_parseExpression(Exp*& exp, std::string end_symbol) {
	exp = new Exp(lex, end_symbol);
}

Syntax::TInit* Syntax::_parseInit() {
	TInit* init = new TInit();
	init->type = _parseType();

	do {
		Token* token = _getNextToken();
		if (token->type != Type::ID) {
			throw SyntaxError(token, "expected id");
		}
		Token* name = token;
		token = _getNextToken();
		if (token->lexem == ",") {
			init->variables.push_back(new TInit::_variable{ name, nullptr });
			continue;
		}

		if (token->lexem == "=") {
			init->variables.push_back(new TInit::_variable{ name, nullptr });
			_parseExpression(init->variables[init->variables.size() - 1]->exp, "init");
			token = _getNextToken();
			if (token->lexem == ",") {
				continue;
			}
			if (token->lexem == ";") {
				return init;
			}
		}

		if (token->lexem != ";") {
			throw SyntaxError(token, "expected ;");
		}
		init->variables.push_back(new TInit::_variable{ name, nullptr });
		return init;
	} while (true);
}

Syntax::TIf* Syntax::_parseIf() {
	TIf* tif = new TIf();

	Token* token = _getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}
	_parseExpression(tif->condition, "if");
	token = _getNextToken();

	if(token->lexem != ")") {
		throw SyntaxError(token, "expected )");
	}

	tif->body = _parseBlock();

	token = _getNextToken();
	if(token->lexem != "else") {
		lex->decrementTokenItern();
		return tif;
	}

	tif->elseBody = _parseBlock();

	return tif;
}

Syntax::TWhile* Syntax::_parseWhile() {
	TWhile* twhile = new TWhile();

	Token* token = _getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}
	_parseExpression(twhile->condition, "if");
	token = _getNextToken();

	if(token->lexem != ")") {
		throw SyntaxError(token, "expected )");
	}

	twhile->body = _parseBlock();

	return twhile;
}

Syntax::TFor* Syntax::_parseFor() {
	TFor* tfor = new TFor();

	Token* token = _getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}

	token = _getNextToken();
	if(token->type == Type::TYPE || _checkSecondID(token)) {
		lex->decrementTokenItern();
		tfor->init = _parseInit();
	} else {
		lex->decrementTokenItern();
		_parseExpression(tfor->exp1, "exp");
		token = _getNextToken();
		if (token->lexem != ";") {
			throw SyntaxError(token, "expected ;");
		}
	}
	
	_parseExpression(tfor->exp2, "exp");
	token = _getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token, "expected ;");
	}

	_parseExpression(tfor->exp3, "if");

	token = _getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token, "expected )");
	}

	tfor->body = _parseBlock();

	return tfor;
}

Syntax::TForEach* Syntax::_parseForEach() {
	TForEach* tfor = new TForEach();

	Token* token = _getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}

	token = _getNextToken();
	if(token->type == Type::TYPE || _checkTypeStructInit(token)) {
		lex->decrementTokenItern();
		tfor->type = _parseType();
		token = _getNextToken();
	}

	if (token->type != Type::ID) {
		throw SyntaxError(token, "expected ID");
	}

	token = _getNextToken();
	if(token->lexem != "->") {
		throw SyntaxError(token, "expected ->");
	}
	_parseExpression(tfor->exp1, "if");
	
	token = _getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token, "expected )");
	}

	tfor->body = _parseBlock();

	return tfor;

}

Syntax::TPrint* Syntax::_parsePrint() {
	TPrint* tprint = new TPrint();

	Token* token = _getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}

	_parseExpression(tprint->condition, "print");

	token = _getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token, "expected )");
	}

	token = _getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token, "expected ;");
	}

	return tprint;
}

Syntax::TRead* Syntax::_parseRead() {
	TRead* tread = new TRead();

	Token* token = _getNextToken();
	if(token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}

	_parseExpression(tread->condition, "print");

	token = _getNextToken();
	if(token->lexem != ")") {
		throw SyntaxError(token, "expected )");
	}

	token = _getNextToken();
	if(token->lexem != ";") {
		throw SyntaxError(token, "expected ;");
	}

	return tread;
}

// TODO //
Syntax::TMatch* Syntax::_parseMatch() {
  return nullptr;
}  

// not completely finished (need using table) //
void Syntax::_parseParameters(std::vector<_parameter*>& parameters, std::string end_symbol) {
	Token* token;
	bool flag = true; // check default params
	do {
		token = _getNextToken();
		if (token->type != Type::TYPE && !_checkTypeStructInit(token)) {
			throw SyntaxError(token, "expected Type");
		}
		lex->decrementTokenItern();
		TType* type = _parseType();
		token = _getNextToken();
		if (token->type != Type::ID) {
			throw SyntaxError(token, "expected id");
		}
		Token* name = token;
		if (flag) {
			token = _getNextToken();
			if (token->lexem != "=") {
				flag = false;
				parameters.push_back(new _parameter{ type, name, nullptr });
				continue;
			}
			parameters.push_back(new _parameter{ type, name, nullptr });
			_parseExpression(parameters[parameters.size() - 1]->exp, end_symbol);
		} else {
			parameters.push_back(new _parameter{ type, name, nullptr });
		}
		token = _getNextToken();
	} while (token->lexem == ",");
	lex->decrementTokenItern();
}

// not completely finished (need using table) //
Syntax::TFunction* Syntax::_parseFunction(TFunction* function) {
	if (function == nullptr) {
		function = new TFunction();
	}

	Token* token = _getNextToken();
	if (token->type != Type::ID) {
		throw SyntaxError(token, "expected id");
	}
	function->nameFunction = token; // add in table with pointer

	token = _getNextToken();
	if (token->lexem != "(") {
		throw SyntaxError(token, "expected (");
	}
	token = _getNextToken();
	if (token->lexem != ")") {
		lex->decrementTokenItern();
		_parseParameters(function->parameters, "fn");
		token = _getNextToken();
		if (token->lexem != ")") {
			throw SyntaxError(token, "expected )");
		}
	}

	token = _getNextToken();
	if (token->lexem != "->") {
		throw SyntaxError(token, "expected ->");
	}
	
	token = _getNextToken();
	if (token->type != Type::TYPE && !_checkTypeStructInit(token)){
		throw SyntaxError(token, "expected Type");
	}
	lex->decrementTokenItern();
	function->type = _parseType();

	token = _getNextToken();
	if (token->lexem == ";") {
		return function; // TODO add in table that function not definition
	}
	lex->decrementTokenItern();
	function->body = _parseBlock();

	return function;
}

Syntax::TFunction* Syntax::_parseFunctionWithStruct(Token* name) {
	TFunction* function = new TFunction();
	function->nameStruct = name;
	return _parseFunction(function);
}

// not completely finished (need using table) //
Syntax::TStruct* Syntax::_parseStruct() {
	TStruct* tstruct = new TStruct();
	Token* token = _getNextToken();
	if (token->type != Type::ID) {
		throw SyntaxError(token, "expected id");
	}

	tstruct->name = token; // TODO add to table
	
	token = _getNextToken();
	if (token->lexem == ";") {
		return tstruct;
	}

	if (token->lexem != "<") {
		throw SyntaxError(token, "expected <");
	}
	token = _getNextToken();
	if (token->lexem != ">") {
		lex->decrementTokenItern();
		_parseParameters(tstruct->parameters, "struct");
		token = _getNextToken();
		if (token->lexem != ">") {
			throw SyntaxError(token, "expected >");
		}
	}

	token = _getNextToken();
	if (token->lexem != ";") {
		throw SyntaxError(token, "expected ;");
	}

	return tstruct;
}

Syntax::TReturn* Syntax::_parseReturn() {
	TReturn* treturn = new TReturn();

	_parseExpression(treturn->exp, "exp");
	return treturn;
}

bool Syntax::_checkTypeStructInit(Token* token) {
	return false;
}

bool Syntax::_checkSecondID(Token* token) {
	if (!_checkTypeStructInit(token)) {
		return false;
	}

	token = _getNextToken();
	if (token->type == Type::ID) {
		lex->decrementTokenItern();
		return true;
	}
	lex->decrementTokenItern();
	return false;
}

