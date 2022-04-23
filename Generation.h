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
  std::vector<_param> _genResoult;

  void _convertSyntaxNode(Syntax::TNode);

};
