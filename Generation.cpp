#include "Generation.h"

#define TRY_CAST_AND_CALL_FUNCTION(cast_type, var) \
  if(dynamic_cast<##cast_type>(##var)){\
    _convertSyntaxNode(dynamic_cast<##cast_type>(##var));\
  }

#define PUSH_UPCODE(upcode)\
  _genResult.emplace_back(upcode, new Generation::_param{ })

#define PUSH_UPCODE_INT_PARAM(upcode, param)\
  _genResult.emplace_back(upcode, new Generation::_paramInt{ param })

#define PUSH_UPCODE_CHAR_PARAM(upcode, param)\
  _genResult.emplace_back(upcode, new Generation::_paramChar{ param })

#define PUSH_UPCODE_FLOAT_PARAM(upcode, param)\
  _genResult.emplace_back(upcode, new Generation::_paramFloat{ param })

#define PUSH_UPCODE_STRING_PARAM(upcode, param)\
  _genResult.emplace_back(upcode, new Generation::_paramStr{ param })


#define PRINT_PARAM(param)\
if(dynamic_cast<_paramFloat*>(param)){\
    std::cout << dynamic_cast<_paramFloat*>(param)->fl << "\n";\
} else if(dynamic_cast<_paramInt*>(param)){ \
    std::cout << dynamic_cast<_paramInt*>(param)->in << "\n";\
} else if(dynamic_cast<_paramChar*>(param)){ \
    std::cout << dynamic_cast<_paramChar*>(param)->ch << "\n";\
} else if(dynamic_cast<_paramStr*>(param)){ \
    std::cout << dynamic_cast<_paramStr*>(param)->str << "\n";\
} else {\
    std::cout << "\n";\
}

std::map<std::string, UPCODES> Generation::_operations = {
  {"fn",  UPCODES::CALL_FUNCTION},
  {"s++", UPCODES::SUFFIX_INCREMENT},
  {"cast", UPCODES::CAST},
  {"s--", UPCODES::SUFFIX_DECREMENT},
  {".",   UPCODES::POINT},
  {"[]",  UPCODES::INDEX_OPERATOR},
  {"p++", UPCODES::PREFFIX_INCREMENT},
  {"p--", UPCODES::PREFFIX_DECREMENT},
  {"u+",  UPCODES::UNARY_ADD},
  {"u-",  UPCODES::UNARY_SUBSTRACTION},
  {"!",   UPCODES::LOGIC_NOT},
  {"~",   UPCODES::BIT_NOT},
  {"u*",  UPCODES::UNARY_DEREFERENCING},
  {"u&",  UPCODES::UNARY_GET_ADDRESS},
  {"^",   UPCODES::POWER},
  {"b*",  UPCODES::MILTIPLY},
  {"/",   UPCODES::DIVISION},
  {"%",   UPCODES::MOD},
  {"b+",  UPCODES::BINARY_ADD},
  {"b-",  UPCODES::BINARY_SUBSTRACTION},
  {">",   UPCODES::COMPARISION_GREATER},
  {"<",   UPCODES::COMPARISION_LESS},
  {">=",  UPCODES::COMPARISION_GREATER_EQUAL},
  {"<=",  UPCODES::COMPARISION_LESS_EQUAL},
  {"==",  UPCODES::COMPARISION_EQUAL},
  {"!=",  UPCODES::COMPARISION_NOT_EQUAL},
  {"b&",  UPCODES::BIT_AND},
  {"|",   UPCODES::BIT_OR},
  {"and", UPCODES::LOGIC_AND},
  {"or",  UPCODES::LOGIC_OR},
  {"+=",  UPCODES::ADD_ASSIGN},
  {"-=",  UPCODES::SUBSTRACTION_ASSIGN},
  {"*=",  UPCODES::MILTIPLY_ASSIGN},
  {"/=",  UPCODES::DIVISION_ASSIGN},
  {"%=",  UPCODES::MOD_ASSIGN},
  {"&=",  UPCODES::BIT_AND_ASSIGN},
  {"|=",  UPCODES::BIT_OR_ASSIGN},
  {"^=",  UPCODES::POWER_ASSIGN},
  {"=",   UPCODES::ASSIGN},
  {",",   UPCODES::COMMA}
};

std::map<std::string, Generation::StructInfo*> Generation::_structs = { };
std::map<std::string, int64_t> Generation::_functions = { };
std::map<std::string, std::vector<Syntax::_parameter*>*> Generation::_functionsDefaultsValue = { };
std::map<std::string, std::vector<int64_t>> Generation::_functionsDefaultsValueJumpTable = { };
std::vector<Generation::_upCode> Generation::_genResult = { };

int64_t Generation::_StartProgram = -1;

Generation::Generation(Syntax* syntax) {
  _program = syntax->getProgram();
  _convertSyntaxNode(_program);
#if PRINT_GEN == 1
  for (auto& elem : _genResult) { 
    std::cout << GetStringUPCODES(elem.code) <<" ";
    PRINT_PARAM(elem.param);
  }

  for (auto& elem : _functionsDefaultsValueJumpTable) {
    for (auto& it : elem.second) {
      std::cout << it << " ";
    }
    std::cout << "\n";
  }
#endif // PRINT_GEN == 1

}

void Generation::_convertSyntaxNode(Syntax::TProgram* elem) { 
  // what do we do with structures ??
  for (auto& it : elem->program) {
    _convertSyntaxNode(it);
  }
}

void Generation::_convertSyntaxNode(Syntax::TNode* elem) {
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TSingleKeyWord*, elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TType*,          elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TBlock*,         elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TExp*,           elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TInit*,          elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TIf*,            elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TWhile*,         elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TFor*,           elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TForEach*,       elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TPrint*,         elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TRead*,          elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TFunction*,      elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TStruct*,        elem);
  TRY_CAST_AND_CALL_FUNCTION(Syntax::TReturn*,        elem);
}

