#include "Generation.h"

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
  _makeFunctionTable(elem);
  return;
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

void Generation::_makeFunctionTable(Syntax::TProgram* elem) {
  return;
}
