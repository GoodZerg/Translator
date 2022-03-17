#include "Syntax.h"

#define _SemanticError(message) SemanticError((lex->decrementTokenItern(), lex->getNextToken()), message);

Syntax::Syntax(Lex* lex) {
  this->lex = lex;
  _program = new TProgram();
	_sRoot = new SemanticTree();
	_sCurrent = _sRoot;
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
					_program->program.push_back(_parseFunctionWithStruct(token1));
				} else {
					lex->decrementTokenItern(2);
					_program->program.push_back(_parseFunction());
				}
			} else if (token->lexem == "struct") {
				_program->program.push_back(_parseStruct());
			}
		} else if (token->type == Type::TYPE || _checkTypeStructInit(token)) {
			lex->decrementTokenItern();
			_program->program.push_back(_parseInit());
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
	if (_flagOfSemantic) {
		_sCurrent->nodes.push_back(new SemanticTree(_sCurrent));
		_sCurrent = _sCurrent->nodes.back();
	}
	_flagOfSemantic = true;

	Token* token = _getNextToken();
	if(token->lexem != "{") {
		throw SyntaxError(token, "expected {");
	}
	
	while (true) {
		token = _getNextToken();
		if (token->type == Type::KEYWORD) {
			if (token->lexem == "if") {
				block->nodes.push_back(_parseIf());
			} else if (token->lexem == "for") {
				block->nodes.push_back(_parseFor());
			} else if (token->lexem == "while") {
				block->nodes.push_back(_parseWhile());
			} else if (token->lexem == "match") {
				;						// TODO //
			} else if (token->lexem == "foreach") {
				block->nodes.push_back(_parseForEach());
			} else if (token->lexem == "return") {
				block->nodes.push_back(_parseReturn());
			} else if (token->lexem == "read") {
				block->nodes.push_back(_parseRead());
			} else if (token->lexem == "print") {
				block->nodes.push_back(_parsePrint());
			} else if (token->lexem == "continue" || token->lexem == "break") {
				lex->decrementTokenItern();
				block->nodes.push_back(_parseSingleKeyWord());
			} else {
				throw SyntaxError(token, "unexpected keyword");
			}
		} else if (token->type == Type::TYPE || _checkSecondID(token)) {
			lex->decrementTokenItern();
			block->nodes.push_back(_parseInit());
		} else if (token->lexem == "}") {
			_sCurrent = _sCurrent->parent;
			_flagOfSemantic = true;
			return block;
		} else if (token->lexem == "{") {
			lex->decrementTokenItern();
			block->nodes.push_back(_parseBlock());
		} else {
			lex->decrementTokenItern();
			Exp* exp = nullptr;
			_parseExpression(exp, "exp");
			_getNextToken();
			block->nodes.push_back(new TExp{ exp });
		}
	}

	_sCurrent = _sCurrent->parent;
	_flagOfSemantic = true;
  return block;
}