void Generation::_convertSyntaxNode(Syntax::TSingleKeyWord* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TType* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TBlock* elem) { 
  for (auto& it : elem->nodes) {
    _convertSyntaxNode(it);
  }
}

void Generation::_convertSyntaxNode(Syntax::TExp* elem) { 
  _convertSyntaxNode(elem->exp);
  PUSH_UPCODE(UPCODES::POP);
  return;
}

void Generation::_convertSyntaxNode_WithSeparate(Syntax::Exp* elem) {
  elem->polis.erase(elem->polis.begin());
  elem->polis.erase(elem->polis.end() - 1);
  _convertSyntaxNode(elem);
  PUSH_UPCODE(UPCODES::ASSIGN_BY_ADDRESS);
}

void Generation::_convertSyntaxNode(Syntax::Exp* elem) {
  for (auto& it : elem->polis) {
    std::string tmp = it->lexem;
    switch (it->type) {
      case Type::ID:
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_NAME, it->lexem);
        break;
      case Type::TYPE:
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_STRING, it->lexem);
        break;
      case Type::NUMBER:
        if (Syntax::_checkNumberType(tmp) == "f128") {
          PUSH_UPCODE_FLOAT_PARAM(UPCODES::LOAD_CONST_FLOAT, std::stold(it->lexem));
        } else {
          PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, std::stoll(it->lexem));
        }
        break;
      case Type::CHAR:
        PUSH_UPCODE_CHAR_PARAM(UPCODES::LOAD_CONST_CHAR, it->lexem[1]);
        break;
      case Type::LITERAL:
        tmp.erase(tmp.begin());
        tmp.erase(tmp.end() - 1);
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, tmp);
        break;
      case Type::OPERATOR:
        if (_operations[tmp] == UPCODES::CALL_FUNCTION) {
          PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)_genResult.size() + 2);
        }
        PUSH_UPCODE(_operations[tmp]);
        break;
      default:
        break;
    }
  }
}


void Generation::_convertSyntaxNode(Syntax::TInit* elem) {
  auto typeInfo = new TypeInfo(0, elem->type->typrStr);
  Syntax::_countAndRemovePoints(&typeInfo->type, typeInfo->points, typeInfo->isReference);
  if (_structs.contains(typeInfo->type)) {
    typeInfo->isStruct = true;
    typeInfo->size = _structs[typeInfo->type]->size;
    if (typeInfo->points == 0) {
      for (auto& it : elem->variables) {
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)_genResult.size() + 3);
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, typeInfo->type);
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, *it->preffix);
        PUSH_UPCODE_INT_PARAM(UPCODES::CREATE_STRUCT, _structs[typeInfo->type]->constrAddr);

        if (it->exp) {
          _convertSyntaxNode(it->exp);
          PUSH_UPCODE(UPCODES::POP);
        }
      }
      return;
    }
  }

  Syntax::_countBitSize(&typeInfo->type, typeInfo->size);
  typeInfo->size >>= 3;
  if (typeInfo->points) {
    typeInfo->baseStep = typeInfo->size;
    typeInfo->size = 8;
  }
  Syntax::_transformToBaseType(&typeInfo->type);

  for (auto& it : elem->variables) {
    PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
    PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->points);
    PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, typeInfo->type);
    PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->isReference);
    PUSH_UPCODE_STRING_PARAM(UPCODES::INIT_VARIABLE, *it->preffix);

    if (it->exp) {
      _convertSyntaxNode(it->exp);
      PUSH_UPCODE(UPCODES::POP);
    }
  }
}

