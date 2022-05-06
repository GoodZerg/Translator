#pragma once

#include "ExpParser.h"
#include <stack>
class Syntax {
public:
  Syntax(Lex* lex);

  struct TNode {
    virtual ~TNode() = default;
  };

  struct TProgram : public TNode {
    std::vector<TNode*> program;
  };

  struct TSingleKeyWord : public TNode {
    Token* keyWord = nullptr;
  };

  struct TType : public TNode {
    std::string typrStr;
  };

  struct TBlock : public TNode {
    std::vector<TNode*> nodes;
  };

  typedef ExpParser Exp;

  struct TExp : public TNode {
    Exp* exp;
    TExp(Exp* exp) : exp(exp) {
    }
  };
  
  struct TInit : TNode {
    TType* type;

    struct _variable {
      Token* name;
      //std::string prefix;
      Exp* exp;
    };

    std::vector<_variable*> variables;

  };

  struct TIf : TNode {
    Exp* condition;
    TBlock* body = nullptr;
    TBlock* elseBody = nullptr;
  };

  struct TWhile : TNode {
    Exp* condition;
    TBlock* body = nullptr;
  };

  struct TFor : TNode {
    Exp* exp1;
    TInit* init = nullptr;
    Exp* exp2;
    Exp* exp3;
    TBlock* body = nullptr;
  };

  struct TForEach : TNode {
    Exp* exp1;
    TType* type = nullptr;
    TBlock* body = nullptr;
  };

  struct TPrint : TNode {
    Exp* condition;
  };

  struct TRead : TNode {
    Exp* condition;
  };

  struct TMatch : TNode {}; // TODO

  struct _parameter {
    TType* type;
    Token* name;
    Exp* exp;
  };

  struct TFunction : TNode {
    Token* nameStruct = nullptr;
    Token* nameFunction;
    std::vector<_parameter*> parameters;
    TType* type;
    TBlock* body = nullptr;
    int64_t indexStartDefault = 0;
  };

  struct TStruct : TNode {
    Token* name;
    std::vector<_parameter*> parameters;
  };

  struct TReturn : TNode {
    Exp* exp;
  };

  /* semantic */

  struct Variable;
  struct Function;

  struct TypeStruct {
    std::string type;
    std::vector<Variable*> stVariables;
    std::map<std::string, std::vector<Function*>> stFunctions;
  };

  struct Variable {
    TypeStruct* typest;
    std::string name, type;
    Variable(std::string& name, std::string& type, TypeStruct* typest = nullptr) {
      this->name = name;
      this->type = type;
      this->typest = typest;
    }
  };

  struct Function {
    TypeStruct* belongToStruct;
    std::string name, retType;
    std::vector<Variable*> parameters;
    int64_t indexStartDefault = 0;
    bool isImplemented;
    Function(std::string& name, std::string& retType, int64_t indexStartDefault, bool isImplemented = false, TypeStruct* belongToStruct = nullptr) {
      this->name = name;
      this->retType = retType;
      this->indexStartDefault = indexStartDefault;
      this->belongToStruct = belongToStruct;
      this->isImplemented = isImplemented;
    }
  };

  struct SemanticTree {
    std::vector<SemanticTree*> nodes;
    std::vector<Variable*> localVariables;
    SemanticTree* parent;
    SemanticTree(SemanticTree* parent = nullptr) {
      this->parent = parent;
    }
  };

  struct polisType {
    std::string* type;
    int64_t bitSize;
    int64_t points;
    bool isType, isReference, isStruct;

    polisType(std::string type, bool isType = false, bool isReference = true);
    ~polisType();

    inline void countAndRemovePoints() { _countAndRemovePoints(type, points, isReference); };
    inline void countBitSize()         { _countBitSize        (type, bitSize); };
    inline void transformToBaseType()  { _transformToBaseType (type); };

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

  typedef SyntaxError SemanticError;

  typedef bool(*compFunctions)(const Syntax::Function&, const std::vector<polisType*>&);
  typedef bool(*castFunction) (const polisType* first, const polisType* second);
  
  typedef std::map<std::string, std::vector<Function*>> FunctionTable;

  [[nodiscard]] TProgram* getProgram();
  [[nodiscard]] FunctionTable* getFunctionTable();

  static void _countAndRemovePoints(std::string* type, int64_t& points, bool& isReference);
  static void _countBitSize(std::string* type, int64_t& bitSize);
  static void _transformToBaseType(std::string* type);

private:
  Lex* lex;

  TProgram* _program;
  
  void              _parseGeneral();
  void              _parseExpression(Exp*& exp, std::string end_symbol);
  int64_t           _parseParameters(std::vector<_parameter*>& parameters, std::string end_symbol);
  TFunction*        _parseFunction(TFunction* function = nullptr);
  TFunction*        _parseFunctionWithStruct(Token* name);
  TSingleKeyWord*   _parseSingleKeyWord();
  TType*            _parseType();
  TBlock*           _parseBlock();
  TInit*            _parseInit();
  TIf*              _parseIf();
  TWhile*           _parseWhile();
  TFor*             _parseFor();
  TForEach*         _parseForEach();
  TPrint*           _parsePrint();
  TRead*            _parseRead();
  TStruct*          _parseStruct();
  TReturn*          _parseReturn();
  TMatch*           _parseMatch(); // TODO

  /* semantic */

  static std::map<std::string, int64_t> typesCastPriop;
  
  std::vector<TypeStruct*>  _structsTable   = {};
  std::vector<Variable*>    _variablesTable = {};
  static FunctionTable      _functionsTable;
  static SemanticTree*      _sRoot,* _sCurrent;

  bool        _flagOfSemantic = true;
  bool        _isReturn = false;
  std::string _returnValueType = "";

  bool         _checkTypeStructInit(Token* token);
  bool         _checkSecondID(Token* token); //check next token is ID(Exp or TypeStructInit)
  bool         _checkFunctionInTable(Function* function, Token* errorPoint = nullptr, 
                 std::map<std::string, std::vector<Function*>>& functionsTable = _functionsTable); // return true if replace prototype
  void         _checkVariableExistance(SemanticTree* tree, std::string& name, std::string& type);
  std::string  _checkNumberType(std::string& type);

  void _addStructToTable(TStruct* tstruct);
  void _addFunctionToTable(TFunction* function, Token* errorPoint = nullptr);
  void _addVariableToSemanticTree(SemanticTree* tree, std::string& name, std::string& type);
  
  static std::string*             _findVariableInTree(std::string* name, bool& isStruct);
  static std::string*             __findVariableInTree(std::string* name, SemanticTree* node, bool& isStruct);
  std::string*                    _findVariableInStruct(std::string& type, std::string& variable);
  TypeStruct*                     _findTypeStruct(std::string& type);
  std::vector<Syntax::Function*>& _findFunctionInStruct(std::string& type, std::string& function);
  std::vector<Syntax::Function*>& _findFunctionInTable(std::string& function);
  Syntax::Function*               _findSubstitution(std::vector<Function*>& functions, std::vector<polisType*>& parametrs,
                                     Token* error, castFunction cast);
 
  Variable* _castParametrToVariable(_parameter* parametr);
  void      _castTypesBinaryOperation(polisType& first, polisType& second, Token* error);
  void      _castSpecialType(polisType& first, std::string second, Token* error);
  void      _castPointersType(polisType& first, polisType& second, Token* error);

  void _validatePolis(std::vector<Token*>& exp);

  static bool _compFunctions(const Syntax::Function&, const std::vector<polisType*>&, castFunction);
  
};

