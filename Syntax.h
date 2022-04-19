#pragma once

#include "ExpParser.h"
#include <stack>
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
    std::map<std::string, std::vector<Function*>> stFunctions;
  };

  std::vector<TypeStruct*> _structsTable = {};
  TypeStruct* _findTypeStruct(std::string& type);
  void _addStructToTable(TStruct* tstruct);

  struct Variable {
    TypeStruct* typest;
    std::string name, type;
    Variable(std::string& name, std::string& type, TypeStruct* typest = nullptr) {
      this->name = name;
      this->type = type;
      this->typest = typest;
    }
  };

  std::vector<Variable*> _variablesTable ;
  Variable* _castParametrToVariable(_parameter* parametr);

  struct Function {
    TypeStruct* belongToStruct;
    std::string name, retType;
    std::vector<Variable*> parameters;
    int64_t indexStartDefault;
    bool isImplemented;
    Function(std::string& name, std::string& retType, int64_t indexStartDefault, bool isImplemented = false, TypeStruct* belongToStruct = nullptr) {
      this->name = name;
      this->retType = retType;
      this->indexStartDefault = indexStartDefault;
      this->belongToStruct = belongToStruct;
      this->isImplemented = isImplemented;
    }
  };
  static std::map<std::string, std::vector<Function*>> _functionsTable;

  void _addFunctionToTable(TFunction* function, Token* errorPoint = nullptr);
  bool _checkFunctionInTable(Function* function, Token* errorPoint = nullptr, 
    std::map<std::string, std::vector<Function*>>& functionsTable = _functionsTable); // return true if replace prototype

  struct SemanticTree {
    std::vector<SemanticTree*> nodes;
    std::vector<Variable*> localVariables;
    SemanticTree* parent;
    SemanticTree(SemanticTree* parent = nullptr) {
      this->parent = parent;
    }
  };

  static SemanticTree* _sRoot,* _sCurrent;

  void _addVariableToSemanticTree(SemanticTree* tree, std::string& name, std::string& type);
  void _checkVariableExistance(SemanticTree* tree, std::string& name, std::string& type);

  typedef SyntaxError SemanticError;

  void _validatePolis(std::vector<Token*>& exp);
  
  bool _flagOfSemantic = true;
  std::string _returnValueType = "";
  bool _isReturn = false;

  struct polisType {
    std::string* type;
    int64_t bitSize;
    int64_t points;
    bool isType, isReference, isStruct;

    polisType(std::string type, bool isType = false, bool isReference = true);
    ~polisType();

    void countAndRemovePoints();
    void countBitSize();
    
    void transformToBaseType();
    void transformVariableToType(Token* error);

    void validateType();

    friend void swap(polisType& first, polisType& second) {
      std::swap(first.type, second.type);
      std::swap(first.bitSize, second.bitSize);
      std::swap(first.points, second.points);
      std::swap(first.isReference, second.isReference);
      std::swap(first.isStruct, second.isStruct);
      std::swap(first.isType, second.isType);
    }

    void clear();
    void clear(std::string type);

    bool operator==(const polisType& second) const;
    bool operator!=(const polisType& second) const;
    polisType& operator=(polisType& second);
  };

  static std::map<std::string, int64_t> typesCastPriop;

  static std::string* _findVariableInTree(std::string* name, bool& isStruct);
  static std::string* __findVariableInTree(std::string* name, SemanticTree* node, bool& isStruct);
  
  std::string* _findVariableInStruct(std::string& type, std::string& variable);
  std::vector<Syntax::Function*>& _findFunctionInStruct(std::string& type, std::string& function);
  std::vector<Syntax::Function*>& _findFunctionInTable(std::string& function);

  std::string _checkNumberType(std::string& type);

  void _castTypesBinaryOperation(polisType& first, polisType& second, Token* error);
  void _castSpecialType(polisType& first, std::string second, Token* error);
  void _castPointersType(polisType& first, polisType& second, Token* error);

  typedef bool(*compFunctions)(const Syntax::Function&, const std::vector<polisType*>&);
  typedef bool(*castFunction) (const polisType* first, const polisType* second);

  static bool _compFunctions(const Syntax::Function&, const std::vector<polisType*>&, castFunction) noexcept;

  Syntax::Function* _findSubstitution(std::vector<Function*>& functions, std::vector<polisType*>& parametrs,
    Token* error, castFunction cast);
  
};

