#include "Generation.h"

#define TRY_CAST_AND_CALL_FUNCTION(cast_type, var) \
  if(dynamic_cast<##cast_type>(##var)){\
    _convertSyntaxNode(dynamic_cast<##cast_type>(##var));\
  }\


enum class UPCODES {
  CREATE_STRUCT,
  CREATE_FIELD_STRUCT,

  LOAD_CONST_INT,
  LOAD_CONST_CHAR,
  LOAD_CONST_FLOAT,
  LOAD_CONST_STRING,
  LOAD_STRING,

  BINARY_ADD,

  ASSIGN_BY_ADDRESS,

  DUP,
  SWAP,
  POP,
  END
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
    switch (elem.code) {
    case UPCODES::CREATE_STRUCT:
        std::cout << "CREATE_STRUCT\n";
        break;
    case UPCODES::CREATE_FIELD_STRUCT:
      std::cout << "CREATE_FIELD_STRUCT\n";
      break;
    case UPCODES::LOAD_CONST_INT:
      std::cout << "LOAD_CONST_INT\n";
      break;
    case UPCODES::BINARY_ADD:
      std::cout << "BINARY_ADD\n";
      break;
    case UPCODES::ASSIGN_BY_ADDRESS:
      std::cout << "ASSIGN_BY_ADDRESS\n";
      break;
    case UPCODES::SWAP:
      std::cout << "SWAP\n";
      break;
    case UPCODES::POP:
      std::cout << "POP\n";
      break;
    case UPCODES::END:
      std::cout << "END\n";
      break;
    case UPCODES::DUP:
      std::cout << "DUP\n";
      break;
    default:
      break;
    }
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
  _genResult.emplace_back(UPCODES::ASSIGN_BY_ADDRESS, Generation::_param{});
}

void Generation::_convertSyntaxNode(Syntax::Exp* elem) {
  for (auto& it : elem->polis) {
    std::string tmp = it->lexem;
    switch (it->type) {
      case Type::ID:
        _genResult.emplace_back(UPCODES::LOAD_STRING, Generation::_paramStr{ it->lexem });
        break;
      case Type::NUMBER:

        break;
      case Type::CHAR:
        _genResult.emplace_back(UPCODES::LOAD_STRING, Generation::_paramChar{ it->lexem[1] });
        break;
      case Type::LITERAL:
        tmp.erase(tmp.begin());
        tmp.erase(tmp.end() - 1);
        _genResult.emplace_back(UPCODES::LOAD_STRING, Generation::_paramStr{ tmp });
        break;
      case Type::OPERATOR:
        if (it->lexem == "b+") {
          ;
        }
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
        _genResult.emplace_back(UPCODES::DUP, Generation::_param{});
        _genResult.emplace_back(UPCODES::LOAD_CONST_INT,      Generation::_paramInt{ (int64_t)_genResult.size() });
        _genResult.emplace_back(UPCODES::SWAP,                Generation::_param{});
        _genResult.emplace_back(UPCODES::CREATE_FIELD_STRUCT, Generation::_paramInt{ _structs[typeInfo->type]->constrAddr });
        _genResult.emplace_back(UPCODES::LOAD_CONST_INT, Generation::_paramInt{ (int64_t)typeInfo->size });
        _genResult.emplace_back(UPCODES::BINARY_ADD, Generation::_param{});
      } else {
        Generation::_convertSyntaxNode_WithSeparate(elem->exp);
        _genResult.emplace_back(UPCODES::LOAD_CONST_INT, Generation::_paramInt{ (int64_t)typeInfo->size });
        _genResult.emplace_back(UPCODES::BINARY_ADD, Generation::_param{});
      }
    } else {
      Syntax::_countBitSize(&typeInfo->type, typeInfo->size);
      typeInfo->size >>= 3;
      Generation::_convertSyntaxNode_WithSeparate(elem->exp);
      _genResult.emplace_back(UPCODES::LOAD_CONST_INT, Generation::_paramInt{ (int64_t)typeInfo->size });
      _genResult.emplace_back(UPCODES::BINARY_ADD, Generation::_param{});
    }
    if (typeInfo->points) {
      typeInfo->baseStep = typeInfo->size;
      typeInfo->size = 8;
    }
    Syntax::_transformToBaseType(&typeInfo->type);
    this->size += typeInfo->size;
  }
  _genResult.emplace_back(UPCODES::POP, Generation::_param{});
  _genResult.emplace_back(UPCODES::END, Generation::_param{});
}

Generation::StructInfo::TypeInfo::TypeInfo(int64_t offset, std::string type) {
  this->offset = offset;
  this->type = type;
}
