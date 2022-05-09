#pragma once

#include "Syntax.h"


#include "Upcodes.h"

class Generation {
public:
  Generation(Syntax* syntax);

  struct TypeInfo {
    int64_t offset = 0, size = 0, points = 0, baseStep = 0;
    std::string type = "";
    bool isReference, isStruct = false;
    TypeInfo(int64_t offset, std::string type);
  };

  struct _param {
    _param() {};
    virtual ~_param() {};
  };
  struct _paramStr    : _param {
    std::string str;
    _paramStr(std::string str) : str(str) {};
  };
  struct _paramInt    : _param {
    int64_t in;
    _paramInt(int64_t in) : in(in) {};
  };
  struct _paramChar   : _param {
    char ch;
    _paramChar(char ch) : ch(ch) {};
  };
  struct _paramFloat  : _param {
    long double fl;
    _paramFloat(long double fl) : fl(fl) {};
  };

  struct _upCode {
    UPCODES code;
    _param* param;
  };

  struct Variable {

  };

  struct StructInfo {
    std::map<std::string, TypeInfo*> fields;
    int64_t constrAddr = -1, size = 0;
    StructInfo(int64_t constrAddr, Syntax::TStruct* tstruct);
  };

private:
  static std::map<std::string, UPCODES> _operations;

  static std::map<std::string, int64_t>  _functions;
  static std::map<std::string, std::vector<Syntax::_parameter*>*> _functionsDefaultsValue;

  static std::map<std::string, StructInfo*> _structs;
  static std::vector<_upCode> _genResult;

  Syntax::TProgram* _program = nullptr;

  static void _convertSyntaxNode                 (Syntax::TNode*          elem);
  static void _convertSyntaxNode                 (Syntax::TProgram*       elem);
  static void _convertSyntaxNode                 (Syntax::TSingleKeyWord* elem);
  static void _convertSyntaxNode                 (Syntax::TType*          elem);
  static void _convertSyntaxNode                 (Syntax::TBlock*         elem);
  static void _convertSyntaxNode                 (Syntax::TExp*           elem);
  static void _convertSyntaxNode_WithSeparate    (Syntax::Exp*            elem);
  static void _convertSyntaxNode                 (Syntax::Exp*            elem);
  static void _convertSyntaxNode                 (Syntax::TInit*          elem);
  static void _convertSyntaxNode                 (Syntax::TIf*            elem);
  static void _convertSyntaxNode                 (Syntax::TWhile*         elem);
  static void _convertSyntaxNode                 (Syntax::TFor*           elem);
  static void _convertSyntaxNode                 (Syntax::TForEach*       elem);
  static void _convertSyntaxNode                 (Syntax::TPrint*         elem);
  static void _convertSyntaxNode                 (Syntax::TRead*          elem);
  static void _convertSyntaxNode                 (Syntax::TFunction*      elem);
  static void _convertSyntaxNode                 (Syntax::TStruct*        elem);
  static void _convertSyntaxNode                 (Syntax::TReturn*        elem);
};
