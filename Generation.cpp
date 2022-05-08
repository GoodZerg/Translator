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
  {"s--", UPCODES::SUFFIX_DECREMENT},
  {".",   UPCODES::POINT},
  {"[]",  UPCODES::INDEX_OPERATOR},
  {"p++", UPCODES::POSTFIX_INCREMENT},
  {"p--", UPCODES::POSTFIX_DECREMENT},
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
  {"<",   UPCODES::COMPARISION_SMALLER},
  {">=",  UPCODES::COMPARISION_GREATER_EQUAL},
  {"<=",  UPCODES::COMPARISION_SMALLER_EQUAL},
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

std::map<std::string, Generation::StructInfo*> Generation::_structs = {

};

std::map<std::string, Generation::Variable> Generation::_variables = {

};

std::map<std::string, int64_t> Generation::_functions = {

};

std::vector<Generation::_upCode> Generation::_genResult = {

};

Generation::Generation(Syntax* syntax) {
  _program = syntax->getProgram();
  _convertSyntaxNode(_program);
#if PRINT_GEN == 1
  for (auto& elem : _genResult) { 
    std::cout << GetStringUPCODES(elem.code) <<" ";
    PRINT_PARAM(elem.param);
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
  return;
}

void Generation::_convertSyntaxNode(Syntax::TExp* elem) { 
  _convertSyntaxNode(elem->exp);
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
        PUSH_UPCODE_CHAR_PARAM(UPCODES::LOAD_STRING, it->lexem[1]);
        break;
      case Type::LITERAL:
        tmp.erase(tmp.begin());
        tmp.erase(tmp.end() - 1);
        PUSH_UPCODE_STRING_PARAM(UPCODES::LOAD_STRING, tmp);
        break;
      case Type::OPERATOR:
        PUSH_UPCODE(_operations[tmp]);
        break;
      default:
        break;
    }
  }
}


void Generation::_convertSyntaxNode(Syntax::TInit* elem) { 
}

void Generation::_convertSyntaxNode(Syntax::TIf* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TWhile* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TFor* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TForEach* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TPrint* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TRead* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TFunction* elem) { 
  return;
}

void Generation::_convertSyntaxNode(Syntax::TStruct* elem) {
  _structs[elem->name->lexem] = new StructInfo(_genResult.size(), elem);
  return;
}

void Generation::_convertSyntaxNode(Syntax::TReturn* elem) { 
  return;
}

Generation::StructInfo::StructInfo(int64_t constrAddr, Syntax::TStruct* tstruct) {
  this->constrAddr = constrAddr;
  this->size = 0;
  for (auto& elem : tstruct->parameters) {
    Generation::StructInfo::TypeInfo* typeInfo =
    fields[elem->name->lexem] = new TypeInfo(this->size, elem->type->typrStr);
    Syntax::_countAndRemovePoints(&typeInfo->type, typeInfo->points, typeInfo->isReference);
    if (_structs.contains(typeInfo->type)) {
      typeInfo->isStruct = true;
      typeInfo->size = _structs[typeInfo->type]->size;
      if (typeInfo->points == 0) {
        PUSH_UPCODE(UPCODES::DUP);
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)_genResult.size());
        PUSH_UPCODE(UPCODES::SWAP);
        PUSH_UPCODE_INT_PARAM(UPCODES::CREATE_FIELD_STRUCT, _structs[typeInfo->type]->constrAddr);
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
        PUSH_UPCODE(UPCODES::BINARY_ADD);
      } else {
        Generation::_convertSyntaxNode_WithSeparate(elem->exp);
        PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
        PUSH_UPCODE(UPCODES::BINARY_ADD);
      }
    } else {
      Syntax::_countBitSize(&typeInfo->type, typeInfo->size);
      typeInfo->size >>= 3;
      Generation::_convertSyntaxNode_WithSeparate(elem->exp);
      PUSH_UPCODE_INT_PARAM(UPCODES::LOAD_CONST_INT, (int64_t)typeInfo->size);
      PUSH_UPCODE(UPCODES::BINARY_ADD);
    }
    if (typeInfo->points) {
      typeInfo->baseStep = typeInfo->size;
      typeInfo->size = 8;
    }
    Syntax::_transformToBaseType(&typeInfo->type);
    this->size += typeInfo->size;
  }
  PUSH_UPCODE(UPCODES::POP);
  PUSH_UPCODE(UPCODES::END);
}

Generation::StructInfo::TypeInfo::TypeInfo(int64_t offset, std::string type) {
  this->offset = offset;
  this->type = type;
}
