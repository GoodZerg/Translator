#include "Syntax.h"

#define _SemanticError(message) SemanticError((lex->decrementTokenItern(), lex->getNextToken()), message);

#define __polisStackTopWPop() [&]() -> std::vector<polisType*> {\
auto ahah = polisStack.top();\
polisStack.pop();\
return ahah;\
}

#define _polisStackTopWPop() __polisStackTopWPop()()

#define firstOp  firstOperand.back()
#define secondOp secondOperand.back()
#define thirdOp  thirdOperand.back()

Syntax::SemanticTree *Syntax:: _sRoot = nullptr, *Syntax::_sCurrent = nullptr;
std::map<std::string, std::vector<Syntax::Function*>> Syntax::_functionsTable = std::map< std::string, std::vector<Syntax::Function*>>();

std::map<std::string, int64_t> Syntax::typesCastPriop = {
	{"float",    0},
	{"signed",   1},
	{"unsigned", 2},
	{"char",     3},
	{"bool",     4}
};

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
		} else {
			throw SyntaxError(token, "unexpected expression");
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
	for (Token* elem : exp->polis) {
		std::cout << elem->lexem << " ";
	}
	std::cout << "\n";

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
	_returnValueType = function->type->typrStr;

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
	_isReturn = true;
	_parseExpression(treturn->exp, "exp");
	_isReturn = false;
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

bool Syntax::_checkFunctionInTable(Function* function, Token* errorPoint, 
	std::map<std::string, std::vector<Function*>>& functionsTable) {
	std::vector<Function*> functionsArray = functionsTable[function->name];
	bool flagIsNotSameFunction = false;
	for (Function* elem : functionsArray) {
		if (elem->parameters.size() == function->parameters.size()) {
			for (size_t index = 0; index < elem->parameters.size(); ++index) {
				if (elem->parameters[index]->type != function->parameters[index]->type) {
					flagIsNotSameFunction = true;
					break;
				}
			}
			if (flagIsNotSameFunction) {
				flagIsNotSameFunction = false;
				continue;
			}
			if (elem->isImplemented == false) {
				if (function->isImplemented == true) {
					if (elem->indexStartDefault != elem->parameters.size() && 
						function->indexStartDefault != function->parameters.size()) {
						throw SemanticError(errorPoint, "double default value");
					}
					elem->isImplemented = true;
					if (elem->indexStartDefault > function->indexStartDefault) {
						elem->indexStartDefault = function->indexStartDefault;
					}
					return true;
				}
				//throw _SemanticError("double prototype");
				throw SemanticError(errorPoint, "double prototype");
			}
			if (elem->retType == function->retType) {
				throw SemanticError(errorPoint, "wrong overload");
			}
			if (function->isImplemented == true) {
				//throw _SemanticError("ambiguous definition");
				throw SemanticError(errorPoint, "ambiguous definition");
			}
			//throw _SemanticError("re-declaration");
			throw SemanticError(errorPoint, "re-declaration");
		}
	}
	return false;
}

void Syntax::_addVariableToSemanticTree(SemanticTree* tree, std::string& name, std::string& type) {
	tree->localVariables.push_back(new Variable(name, type, _findTypeStruct(type)));
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
	std::stack<std::vector<polisType*>> polisStack;

	for (size_t i = 0; i < exp.size(); ++i) {
		Token* elem = exp[i];
		if (elem->type == Type::ID) {
			polisStack.push(std::vector<polisType*>(1, new polisType(elem->lexem)));
		} else if (elem->type == Type::LITERAL) {
			polisStack.push(std::vector<polisType*>(1, new polisType("string", true, false)));
		} else if (elem->type == Type::CHAR) {
			polisStack.push(std::vector<polisType*>(1, new polisType("char", true, false)));
		} else if (elem->type == Type::NUMBER) {
			polisStack.push(std::vector<polisType*>(1, new polisType(_checkNumberType(elem->lexem), true, false)));
		} else if (elem->type == Type::OPERATOR) {
			if (elem->lexem[0] == 's' || elem->lexem[0] == 'p' || elem->lexem[0] == 'u') {
				std::vector<polisType*> firstOperand = _polisStackTopWPop();
				firstOp->transformVariableToType(elem);
				if(*firstOp->type == "void") {
					throw SemanticError(elem, "u operation with void-type");
				}
				if (elem->lexem == "u&") {
					if (!firstOp->isReference) {
						throw SemanticError(elem, "try to get r-value address");
					}
					firstOp->points++;
					firstOp->isReference = false;
				} else if (elem->lexem == "u*") {
					if(firstOp->points == false) {
						throw SemanticError(elem, "try to dereference a non-pointer");
					}
					firstOp->points--;
					firstOp->isReference = true;
				} else if (elem->lexem == "p++" || elem->lexem == "p--" || elem->lexem == "s++" || elem->lexem == "s--") {
					if(!firstOp->isReference) {
						throw SemanticError(elem, "try to use inc\dec with r-value");
					}
					if (!firstOp->points) {
						if (*firstOp->type == "string") {
							throw SemanticError(elem, "u operation with string");
						}
						if(firstOp->isStruct) {
							throw SemanticError(elem, "u operation with struct");
						}
					}

					if(elem->lexem == "s++" || elem->lexem == "s--") {
						firstOp->isReference = false;
					} else {
						firstOp->isReference = true;
					}
				} else if (elem->lexem == "u-" || elem->lexem == "u+") {
					if(firstOp->points) {
						throw SemanticError(elem, "u+/u- operation with pointer");
					}
					if(*firstOp->type == "string") {
						throw SemanticError(elem, "u+/u- with string");
					}
					if(firstOp->isStruct) {
						throw SemanticError(elem, "u+/u- operation with struct");
					}
					firstOp->isReference = false;
				}
				polisStack.push(std::vector<polisType*>(1, firstOp));
			} else if(elem->lexem == "!") {
				std::vector<polisType*> firstOperand = _polisStackTopWPop();
				_castSpecialType(*firstOp, "bool", elem);
				firstOp->isReference = false;
				polisStack.push(std::vector<polisType*>(1, firstOp));
			} else if(elem->lexem == "~") {
				std::vector<polisType*> firstOperand = _polisStackTopWPop();
				if(firstOp->points) {
					throw SemanticError(elem, "can't cast pointer in bits operations");
				} else if(*firstOp->type == "float") {
					throw SemanticError(elem, "can't cast float in bits operations");
				} else if(*firstOp->type != "unsigned") {
					_castSpecialType(*firstOp, "signed", elem);
				}
				firstOp->isReference = false;
				polisStack.push(std::vector<polisType*>(1, firstOp));
			} else {
				std::vector<polisType*> firstOperand = _polisStackTopWPop();
				std::vector<polisType*> secondOperand = _polisStackTopWPop();
				if (elem->lexem == "fn") {
					std::vector<Function*> functions;
					if (i + 1 < exp.size() && exp[i + 1]->lexem[0] == '.') {
						std::vector<polisType*> thirdOperand = _polisStackTopWPop();
						thirdOp->transformVariableToType(elem);
						if (thirdOp->isStruct == false) {
							throw SemanticError(elem, "not a struct"); // TODO rename error
						}
						if (thirdOp->points == true) {
							throw SemanticError(elem, "wrong struct pointer"); // TODO rename error
						}
						if (secondOp->isType == true) {
							throw SemanticError(elem, "wrong type"); // TODO rename error
						}
						functions = _findFunctionInStruct(*thirdOp->type, *secondOp->type);
						++i;
					} else {
						functions = _findFunctionInTable(*secondOp->type);
					}
					if (functions.empty()) {
						throw SemanticError(elem, "undefined function"); // TODO rename error
					}
					Function* calledFunction = nullptr;

					// TODO hurt in ass(подстановка аргументов)
					//polisType* retType = new polisType(function->retType, true);
					//polisStack.push(std::vector<polisType*>(1, retType));
					continue;

				} else {
					secondOp->transformVariableToType(elem);
					if (elem->lexem == "[]") {
						firstOp->transformVariableToType(elem);
						if (!secondOp->points) {
							throw SemanticError(elem, "try to index not-pointer type");
						}
						if(firstOp->points) {
							throw SemanticError(elem, "pointer connot be an index");
						}
						if(*firstOp->type == "float") {
							throw SemanticError(elem, "float connot be an index");
						}
						_castSpecialType(*firstOp, "signed", elem);
						*firstOp = *secondOp;
						firstOp->points--;
					} else if (elem->lexem == ".") {
						if (secondOp->isStruct == false) {
							throw SemanticError(elem, "not a struct"); // TODO rename error
						}
						if (secondOp->points == true) {
							throw SemanticError(elem, "wrong struct pointer"); // TODO rename error
						}
						if (firstOp->isType == true) {
							throw SemanticError(elem, "wrong type"); // TODO rename error
						}
						firstOp->type = _findVariableInStruct(*secondOp->type, *firstOp->type);
						if (firstOp->type == nullptr) {
							throw SemanticError(elem, "undefined variable in struct");
						}

						firstOp->isType = true;
						firstOp->validateType();

					} else if (elem->lexem == ",") {
						firstOp->transformVariableToType(elem);
						secondOperand.push_back(firstOp);
						polisStack.push(secondOperand);
						continue;
					} else {
						firstOp->transformVariableToType(elem);
						if (elem->lexem == "==" || elem->lexem == "!=") {
							if (firstOp->points && secondOp->points) {
								_castPointersType(*firstOp, *secondOp, elem);
							} else if (firstOp->points || secondOp->points) {
								if (secondOp->points) {
									swap(*firstOp, *secondOp);
								}
								if (*secondOp->type != "signed" && *secondOp->type != "unsigned") {
									throw SemanticError(elem, "can't cast pointer");
								}
							} else if (!(*firstOp->type == "string" && *secondOp->type == "string")) {
								_castTypesBinaryOperation(*firstOp, *secondOp, elem);
							}
							firstOp->clear("bool");
						} else if (elem->lexem == ">" || elem->lexem == "<" ||
							elem->lexem == ">=" || elem->lexem == "<=") {
							if (firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer in comparisons");
							} else {
								_castTypesBinaryOperation(*firstOp, *secondOp, elem);
							}
							firstOp->clear("bool");
						} else if (elem->lexem == "b&" || elem->lexem == "|") {
							if (firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer in bits operations");
							} else if (*firstOp->type == "float" || *secondOp->type == "float") {
								throw SemanticError(elem, "can't cast float in bits operations");
							} else if (*firstOp->type == "unsigned" || *secondOp->type == "unsigned") {
								_castSpecialType(*firstOp, "unsigned", elem);
								_castSpecialType(*secondOp, "unsigned", elem);
							} else {
								_castSpecialType(*firstOp, "signed", elem);
								_castSpecialType(*secondOp, "signed", elem);
							}
						} else if (elem->lexem == "&=" || elem->lexem == "|=") {

							if(!secondOp->isReference) {
								throw SemanticError(elem, "try assign to r-value ");
							}

							if (firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer in bits operations");
							} else if (*firstOp->type == "float" || *secondOp->type == "float") {
								throw SemanticError(elem, "can't cast float in bits operations");
							} else {
								_castSpecialType(*firstOp, *secondOp->type, elem);
								*firstOp = *secondOp;
							}
							firstOp->isReference = true;
							polisStack.push(std::vector<polisType*>(1, firstOp));
							continue;
						} else if (elem->lexem == "b+") {
							if (firstOp->points && secondOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							}
							if (firstOp->points || secondOp->points) {
								if (secondOp->points) {
									swap(*firstOp, *secondOp);
								}
								std::cout << *secondOp->type << "\n";
								if (*secondOp->type != "signed" && *secondOp->type != "unsigned") {
									throw SemanticError(elem, "can't cast pointer");
								}
							} else {
								if (!(*firstOp->type == "string" && *secondOp->type == "string")) {
									_castTypesBinaryOperation(*firstOp, *secondOp, elem);
								}
							}
						} else if (elem->lexem == "b-") {
							if (firstOp->points && secondOp->points) {
								_castPointersType(*firstOp, *secondOp, elem);
							} else if (firstOp->points || secondOp->points) {
								if (firstOp->points) {
									throw SemanticError(elem, "can't subtract pointer");
								}
								if (*firstOp->type != "signed" && *firstOp->type != "unsigned") {
									throw SemanticError(elem, "can't cast pointer");
								}
								*firstOp = *secondOp;
							} else {
								_castTypesBinaryOperation(*firstOp, *secondOp, elem);
							}
						} else if (elem->lexem == "%") {
							if (firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							}

							if (*firstOp->type == "float" || *secondOp->type == "float") {
								throw SemanticError(elem, "can't % float");
							}
							_castTypesBinaryOperation(*firstOp, *secondOp, elem);
						} else if (elem->lexem == "b*" || elem->lexem == "/" || elem->lexem == "^") {
							if (firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							}
							_castTypesBinaryOperation(*firstOp, *secondOp, elem);
						} else if (elem->lexem == "+=") {
							if(!secondOp->isReference) {
								throw SemanticError(elem, "try assign to r-value ");
							}
							if(firstOp->points && secondOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							}
							if(firstOp->points || secondOp->points) {
								if(firstOp->points) {
									throw SemanticError(elem, "can't subtract pointer");
								}
								if(*firstOp->type != "signed" && *firstOp->type != "unsigned") {
									throw SemanticError(elem, "can't cast pointer");
								}
								*firstOp = *secondOp;
							} else {
								if(!(*firstOp->type == "string" && *secondOp->type == "string")) {
									_castSpecialType(*firstOp, *secondOp->type, elem);
									*firstOp = *secondOp;
								}
							}
							firstOp->isReference = true;
							polisStack.push(std::vector<polisType*>(1, firstOp));
							continue;
						} else if (elem->lexem == "-=") {
							if(!secondOp->isReference) {
								throw SemanticError(elem, "try assign to r-value ");
							}
							if(firstOp->points && secondOp->points) {
								_castPointersType(*firstOp, *secondOp, elem);
							}
							if(firstOp->points || secondOp->points) {
								if(firstOp->points) {
									throw SemanticError(elem, "can't subtract pointer");
								}
								if(*firstOp->type != "signed" && *firstOp->type != "unsigned") {
									throw SemanticError(elem, "can't cast pointer");
								}
								*firstOp = *secondOp;
							} else {
								if(!(*firstOp->type == "string" && *secondOp->type == "string")) {
									_castSpecialType(*firstOp, *secondOp->type, elem);
									*firstOp = *secondOp;
								}
							}
							firstOp->isReference = true;
							polisStack.push(std::vector<polisType*>(1, firstOp));
							continue;
						} else if (elem->lexem == "%=") {
							if(!secondOp->isReference) {
								throw SemanticError(elem, "try assign to r-value ");
							}
							if(firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							}
							if(*firstOp->type == "float" || *secondOp->type == "float") {
								throw SemanticError(elem, "can't % float");
							}
							_castSpecialType(*firstOp, *secondOp->type, elem);
							*firstOp = *secondOp;
							firstOp->isReference = true;
							polisStack.push(std::vector<polisType*>(1, firstOp));
							continue;
						} else if( elem->lexem == "*=" || elem->lexem == "/=" ||  elem->lexem == "^=") {
							if(!secondOp->isReference) {
								throw SemanticError(elem, "try assign to r-value ");
							}
							if(firstOp->points || secondOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							}
							_castSpecialType(*firstOp, *secondOp->type, elem);
							*firstOp = *secondOp;
							firstOp->isReference = true;
							polisStack.push(std::vector<polisType*>(1, firstOp));
							continue;
						} else if (elem->lexem == "or" || elem->lexem == "and") {
							_castSpecialType(*firstOp, "bool", elem);
							_castSpecialType(*secondOp, "bool", elem);
						} else if (elem->lexem == "=") {
							if(!secondOp->isReference) {
								throw SemanticError(elem, "try assign to r-value ");
							}
							if (*firstOp == *secondOp) {
								;
							} else if (firstOp->points && secondOp->points) {
								_castPointersType(*firstOp, *secondOp, elem); // only for correct errors
							} else if (firstOp->points) {
								throw SemanticError(elem, "can't cast pointer");
							} else if (secondOp->points && (*firstOp->type == "signed" || *firstOp->type == "unsigned")) {
								;
							} else if(firstOp->isStruct || secondOp->isStruct) {
								throw SemanticError(elem, "can't cast struct");
							} else {
								_castSpecialType(*firstOp, *secondOp->type, elem);
								*firstOp = *secondOp;
							}
							firstOp->isReference = true;
							polisStack.push(std::vector<polisType*>(1, firstOp));
							continue;
						}
					}
				}
				firstOp->isReference = false;
				polisStack.push(std::vector<polisType*>(1, firstOp));
			}
		}
	}
	if (!polisStack.empty()) {
		std::vector<polisType*> lastOperand = _polisStackTopWPop();
		lastOperand.back()->transformVariableToType(exp.back());
		if (_isReturn) {
			polisType* returnType = new polisType(_returnValueType, true);
			if (*lastOperand.back() == *returnType) {
				;
			} else if (lastOperand.back()->points && returnType->points) {
				_castPointersType(*lastOperand.back(), *returnType, exp.back()); // only for correct errors
			} else if (lastOperand.back()->points) {
				throw SemanticError(exp.back(), "can't cast pointer");
			} else if (returnType->points && (*lastOperand.back()->type == "signed" || 
				*lastOperand.back()->type == "unsigned")) {
				;
			} else if (lastOperand.back()->isStruct || returnType->isStruct) {
				throw SemanticError(exp.back(), "can't cast struct");
			} else {
				_castSpecialType(*lastOperand.back(), *returnType->type, exp.back());
			}
		}
	} else if (_isReturn && _returnValueType != "void") {
		throw SemanticError(exp.back(), "function return wrong type");
	}
}

std::string Syntax::_checkNumberType(std::string& type) {
	for (char& elem : type) {
		if (elem == '.') {
			return std::string("float");
		}
	}
	return std::string("unsigned");
}

std::string* Syntax::_findVariableInTree(std::string* name, bool& isStruct) {
	return __findVariableInTree(name, Syntax::_sCurrent, isStruct);
}

Syntax::polisType::polisType(std::string type, bool isType, bool isReference) {
	this->type = new std::string(type);
	this->isType = isType;
	this->bitSize = 0;
	this->points = 0;
	this->isReference = isReference;
	this->isStruct = false;
	if(isType) {
		this->validateType();
	}
}

Syntax::polisType::~polisType() {
	delete type;
}

void Syntax::polisType::countAndRemovePoints() {
	for(char elem : *type) {
		if (elem == '*') {
			++points;
		} else if(elem == '&') {
			isReference = true;
		}
	}
	type->erase(type->size() - points - ((*type)[type->size() - 1] == '&' ? 1 : 0), 
		points + ((*type)[type->size() - 1] == '&' ? 1 : 0));
}

void Syntax::polisType::countBitSize() {
	if(*type == "char" || *type == "bool") {
		bitSize = 8;
	} else if(*type == "si16" || *type == "ui16") {
		bitSize = 16;
	} else if(*type == "si32" || *type == "ui32" || *type == "f32") {
		bitSize = 32;
	} else if(*type == "si64" || *type == "ui64" || *type == "f64") {
		bitSize = 64;
	} else if(*type == "si128" || *type == "ui128" || *type == "f128") {
		bitSize = 128;
	} else {
		bitSize = 0;
	}
}

void Syntax::polisType::transformToBaseType() {
	if(*type == "si8" || *type == "si16" || *type == "si32" || *type == "si64" || *type == "si128") {
		*type = "signed";
	} else if(*type == "ui8" || *type == "ui16" || *type == "ui32" || *type == "ui64" || *type == "ui128") {
		*type = "unsigned";
	} else if(*type == "f32" || *type == "f64" || *type == "f128") {
		*type = "float";
	}
}

void Syntax::polisType::transformVariableToType(Token* error) {
	if(this->isType == false) {
		this->isType = true;

		this->type = _findVariableInTree(this->type, this->isStruct);
		if(this->type == nullptr) {
			throw SemanticError(error, "unknown variable");
		}

		countAndRemovePoints();
		countBitSize();
		transformToBaseType();
	}
}

void Syntax::polisType::validateType() {
	this->countAndRemovePoints();
	this->countBitSize();
	this->transformToBaseType();
	if(*this->type != "float" && *this->type != "signed" && *this->type != "unsigned" &&
		*this->type != "char" && *this->type != "string" && *this->type != "void" &&
		*this->type != "bool") {
		this->isStruct = true;
	}
}

void Syntax::polisType::clear() {
	delete this->type;
	this->type = nullptr;
	this->bitSize = 0;
	this->points = 0;
	this->isType = false;
	this->isReference = false;
	this->isStruct = false;
}

void Syntax::polisType::clear(std::string type) {
	delete this->type;
	this->type = new std::string(type);
	this->bitSize = 0;
	this->points = 0;
	this->isType = true;
	this->isReference = false;
	this->isStruct = false;
	this->validateType();
}

bool Syntax::polisType::operator==(polisType& second) {
	return this->points == second.points && *this->type == *second.type &&
		this->bitSize == second.bitSize;
}

Syntax::polisType& Syntax::polisType::operator=(polisType& second) {
	delete this->type;
	this->type = new std::string(*second.type);
	this->bitSize = second.bitSize;
	this->points = second.points;
	this->isReference = second.isReference;
	this->isStruct = second.isStruct;
	this->isType = second.isType;
	return *this;
}

std::string* Syntax::__findVariableInTree(std::string* name, SemanticTree* node, bool& isStruct) {
	for(Variable* elem : node->localVariables) {
		if(elem->name == *name) {
			return elem->typest != nullptr ? (isStruct = true, new std::string(elem->type)) : new std::string(elem->type);
		}
	}
	if(node->parent == nullptr) {
		return nullptr;
	}
	return __findVariableInTree(name, node->parent, isStruct);
}

std::string* Syntax::_findVariableInStruct(std::string& type, std::string& variable) {
	TypeStruct* typest = _findTypeStruct(type);
	for (Variable* elem : typest->stVariables) {
		if (elem->name == variable) {
			return &elem->type;
		}
	}
	
	return nullptr;
}

std::vector<Syntax::Function*>& Syntax::_findFunctionInTable(std::string& function) {
	return _functionsTable[function];
}

void Syntax::_castSpecialType(polisType& first, std::string second, Token* error) {
	if (first.isStruct && !first.points) {
		throw SemanticError(error, "can't cast struct type");
	}
	if (*first.type == "string" && second == "string") {
		first.clear(std::string("string"));
	} else if (*first.type == "string") {
		throw SemanticError(error, "can't cast string");
	} else if (*first.type == "void") {
		throw SemanticError(error, "can't cast void");
	} else if (second == "bool") {
		first.clear(std::string("bool"));
	} else if (second == "signed") {
		first.clear(std::string("signed"));
	} else if (second == "unsigned") {
		first.clear(std::string("unsigned"));
	} else if (second == "char") {
		first.clear(std::string("char"));
	} else if (second == "float") {
		first.clear(std::string("float"));
	} else {
		throw SemanticError(error, "can't cast");
	}
}

void Syntax::_castTypesBinaryOperation(polisType& first, polisType& second, Token* error) {
	if (first.isStruct || second.isStruct) {
		throw SemanticError(error, "can't cast type struct");
	}
	if (*first.type == "void" || *second.type == "void") {
		throw SemanticError(error, "can't cast type void");
	}
	if (*first.type == "string" || *second.type == "string") {
		throw SemanticError(error, "can't cast type string");
	}

	if (typesCastPriop[*first.type] > typesCastPriop[*second.type]) {
		first = second;
	} else if (typesCastPriop[*first.type] == typesCastPriop[*second.type] && first.bitSize < second.bitSize) {
		first.bitSize = second.bitSize;
	}
}

Syntax::Function* Syntax::_firstSubstitution(std::vector<Syntax::Function*> functions) {
	return nullptr;
}

Syntax::Function* Syntax::_secondSubstitution(std::vector<Syntax::Function*> functions) {
	return nullptr;
}

std::vector<Syntax::Function*>& Syntax::_findFunctionInStruct(std::string& type, std::string& function) {
	return _findTypeStruct(type)->stFunctions[function];
}

void Syntax::_addFunctionToTable(TFunction* tFunction, Token* errorPoint) {
	Function* sFunction = new Function(tFunction->nameFunction->lexem, tFunction->type->typrStr, tFunction->indexStartDefault);
	for (_parameter* elem : tFunction->parameters) {
		sFunction->parameters.push_back(_castParametrToVariable(elem));
	}
	if (tFunction->body != nullptr) {
		sFunction->isImplemented = true;
	}
	if (tFunction->nameStruct != nullptr) {
		sFunction->belongToStruct = _findTypeStruct(tFunction->nameStruct->lexem);
		if (sFunction->belongToStruct == nullptr) {
			throw SemanticError(errorPoint, "undefined struct");
		}
		if (!_checkFunctionInTable(sFunction, errorPoint, sFunction->belongToStruct->stFunctions)) {
			sFunction->belongToStruct->stFunctions[sFunction->name].push_back(sFunction);
		}
	} else {
		if (!_checkFunctionInTable(sFunction, errorPoint)) {
			_functionsTable[sFunction->name].push_back(sFunction);
		}
	}
}

void Syntax::_castPointersType(polisType& first, polisType& second, Token* error) {
	if (first.points != second.points) {
		throw SemanticError(error, "can't cast pointers with differnce deepth");
	}

	if (first.type != second.type) {
		throw SemanticError(error, "can't cast pointers on different types");
	}

	if(first.bitSize != second.bitSize) {
		throw SemanticError(error, "can't cast pointers on different bits size types");
	}
}
/*ui32* a = 0;
ui32 b = 10, c, d = 100 + 100;
struct j <char b = 10 * 10, string f = "123" >;

fn bb(si32 a = 10 * 10 - 5, ui128 _hh = 20, ui128 k = 14)->void{
    si32 b;
    if (a > 10 * 10) {
    while (d + 5 == 123 & d) {
        for(f32 a = 10; b < 10; b++){
            foreach(j h -> b) {
				ui32 a, b, c;
				a = b = c;
			}
        }
    }
    } else {
    f32 b;
    }
    j g;
}
fn j::b() -> void;
fn j::b() -> void {}
fn ll(ui32 g) -> void;
fn ll(ui32 g) -> void {}
struct b<>;
fn j::g() -> j;
fn j::g() -> j {}
struct ha<f32 g = 5>;
struct nice<ha h>;
ui32* g = &c - (c * d) + a[10 ^ 10 / 20];

f128 k = a[100 + 50];

string f = "helloworld";
si32 hhhh;
fn main() -> void {
si32 mm = 10;
}
j* lll = 5;
si32****** kk = 5;
*/
