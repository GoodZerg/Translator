#pragma once

#include "Generation.h"

class Interpretation {
public:
  Interpretation(Generation* gen);
  struct VarInfo {
    Generation::TypeInfo* type = nullptr;
    void* value = nullptr;
  };

  typedef void(Interpretation::*upCodeFunction)(Generation::_upCode& upCode, int64_t& index);

private:
  std::vector<std::map<std::string, VarInfo*>> _localScopes;
  std::stack<std::vector<VarInfo*>> _interpretationStack;

  std::map<UPCODES, upCodeFunction> _executeFunctions = {
    {UPCODES::CREATE_STRUCT               ,&Interpretation::_executeUpCode_CREATE_STRUCT},
    {UPCODES::CREATE_FIELD_STRUCT         ,&Interpretation::_executeUpCode_CREATE_FIELD_STRUCT},
    {UPCODES::LOAD_CONST_INT              ,&Interpretation::_executeUpCode_LOAD_CONST_INT},
    {UPCODES::LOAD_CONST_CHAR             ,&Interpretation::_executeUpCode_LOAD_CONST_CHAR},
    {UPCODES::LOAD_CONST_FLOAT            ,&Interpretation::_executeUpCode_LOAD_CONST_FLOAT},
    {UPCODES::LOAD_CONST_STRING           ,&Interpretation::_executeUpCode_LOAD_CONST_STRING},
    {UPCODES::LOAD_STRING                 ,&Interpretation::_executeUpCode_LOAD_STRING},
    {UPCODES::LOAD_NAME                   ,&Interpretation::_executeUpCode_LOAD_NAME},
    {UPCODES::CAST                        ,&Interpretation::_executeUpCode_CAST},
    {UPCODES::CALL_FUNCTION               ,&Interpretation::_executeUpCode_CALL_FUNCTION},
    {UPCODES::SUFFIX_INCREMENT            ,&Interpretation::_executeUpCode_SUFFIX_INCREMENT},
    {UPCODES::SUFFIX_DECREMENT            ,&Interpretation::_executeUpCode_SUFFIX_DECREMENT},
    {UPCODES::PREFFIX_INCREMENT           ,&Interpretation::_executeUpCode_PREFFIX_INCREMENT},
    {UPCODES::PREFFIX_DECREMENT           ,&Interpretation::_executeUpCode_PREFFIX_DECREMENT},
    {UPCODES::POINT                       ,&Interpretation::_executeUpCode_POINT},
    {UPCODES::INDEX_OPERATOR              ,&Interpretation::_executeUpCode_INDEX_OPERATOR},
    {UPCODES::UNARY_ADD                   ,&Interpretation::_executeUpCode_UNARY_ADD},
    {UPCODES::UNARY_SUBSTRACTION          ,&Interpretation::_executeUpCode_UNARY_SUBSTRACTION},
    {UPCODES::BINARY_ADD                  ,&Interpretation::_executeUpCode_BINARY_ADD},
    {UPCODES::BINARY_SUBSTRACTION         ,&Interpretation::_executeUpCode_BINARY_SUBSTRACTION},
    {UPCODES::LOGIC_NOT                   ,&Interpretation::_executeUpCode_LOGIC_NOT},
    {UPCODES::BIT_NOT                     ,&Interpretation::_executeUpCode_BIT_NOT},
    {UPCODES::UNARY_DEREFERENCING         ,&Interpretation::_executeUpCode_UNARY_DEREFERENCING},
    {UPCODES::UNARY_GET_ADDRESS           ,&Interpretation::_executeUpCode_UNARY_GET_ADDRESS},
    {UPCODES::POWER                       ,&Interpretation::_executeUpCode_POWER},
    {UPCODES::MILTIPLY                    ,&Interpretation::_executeUpCode_MILTIPLY},
    {UPCODES::DIVISION                    ,&Interpretation::_executeUpCode_DIVISION},
    {UPCODES::MOD                         ,&Interpretation::_executeUpCode_MOD},
    {UPCODES::COMPARISION_GREATER         ,&Interpretation::_executeUpCode_COMPARISION_GREATER},
    {UPCODES::COMPARISION_LESS            ,&Interpretation::_executeUpCode_COMPARISION_LESS},
    {UPCODES::COMPARISION_GREATER_EQUAL   ,&Interpretation::_executeUpCode_COMPARISION_GREATER_EQUAL},
    {UPCODES::COMPARISION_LESS_EQUAL      ,&Interpretation::_executeUpCode_COMPARISION_LESS_EQUAL},
    {UPCODES::COMPARISION_EQUAL           ,&Interpretation::_executeUpCode_COMPARISION_EQUAL},
    {UPCODES::COMPARISION_NOT_EQUAL       ,&Interpretation::_executeUpCode_COMPARISION_NOT_EQUAL},
    {UPCODES::BIT_AND                     ,&Interpretation::_executeUpCode_BIT_AND},
    {UPCODES::BIT_OR                      ,&Interpretation::_executeUpCode_BIT_OR},
    {UPCODES::LOGIC_AND                   ,&Interpretation::_executeUpCode_LOGIC_AND},
    {UPCODES::LOGIC_OR                    ,&Interpretation::_executeUpCode_LOGIC_OR},
    {UPCODES::ADD_ASSIGN                  ,&Interpretation::_executeUpCode_ADD_ASSIGN},
    {UPCODES::SUBSTRACTION_ASSIGN         ,&Interpretation::_executeUpCode_SUBSTRACTION_ASSIGN},
    {UPCODES::MILTIPLY_ASSIGN             ,&Interpretation::_executeUpCode_MILTIPLY_ASSIGN},
    {UPCODES::DIVISION_ASSIGN             ,&Interpretation::_executeUpCode_DIVISION_ASSIGN},
    {UPCODES::MOD_ASSIGN                  ,&Interpretation::_executeUpCode_MOD_ASSIGN},
    {UPCODES::BIT_AND_ASSIGN              ,&Interpretation::_executeUpCode_BIT_AND_ASSIGN},
    {UPCODES::BIT_OR_ASSIGN               ,&Interpretation::_executeUpCode_BIT_OR_ASSIGN},
    {UPCODES::POWER_ASSIGN                ,&Interpretation::_executeUpCode_POWER_ASSIGN},
    {UPCODES::ASSIGN                      ,&Interpretation::_executeUpCode_ASSIGN},
    {UPCODES::COMMA                       ,&Interpretation::_executeUpCode_COMMA},
    {UPCODES::PRINT                       ,&Interpretation::_executeUpCode_PRINT},
    {UPCODES::READ                        ,&Interpretation::_executeUpCode_READ},
    {UPCODES::ASSIGN_BY_ADDRESS           ,&Interpretation::_executeUpCode_ASSIGN_BY_ADDRESS},
    {UPCODES::INIT_VARIABLE               ,&Interpretation::_executeUpCode_INIT_VARIABLE},
    {UPCODES::INIT_VARIABLE_WITHOUT_CREATE,&Interpretation::_executeUpCode_INIT_VARIABLE_WITHOUT_CREATE},
    {UPCODES::JUMP_IF_FALSE               ,&Interpretation::_executeUpCode_JUMP_IF_FALSE},
    {UPCODES::JUMP                        ,&Interpretation::_executeUpCode_JUMP},
    {UPCODES::DUP                         ,&Interpretation::_executeUpCode_DUP},
    {UPCODES::SWAP                        ,&Interpretation::_executeUpCode_SWAP},
    {UPCODES::POP                         ,&Interpretation::_executeUpCode_POP},
    {UPCODES::END                         ,&Interpretation::_executeUpCode_END},
    {UPCODES::START_PROGRAM               ,&Interpretation::_executeUpCode_START_PROGRAM},
    {UPCODES::NOT_PRE_INIT_BLOCK          ,&Interpretation::_executeUpCode_NOT_PRE_INIT_BLOCK},
    {UPCODES::END_NOT_PRE_INIT_BLOCK      ,&Interpretation::_executeUpCode_END_NOT_PRE_INIT_BLOCK} 
  };

