#include "Interpretation.h"

Interpretation::Interpretation(Generation* gen) {
  _preInit();
	_startProgram();
}

void Interpretation::_preInit() {
	for (int64_t i = 0; i < Generation::_genResult.size(); ++i) {
		if (Generation::_genResult[i].code == UPCODES::NOT_PRE_INIT_BLOCK) {
			_isExecuteUpCode = false;
			continue;
		} else if (Generation::_genResult[i].code == UPCODES::END_NOT_PRE_INIT_BLOCK) {
			_isExecuteUpCode = true;
			continue;
		}
		if (_isExecuteUpCode) {
			_executeUpCode(i);
		}
	}
}

void Interpretation::_startProgram() {
	for (int64_t i = Generation::_StartProgram; i < Generation::_genResult.size(); ++i) {
		_executeUpCode(i);
	}
}

void Interpretation::_executeUpCode(int64_t& upCodeIndex) {
	Generation::_upCode upCode = Generation::_genResult[upCodeIndex];
	auto a = _executeFunctions[upCode.code]();
	a(upCode, upCodeIndex);
}

void Interpretation::_executeUpCode_CREATE_STRUCT                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_CREATE_FIELD_STRUCT           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_CONST_INT                (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_CONST_CHAR               (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_CONST_FLOAT              (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_CONST_STRING             (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_STRING                   (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_NAME                     (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_CAST                          (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_CALL_FUNCTION                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_SUFFIX_INCREMENT              (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_SUFFIX_DECREMENT              (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_PREFFIX_INCREMENT             (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_PREFFIX_DECREMENT             (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_POINT                         (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_INDEX_OPERATOR                (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_UNARY_ADD                     (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_UNARY_SUBSTRACTION            (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BINARY_ADD                    (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BINARY_SUBSTRACTION           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOGIC_NOT                     (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BIT_NOT                       (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_UNARY_DEREFERENCING           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_UNARY_GET_ADDRESS             (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_POWER                         (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_MILTIPLY                      (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_DIVISION                      (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_MOD                           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMPARISION_GREATER           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMPARISION_LESS              (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMPARISION_GREATER_EQUAL     (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMPARISION_LESS_EQUAL        (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMPARISION_EQUAL             (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMPARISION_NOT_EQUAL         (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BIT_AND                       (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BIT_OR                        (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOGIC_AND                     (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOGIC_OR                      (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_ADD_ASSIGN                    (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_SUBSTRACTION_ASSIGN           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_MILTIPLY_ASSIGN               (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_DIVISION_ASSIGN               (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_MOD_ASSIGN                    (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BIT_AND_ASSIGN                (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_BIT_OR_ASSIGN                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_POWER_ASSIGN                  (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_ASSIGN                        (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_COMMA                         (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_PRINT                         (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_READ                          (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_ASSIGN_BY_ADDRESS             (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_INIT_VARIABLE                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_INIT_VARIABLE_WITHOUT_CREATE  (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_JUMP_IF_FALSE                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_JUMP                          (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_DUP                           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_SWAP                          (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_POP                           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_END                           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_START_PROGRAM                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_NOT_PRE_INIT_BLOCK            (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_END_NOT_PRE_INIT_BLOCK				(Generation::_upCode& upCode, int64_t& index){
	return;
}


