#include "Generation.h"

#define TRY_CAST_AND_CALL_FUNCTION(cast_type, var) \
  if(dynamic_cast<##cast_type>(##var)){\
    _convertSyntaxNode(dynamic_cast<##cast_type>(##var));\
  }\


enum class UPCODES {
  //
};

std::map<std::string, Generation::Variable> Generation::_variables = {

};

std::map<std::string, int64_t> Generation::_functions = {

};

Generation::Generation(Syntax* syntax) {
  _program = syntax->getProgram();
  _convertSyntaxNode(_program);
}

void Generation::_convertSyntaxNode(Syntax::TProgram* elem) { 
  // what do we do with structures ??
  for (auto& it : elem->program) {
    _convertSyntaxNode(it);
  }
  return;
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
  return;
}

void Generation::_convertSyntaxNode(Syntax::TInit* elem) { 
  return;
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
  return;
}

void Generation::_convertSyntaxNode(Syntax::TReturn* elem) { 
  return;
}