void Generation::_convertSyntaxNode(Syntax::TIf* elem) { 
  _convertSyntaxNode(elem->condition);
  PUSH_UPCODE_INT_PARAM(UPCODES::JUMP_IF_FALSE, 0);
  int64_t* addressToJumpIfFalse = &dynamic_cast<_paramInt*>(_genResult.back().param)->in;
  _convertSyntaxNode(elem->body);

  if (elem->elseBody) {
    PUSH_UPCODE_INT_PARAM(UPCODES::JUMP, 0);
    int64_t* addressToJump = &dynamic_cast<_paramInt*>(_genResult.back().param)->in;

    *addressToJumpIfFalse = _genResult.size();
    _convertSyntaxNode(elem->elseBody);
    *addressToJump = _genResult.size();
  } else {
    *addressToJumpIfFalse = _genResult.size();
  }
}

void Generation::_convertSyntaxNode(Syntax::TWhile* elem) { 
  int64_t addressToReturn = _genResult.size();

  _convertSyntaxNode(elem->condition);
  PUSH_UPCODE_INT_PARAM(UPCODES::JUMP_IF_FALSE, 0);
  int64_t* addressToJumpIfFalse = &dynamic_cast<_paramInt*>(_genResult.back().param)->in;

  _convertSyntaxNode(elem->body);

  PUSH_UPCODE_INT_PARAM(UPCODES::JUMP, addressToReturn);
  *addressToJumpIfFalse = _genResult.size();
}

void Generation::_convertSyntaxNode(Syntax::TFor* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TForEach* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TPrint* elem) { 
  _convertSyntaxNode(elem->condition);
  PUSH_UPCODE(UPCODES::PRINT);
}

void Generation::_convertSyntaxNode(Syntax::TRead* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TFunction* elem) {
  _functions[*elem->preffix] = elem->body ? _genResult.size() : -1;
  if (elem->body) {
    PUSH_UPCODE(UPCODES::NOT_PRE_INIT_BLOCK);
    if (elem->nameFunction->lexem == "main") {
      PUSH_UPCODE(UPCODES::START_PROGRAM);
      Generation::_StartProgram = _genResult.size();
    }
    auto parameters = &elem->parameters;
    if (_functionsDefaultsValue.contains(*elem->preffix)) {
      bool flag = false;
      auto vec = _functionsDefaultsValue[*elem->preffix];
      for (auto& it : *vec) {
        if (it->exp) {
          flag = true;
          break;
        }
      }
      if (flag) {
        std::map<std::string, std::string> prikol;
        for (size_t i = 0; i < parameters->size(); ++i) {
          prikol[*vec->at(i)->preffix] = *parameters->at(i)->preffix;
          *vec->at(i)->preffix = *parameters->at(i)->preffix;
        }
        parameters = vec;
        for (auto& it : *parameters) {
          for (auto& token : it->exp->polis) {
            if (token->type == Type::ID && prikol.contains(token->lexem)) {
              token->lexem = prikol[token->lexem];
            }
          }
        }
      }
    }

    _functionsDefaultsValue[*elem->preffix] = parameters;
    _functionsDefaultsValueJumpTable[*elem->preffix] = std::vector<int64_t>();

    for (auto& it : *parameters) {
      auto typeInfo = new TypeInfo(0, it->type->typrStr);
      Syntax::_countAndRemovePoints(&typeInfo->type, typeInfo->points, typeInfo->isReference);
      if (_structs.contains(typeInfo->type)) {
        typeInfo->isStruct = true;
        typeInfo->size = _structs[typeInfo->type]->size;
        if (typeInfo->points == 0) {
          PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)_genResult.size() + 3);
          PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, typeInfo->type);
          PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, *it->preffix);
          PUSH_UPCODE_INT_PARAM(UPCODES::CREATE_STRUCT, _structs[typeInfo->type]->constrAddr);
        }
      }
      if (!typeInfo->isStruct || typeInfo->points) {
        Syntax::_countBitSize(&typeInfo->type, typeInfo->size);
        typeInfo->size >>= 3;
        if (typeInfo->points) {
          typeInfo->baseStep = typeInfo->size;
          typeInfo->size = 8;
        }
        Syntax::_transformToBaseType(&typeInfo->type);

        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->points);
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, typeInfo->type);
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->isReference);
        PUSH_UPCODE_STRING_PARAM(UPCODES::INIT_VARIABLE, *it->preffix);
      }
      if (it->exp) {
        _convertSyntaxNode(it->exp);
        PUSH_UPCODE(UPCODES::POP);
      }
      _functionsDefaultsValueJumpTable[*elem->preffix].push_back(_genResult.size());
    }
    
    _convertSyntaxNode(elem->body);
    // default return value, also in void functions
    PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, 0);
    PUSH_UPCODE(UPCODES::SWAP);
    PUSH_UPCODE(UPCODES::END);
    PUSH_UPCODE(UPCODES::END_NOT_PRE_INIT_BLOCK);
  } else {
    _functionsDefaultsValue[*elem->preffix] = &elem->parameters;
  }
}

