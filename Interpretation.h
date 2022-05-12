#pragma once

#include "Generation.h"

class Interpretation {
public:
  Interpretation(Generation* gen);
  struct VarInfo {
    Generation::TypeInfo* type = nullptr;
    void* value = nullptr;
  };
private:
  std::vector<std::map<std::string, VarInfo*>> _localScopes;
  std::stack<std::vector<VarInfo*>> _interpretationStack;
};