void Syntax::_parseExpression(Exp*& exp, std::string end_symbol) {
	exp = new Exp(lex, end_symbol);
	_validatePolis(exp->polis);
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
		_checkVariableExistance(_sCurrent, name->lexem, init->type->typrStr);

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

	_sCurrent->nodes.push_back(new SemanticTree(_sCurrent));
	_sCurrent = _sCurrent->nodes.back();
	_flagOfSemantic = false;

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

	_sCurrent->nodes.push_back(new SemanticTree(_sCurrent));
	_sCurrent = _sCurrent->nodes.back();
	_flagOfSemantic = false;

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

int64_t Syntax::_parseParameters(std::vector<_parameter*>& parameters, std::string end_symbol) {
	Token* token;
	bool flag = true; // check default params
	int64_t index = 0;
	do {
		/*token = _getNextToken();
		if (token->type != Type::TYPE && !_checkTypeStructInit(token)) {
			throw SyntaxError(token, "expected Type");
		}
		lex->decrementTokenItern();
		*/
		TType* type = _parseType();
		token = _getNextToken();
		if (token->type != Type::ID) {
			throw SyntaxError(token, "expected id");
		}
		Token* name = token;

		_checkVariableExistance(_sCurrent, name->lexem, type->typrStr);

		if (flag) {
			token = _getNextToken();
			if (token->lexem == "=") {
				flag = false;
				parameters.push_back(new _parameter{ type, name, nullptr });
				_parseExpression(parameters[parameters.size() - 1]->exp, end_symbol);
				token = _getNextToken(); 
				continue;
			}
			++index;
			parameters.push_back(new _parameter{ type, name, nullptr });
		} else {
			token = _getNextToken();
			if (token->lexem != "=") {
				throw SyntaxError(token, "expected =");
			}
			parameters.push_back(new _parameter{ type, name, nullptr });
			_parseExpression(parameters[parameters.size() - 1]->exp, end_symbol);
			token = _getNextToken();
		}
	} while (token->lexem == ",");
	lex->decrementTokenItern();
	return index;
}

Syntax::TFunction* Syntax::_parseFunction(TFunction* function) {
	if (function == nullptr) {
		function = new TFunction();
	}

	_sCurrent->nodes.push_back(new SemanticTree(_sCurrent));
	_sCurrent = _sCurrent->nodes.back();
	_flagOfSemantic = false;

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
		function->indexStartDefault = _parseParameters(function->parameters, "fn");
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
		throw SyntaxError(token, "undefined type");
	}
	lex->decrementTokenItern();
	function->type = _parseType();

	token = _getNextToken();
	if (token->lexem == ";") {
		_sCurrent = _sCurrent->parent;
		_flagOfSemantic = true;
		function->body = nullptr;
		_addFunctionToTable(function, function->nameFunction);
		return function;
	}
	lex->decrementTokenItern();
	function->body = _parseBlock();

	_addFunctionToTable(function, function->nameFunction);

	return function;
}

Syntax::TFunction* Syntax::_parseFunctionWithStruct(Token* name) {
	TFunction* function = new TFunction();
	function->nameStruct = name;
	if (_findTypeStruct(name->lexem) == nullptr) {
		throw _SemanticError("undefined struct");
	}
	return _parseFunction(function);
}