void Generation::_convertSyntaxNode(Syntax::TStruct* elem) {
  PUSH_UPCODE(UPCODES::NOT_PRE_INIT_BLOCK);
  _structs[elem->name->lexem] = new StructInfo(_genResult.size(), elem);
  PUSH_UPCODE(UPCODES::END_NOT_PRE_INIT_BLOCK);
  return;
}

void Generation::_convertSyntaxNode(Syntax::TReturn* elem) {
  if (elem->exp && elem->exp->polis.size()) {
    _convertSyntaxNode(elem->exp);
    PUSH_UPCODE(UPCODES::SWAP);
    PUSH_UPCODE(UPCODES::END);
  } else {
    PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, 0);
    PUSH_UPCODE(UPCODES::SWAP);
    PUSH_UPCODE(UPCODES::END);
  }
}

Generation::StructInfo::StructInfo(int64_t constrAddr, Syntax::TStruct* tstruct) {
  this->constrAddr = constrAddr;
  this->size = 0;
  for (auto& elem : tstruct->parameters) {
    //this->size in that context is current offset for variable
    Generation::TypeInfo* typeInfo =
    fields[elem->name->lexem] = new TypeInfo(this->size, elem->type->typrStr);
    Syntax::_countAndRemovePoints(&typeInfo->type, typeInfo->points, typeInfo->isReference);
    if (_structs.contains(typeInfo->type)) {
      typeInfo->isStruct = true;
      typeInfo->size = _structs[typeInfo->type]->size;
      if (typeInfo->points == 0) {
        PUSH_UPCODE(UPCODES::DUP);
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)_genResult.size() + 1);
        PUSH_UPCODE(UPCODES::SWAP);
        PUSH_UPCODE_INT_PARAM(UPCODES::CREATE_FIELD_STRUCT, _structs[typeInfo->type]->constrAddr);
      }
    }
    if (!typeInfo->isStruct || typeInfo->points) {
      Syntax::_countBitSize(&typeInfo->type, typeInfo->size);
      typeInfo->size >>= 3;
      if (typeInfo->points) {
        typeInfo->baseStep = typeInfo->size;
        typeInfo->size = 8;
      }
      Syntax::_transformToBaseType(&typeInfo->type);
      if (elem->exp) {
        Generation::_convertSyntaxNode_WithSeparate(elem->exp);
      }
      PUSH_UPCODE(UPCODES::DUP);
      PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
      PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->points);
      PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_CONST_STRING, typeInfo->type);
      PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->isReference);
      PUSH_UPCODE_STRING_PARAM(UPCODES::INIT_VARIABLE_WITHOUT_CREATE, *elem->preffix);
    }

    PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
    PUSH_UPCODE(UPCODES::BINARY_ADD);

    this->size += typeInfo->size;
  }
  PUSH_UPCODE(UPCODES::POP);
  PUSH_UPCODE(UPCODES::END);
}

Generation::TypeInfo::TypeInfo(int64_t offset, std::string type) {
  this->offset = offset;
  this->type = type;
}

Generation::TypeInfo::TypeInfo(std::string type, int64_t size) {
  this->type = type;
  this->baseStep =  this->size = size;
}

Generation::TypeInfo::TypeInfo(std::string type, int64_t size, int64_t points, int64_t baseStep) {
  this->type = type;
  this->size = size;
  this->points = points;
  this->baseStep = baseStep;
}

Generation::TypeInfo::TypeInfo(std::string type, int64_t size, int64_t points, bool isReference) {
  this->type = type;
  this->size = size;
  this->points = points;
  this->isReference = isReference;
}

Generation::TypeInfo::TypeInfo(TypeInfo* elem) {
  this->type = elem->type;
  this->size = elem->size;
  this->points = elem->points;
  this->isReference = elem->isReference;
  this->isStruct = elem->isStruct;
  this->baseStep = elem->baseStep;
  this->offset = elem->offset;
}

