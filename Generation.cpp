#include "Generation.h"

enum class UPCODES {
  //
};

std::map<std::string, Generation::Variable> Generation::_variables = {

};

std::map<std::string, int64_t> Generation::_functions = {

};

Generation::Generation(Syntax* syntax) {
  _program = syntax->getProgram();
}
