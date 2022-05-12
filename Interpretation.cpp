#include "Interpretation.h"

Interpretation::Interpretation(Generation* gen) {
  _preInit();
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
}