  bool _isExecuteUpCode = true;

  void _preInit();
  void _startProgram();
  void _executeUpCode(int64_t& upCodeIndex);


  void _executeUpCode_CREATE_STRUCT                 (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_CREATE_FIELD_STRUCT           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOAD_CONST_INT                (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOAD_CONST_CHAR               (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOAD_CONST_FLOAT              (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOAD_CONST_STRING             (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOAD_STRING                   (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOAD_NAME                     (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_CAST                          (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_CALL_FUNCTION                 (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_SUFFIX_INCREMENT              (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_SUFFIX_DECREMENT              (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_PREFFIX_INCREMENT             (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_PREFFIX_DECREMENT             (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_POINT                         (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_INDEX_OPERATOR                (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_UNARY_ADD                     (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_UNARY_SUBSTRACTION            (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BINARY_ADD                    (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BINARY_SUBSTRACTION           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOGIC_NOT                     (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BIT_NOT                       (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_UNARY_DEREFERENCING           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_UNARY_GET_ADDRESS             (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_POWER                         (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_MILTIPLY                      (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_DIVISION                      (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_MOD                           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMPARISION_GREATER           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMPARISION_LESS              (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMPARISION_GREATER_EQUAL     (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMPARISION_LESS_EQUAL        (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMPARISION_EQUAL             (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMPARISION_NOT_EQUAL         (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BIT_AND                       (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BIT_OR                        (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOGIC_AND                     (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_LOGIC_OR                      (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_ADD_ASSIGN                    (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_SUBSTRACTION_ASSIGN           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_MILTIPLY_ASSIGN               (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_DIVISION_ASSIGN               (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_MOD_ASSIGN                    (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BIT_AND_ASSIGN                (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_BIT_OR_ASSIGN                 (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_POWER_ASSIGN                  (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_ASSIGN                        (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_COMMA                         (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_PRINT                         (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_READ                          (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_ASSIGN_BY_ADDRESS             (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_INIT_VARIABLE                 (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_INIT_VARIABLE_WITHOUT_CREATE  (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_JUMP_IF_FALSE                 (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_JUMP                          (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_DUP                           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_SWAP                          (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_POP                           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_END                           (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_START_PROGRAM                 (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_NOT_PRE_INIT_BLOCK            (Generation::_upCode& upCode, int64_t& index);
  void _executeUpCode_END_NOT_PRE_INIT_BLOCK        (Generation::_upCode& upCode, int64_t& index);

};

