#pragma once
#include "Lex.h"
class Syntax {
public:
  Syntax(char* input);
private:
  Lex* lex;

  struct TNode {
    virtual ~TNode() = default;
  };


  struct TSingleKeyWord : public TNode {
    Token* keyWord = nullptr;
  };


  struct TType : public TNode {
    Token* type = nullptr;
    Token* name = nullptr;
    std::string typrStr;
  };
  TType* _parseType();

  struct TBlock : public TNode {
    std::vector<TNode*> nodes;
  };
  TBlock* _parseBlock();


  typedef std::vector<Token*> Exp;

  struct TExp : public TNode {
    Exp exp;
  };
  TExp* _parseExpression();


  struct TInit : TNode {
    TType* type;

    struct _variable {
    Token* name;
    Exp exp;
    };

    std::vector<_variable*> Variables;

  };
  TInit* _parseInit();

  struct TIf : TNode {
    Exp condition;
    TBlock* body = nullptr;
    TBlock* elseBody = nullptr;
  };
  TIf* _parseIf();


  struct TWhile : TNode {
    Exp condition;
    TBlock* body = nullptr;
  };
  TWhile* _parseWhile();


  struct TFor : TNode {
    Exp exp1;
    TInit* init = nullptr;
    Exp exp2;
    Exp exp3;
    TBlock* body = nullptr;
  };
  TFor* _parseFor();


  struct TForEach : TNode {
    Exp exp1;
    TInit* init = nullptr;
    Exp exp2;
    TBlock* body = nullptr;
  };
  TForEach* _parseForEach();


  struct TPrint : TNode {
    Exp condition;
  };
  TPrint* _parsePrint();


  struct TRead : TNode {
    Exp condition;
  };
  TRead* _parseRead();


  struct TMatch : TNode {
    ////////////////////////////////////// TODO
  };
  TMatch* _parseMatch();


  struct TFunction : TNode {
    Token* nameStruct;
    Token* nameFunction;
    struct _parameter {
      TType* type;
      Token* name;
    };
    std::vector<_parameter*> parameters;
    TType* type;
    TBlock* body = nullptr;
  };
  TFunction* _parseFunction();

  struct TStruct : TNode {
    Token* name;
    std::vector<TInit*> field;
  };
  TStruct* _parseStruct();

};

