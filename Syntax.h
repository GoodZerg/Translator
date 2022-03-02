#pragma once

#include "ExpParser.h"

class Syntax {
public:
  Syntax(Lex* lex);
private:
  Lex* lex;


  void _parseGeneral();


  struct TNode {
    virtual ~TNode() = default;
  };


  struct TProgram : public TNode {
      std::vector<TNode*> program;
  };


  struct TSingleKeyWord : public TNode {
    Token* keyWord = nullptr;
  };
  TSingleKeyWord* _parseSingleKeyWord();


  struct TType : public TNode {
    std::string typrStr;
  };
  TType* _parseType();


  struct TBlock : public TNode {
    std::vector<TNode*> nodes;
  };
  TBlock* _parseBlock();


  typedef ExpParser Exp;

  void _parseExpression(Exp*& exp, std::string end_symbol);

  struct TExp : public TNode {
    Exp* exp;
    TExp(Exp* exp) : exp(exp) {
    }
  }; // We need this struct to use polymorphism in TBlock :)


  struct TInit : TNode {
    TType* type;

    struct _variable {
    Token* name;
    Exp* exp;
    };

    std::vector<_variable*> variables;

  };
  TInit* _parseInit();

  struct TIf : TNode {
    Exp* condition;
    TBlock* body = nullptr;
    TBlock* elseBody = nullptr;
  };
  TIf* _parseIf();


  struct TWhile : TNode {
    Exp* condition;
    TBlock* body = nullptr;
  };
  TWhile* _parseWhile();


  struct TFor : TNode {
    Exp* exp1;
    TInit* init = nullptr;
    Exp* exp2;
    Exp* exp3;
    TBlock* body = nullptr;
  };
  TFor* _parseFor();


  struct TForEach : TNode {
    Exp* exp1;
    TType* type = nullptr;
    TBlock* body = nullptr;
  };
  TForEach* _parseForEach();


  struct TPrint : TNode {
    Exp* condition;
  };
  TPrint* _parsePrint();


  struct TRead : TNode {
    Exp* condition;
  };
  TRead* _parseRead();


  struct TMatch : TNode {
    ////////////////////////////////////// TODO
  };
  TMatch* _parseMatch();

  struct _parameter {
      TType* type;
      Token* name;
      Exp* exp;
  };
  void _parseParameters(std::vector<_parameter*>& parameters, std::string end_symbol);

  struct TFunction : TNode {
    Token* nameStruct = nullptr;
    Token* nameFunction;
    std::vector<_parameter*> parameters;
    TType* type;
    TBlock* body = nullptr;
  };
  TFunction* _parseFunction(TFunction* function = nullptr);

  TFunction* _parseFunctionWithStruct(Token* name);

  struct TStruct : TNode {
    Token* name;
    std::vector<_parameter*> parameters;
  };
  TStruct* _parseStruct();
  
  struct TReturn : TNode {
    Exp* exp;
  };
  TReturn* _parseReturn();

  TProgram* program;

  bool _checkTypeStructInit(Token* token);
  bool _checkSecondID(Token* token); //check next token is ID(Exp or TypeStructInit)
};