Syntax::TStruct* Syntax::_parseStruct() {
	TStruct* tstruct = new TStruct();

	_sCurrent->nodes.push_back(new SemanticTree(_sCurrent));
	_sCurrent = _sCurrent->nodes.back();
	_flagOfSemantic = false;

	Token* token = _getNextToken();
	if (token->type != Type::ID) {
		throw SyntaxError(token, "expected id");
	}

	tstruct->name = token; // TODO add to table
	if (_findTypeStruct(tstruct->name->lexem) != nullptr) {
		throw _SemanticError("double definiton struct");
	}

	token = _getNextToken();
	if (token->lexem == ";") {
		_flagOfSemantic = true;
		_sCurrent = _sCurrent->parent;
		// TODO prototype of struct
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

	_addStructToTable(tstruct);

	token = _getNextToken();
	if (token->lexem != ";") {
		throw SyntaxError(token, "expected ;");
	}

	_flagOfSemantic = true;
	_sCurrent = _sCurrent->parent;

	return tstruct;
}

Syntax::TReturn* Syntax::_parseReturn() {
	TReturn* treturn = new TReturn();

	_parseExpression(treturn->exp, "exp");
	return treturn;
}

bool Syntax::_checkTypeStructInit(Token* token) {
	for (TypeStruct* elem : _structsTable) {
		if (elem->type == token->lexem) {
			return true;
		}
	}
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

Syntax::TypeStruct* Syntax::_findTypeStruct(std::string& type) {
	for (TypeStruct* elem : _structsTable) {
		if (elem->type == type) {
			return elem;
		}
	}
	return nullptr;
}

void Syntax::_addStructToTable(TStruct* tstruct) {
	_structsTable.push_back(new TypeStruct());
	_structsTable.back()->type = tstruct->name->lexem;
	for (_parameter* elem : tstruct->parameters) {
		_structsTable.back()->stVariables.push_back(_castParametrToVariable(elem));
	}
}

Syntax::Variable* Syntax::_castParametrToVariable(_parameter* parametr) {
	return new Variable(parametr->name->lexem, parametr->type->typrStr, _findTypeStruct(parametr->type->typrStr));
}

void Syntax::_findFunctionInTable(Function* function, Token* errorPoint) {
	for (Function* elem : _functionsTable) {
		if (elem->belongToStruct == function->belongToStruct &&
			elem->name == function->name &&
			elem->parameters.size() == function->parameters.size()) {
			for (size_t index = 0; index < elem->parameters.size(); ++index) {
				if (elem->parameters[index]->type != function->parameters[index]->type) {
					return;
				}
			}
			if (elem->isImplemented == false) {
				if (function->isImplemented == true) {
					continue;
				}
				//throw _SemanticError("double prototype");
				throw SemanticError(errorPoint, "double prototype");
			}
			if (function->isImplemented == true) {
				//throw _SemanticError("ambiguous definition");
				throw SemanticError(errorPoint, "ambiguous definition");
			}
			//throw _SemanticError("re-declaration");
			throw SemanticError(errorPoint, "re-declaration");
		}
	}
}

void Syntax::_addVariableToSemanticTree(SemanticTree* tree, std::string& name, std::string& type) {
	tree->localVariables.push_back(new Variable(name, type, _findTypeStruct(type)));
}

void Syntax::_addVariableToTable() {

}

void Syntax::_checkVariableExistance(SemanticTree* tree, std::string& name, std::string& type) {
	for (Variable* elem : tree->localVariables) {
		if (elem->name == name) {
			throw _SemanticError("double variable definition");
		}
	}
	_addVariableToSemanticTree(tree, name, type);
	//_addVariableToTable();
}

void Syntax::_validatePolis(std::vector<Token*>& exp) {
	for (Token* elem : exp) {
		std::cout << elem->lexem << " ";
	}
	std::cout << "\n";

	struct polisType {
		std::string* type;
		bool isType;
		polisType(std::string* type, bool isType){
			this->type = type;
			this->isType = isType;
		}
	};
	std::stack<std::vector<polisType*>> polisStack;

	for (Token* elem : exp) {
		if (elem->type == Type::ID) {
			polisStack.push(std::vector<polisType*>(1, new polisType(&elem->lexem, false)));
		} else if (elem->type == Type::LITERAL) {
			polisStack.push(std::vector<polisType*>(1, new polisType(new std::string("string"), true)));
		} else if (elem->type == Type::NUMBER) {
			polisStack.push(std::vector<polisType*>(1, new polisType(_checkNumberType(elem->lexem), true)));
		} else if (elem->type == Type::OPERATOR) {
			if (elem->lexem == "fn") {

			} else if (elem->lexem[0] == 's') {

			} else if (elem->lexem[0] == 'p') {

			} else if (elem->lexem[0] == 'b') {

			}
		}
	}
	
}

std::string* Syntax::_checkNumberType(std::string& type) {
	for (char& elem : type) {
		if (elem == '.') {
			return new std::string("f128");
		}
	}
	return new std::string("ui128");
}

void Syntax::_addFunctionToTable(TFunction* tFunction, Token* errorPoint) {
	Function* sFunction = new Function(tFunction->nameFunction->lexem, tFunction->type->typrStr, tFunction->indexStartDefault);
	for (_parameter* elem : tFunction->parameters) {
		sFunction->parameters.push_back(_castParametrToVariable(elem));
	}
	if (tFunction->nameStruct != nullptr) {
		sFunction->belongToStruct = _findTypeStruct(tFunction->nameStruct->lexem);
		if (sFunction->belongToStruct == nullptr) {
			//throw _SemanticError("impossible struct");
			throw SemanticError(errorPoint, "undefined struct");
		}
		sFunction->belongToStruct->stFunctions.push_back(sFunction);
	}
	if (tFunction->body != nullptr) {
		sFunction->isImplemented = true;
	}
	_findFunctionInTable(sFunction, errorPoint);
	_functionsTable.push_back(sFunction);
}

