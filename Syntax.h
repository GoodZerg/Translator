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
  int64_t _parseParameters(std::vector<_parameter*>& parameters, std::string end_symbol);

  struct TFunction : TNode {
    Token* nameStruct = nullptr;
    Token* nameFunction;
    std::vector<_parameter*> parameters;
    TType* type;
    TBlock* body = nullptr;
    int64_t indexStartDefault;
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

  TProgram* _program;

  bool _checkTypeStructInit(Token* token);
  bool _checkSecondID(Token* token); //check next token is ID(Exp or TypeStructInit)

  struct Variable;
  struct Function;

  struct TypeStruct {
    std::string type;
    std::vector<Variable*> stVariables;
    std::vector<Function*> stFunctions;
  };
  static std::vector<TypeStruct*> _structsTable;
  TypeStruct* _findTypeStruct(std::string& type);

  struct Variable {
    TypeStruct* typest;
    std::string name, type;
    Variable(std::string& name, std::string& type, TypeStruct* typest = nullptr) {
      this->name = name;
      this->type = type;
      this->typest = typest;
    }
  };
  static std::vector<Variable*> _variablesTable;

  Variable* _castParametrToVariable(_parameter* parametr);

  struct Function {
    TypeStruct* belongToStruct;
    std::string name, retType;
    std::vector<Variable*> parameters;
    int64_t indexStartDefault;
    Function(std::string& name, std::string& retType, int64_t indexStartDefault, TypeStruct* belongToStruct = nullptr) {
      this->name = name;
      this->retType = retType;
      this->indexStartDefault = indexStartDefault;
      this->belongToStruct = belongToStruct;
    }
  };
  static std::vector<Function*> _functionsTable;
  void _addFunctionToTable(TFunction* function);
  void _findFunctionInTable(Function* function);

  struct SemanticTree {
    std::vector<SemanticTree*> nodes;
    std::vector<Variable*> localVariables;
    SemanticTree* parent;
    SemanticTree(SemanticTree* parent = nullptr) {
      this->parent = parent;
    }
  };
  
  SemanticTree* _sRoot,* _sCurrent;
  void _addVariableToSemanticTree(SemanticTree* tree, std::string& name, std::string& type);
  void _addVariableToTable();
  void _checkVariableExistance(SemanticTree* tree, std::string& name, std::string& type);
};

