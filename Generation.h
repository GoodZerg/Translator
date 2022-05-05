#pragma once

#include "Syntax.h"

enum class UPCODES;


class Generation {
public:
  Generation(Syntax* syntax);

  struct _param { };
  struct _paramStr    : _param {
    std::string str;
  };
  struct _paramInt    : _param {
    int64_t in;
  };
  struct _paramChar   : _param {
    char ch;
  };
  struct _paramFloat  : _param {
    double fl;
  };

  struct _upCode {
    UPCODES _code;
    _param param;
  };

  struct Variable {

  };

private:
  static std::map<std::string, Variable> _variables;
  static std::map<std::string, int64_t>  _functions;

  Syntax::TProgram* _program = nullptr;
  std::vector<_upCode> _genResult;

  void _convertSyntaxNode(Syntax::TProgram*       elem);
  void _convertSyntaxNode(Syntax::TSingleKeyWord* elem);
  void _convertSyntaxNode(Syntax::TType*          elem);
  void _convertSyntaxNode(Syntax::TBlock*         elem);
  void _convertSyntaxNode(Syntax::TExp*           elem);
  void _convertSyntaxNode(Syntax::TInit*          elem);
  void _convertSyntaxNode(Syntax::TIf*            elem);
  void _convertSyntaxNode(Syntax::TWhile*         elem);
  void _convertSyntaxNode(Syntax::TFor*           elem);
  void _convertSyntaxNode(Syntax::TForEach*       elem);
  void _convertSyntaxNode(Syntax::TPrint*         elem);
  void _convertSyntaxNode(Syntax::TRead*          elem);
  void _convertSyntaxNode(Syntax::TFunction*      elem);
  void _convertSyntaxNode(Syntax::TStruct*        elem);
  void _convertSyntaxNode(Syntax::TReturn*        elem);

  void _makeFunctionTable(Syntax::TProgram* elem);

};
