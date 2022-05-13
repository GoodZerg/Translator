#include "Interpretation.h"


#define __interpretationStackTopWPop()  \
[&]() -> std::vector<VarInfo*> {				\
auto ahah = _interpretationStack.top(); \
_interpretationStack.pop();							\
return ahah;														\
}

#define _interpretationStackTopWPop() __interpretationStackTopWPop()()

#define intType(bits)   int##bits##_t
#define uintType(bits)  uint##bits##_t
#define floatType(bits) uint##bits##_t

template <typename T>
T _cast(void* pointer, int64_t size, std::string type){
	if (type == "signed") {
		if (size == 2) {
			return *reinterpret_cast<int16_t*>(pointer);
		} else if (size == 4) {
			return *reinterpret_cast<int32_t*>(pointer);
		} else if (size == 8) {
			return *reinterpret_cast<int64_t*>(pointer);
		} else if (size == 16) {
			return *reinterpret_cast<int64_t*>(pointer);////
		}
	} else if (type == "unsigned") {
		if (size == 2) {
			return *reinterpret_cast<uint16_t*>(pointer);
		} else if (size == 4) {
			return *reinterpret_cast<uint32_t*>(pointer);
		} else if (size == 8) {
			return *reinterpret_cast<uint64_t*>(pointer);
		} else if (size == 16) {
			return *reinterpret_cast<uint64_t*>(pointer);////
		}
	} else if (type == "float") {
		if (size == 4) {
			return *reinterpret_cast<float_t*>(pointer);
		} else if (size == 8) {
			return *reinterpret_cast<double_t*>(pointer);
		} else if (size == 16) {
			return *reinterpret_cast<double_t*>(pointer);////
		}
	} else if (type == "char") {
		return *reinterpret_cast<char*>(pointer);
  } else if (type == "string") {
		return *reinterpret_cast<std::string*>(pointer);
	} else if (type == "bool") {
		return *reinterpret_cast<bool*>(pointer);
	}
}

	
template <typename T, typename U>
decltype(auto) plus(T a, U b) {
	return a + b;
}

#define _CHECK1(a,type1,b,type2, oper, type11, size11, type22, size22) \
type1 aa = *reinterpret_cast<type1 *>(a);\
type2 bb = *reinterpret_cast<type2 *>(b);\
auto c = aa oper bb;\
std::string typee = type11;\
int64_t sizee = size11;\
	if (Syntax::typesCastPriop[type11] > Syntax::typesCastPriop[type22]) {\
			typee = type22;\
	} else if (Syntax::typesCastPriop[type11] == Syntax::typesCastPriop[type22] && size11 < size22) {\
		sizee = size22;\
	}\
\
auto cc = new char[sizee];\
memcpy(cc, &c, sizee);\
Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{new Interpretation::VarInfo{new Generation::TypeInfo(typee, sizee, 0, false), (void*)cc}});\
\


#define _CAST1(a,type,size, b,type2, oper, type22, size22) \
if (type == "signed") {\
	if (size == 2) {\
		_CHECK1(a,int16_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 4) {\
		_CHECK1(a,int32_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 8) {\
		_CHECK1(a,int64_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 16) {\
		_CHECK1(a,int64_t, b,type2, ##oper, type, size,type22, size22  )\
	}\
} else if (type == "unsigned") {\
	if (size == 2) {\
		_CHECK1(a,uint16_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 4) {\
		_CHECK1(a,uint32_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 8) {\
		_CHECK1(a,uint64_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 16) {\
		_CHECK1(a,uint64_t, b,type2, ##oper, type, size,type22, size22  )\
	}\
} else if (type == "float") {\
	if (size == 4) {\
		_CHECK1(a,float_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 8) {\
		_CHECK1(a,double_t, b,type2, ##oper, type, size,type22, size22 )\
	} else if (size == 16) {\
		_CHECK1(a,double_t, b,type2, ##oper, type, size,type22, size22  )\
	}\
} else if (type == "char") {\
	_CHECK1(a,char, b,type2, ##oper, type, size,type22, size22  )\
} else if (type == "bool") {\
	_CHECK1(a,bool, b,type2, ##oper, type, size,type22, size22  )\
}\


#define _BY_TYPE1(a,type1,size1,b,type2,size2, oper)\
if (type2 == "signed") {\
	if (size2 == 2) {\
		_CAST1(a,type1, size1, b,int16_t, ##oper, type2, size2   )\
	} else if (size2 == 4) {\
		_CAST1(a,type1, size1, b,int32_t, ##oper, type2, size2  )\
	} else if (size2 == 8) {\
		_CAST1(a,type1, size1, b,int64_t, ##oper, type2, size2  )\
	} else if (size2 == 16) {\
		_CAST1(a,type1, size1, b,int64_t, ##oper, type2, size2  )\
	}\
} else if (type2 == "unsigned") {\
	if (size2 == 2) {\
		_CAST1(a,type1, size1, b,uint16_t, ##oper, type2, size2  )\
	} else if (size2 == 4) {\
		_CAST1(a,type1, size1, b,uint32_t, ##oper, type2, size2 )\
	} else if (size2 == 8) {\
		_CAST1(a,type1, size1, b,uint64_t, ##oper, type2, size2 )\
	} else if (size2 == 16) {\
		_CAST1(a,type1, size1, b,uint64_t, ##oper, type2, size2  )\
	}\
} else if (type2 == "float") {\
	if (size2 == 4) {\
		_CAST1(a,type1, size1, b,float_t, ##oper, type2, size2  )\
	} else if (size2 == 8) {\
		_CAST1(a,type1, size1, b,double_t, ##oper, type2, size2  )\
	} else if (size2 == 16) {\
		_CAST1(a,type1, size1, b,double_t, ##oper, type2, size2  )\
	}\
} else if (type2 == "char") {\
	_CAST1(a,type1, size1, b,char, ##oper, type2, size2  )\
} else if (type2 == "bool") {\
	_CAST1(a,type1, size1, b,bool, ##oper, type2, size2  )\
}\




#define _CHECK(a,type1,b,type2, oper, type11, size11, type22, size22) \
type2 bb = *reinterpret_cast<type2 *>(b);\
*reinterpret_cast<type1 *>(a) oper bb;\
std::string typee = type11;\
int64_t sizee = size11;\
	if (Syntax::typesCastPriop[type11] > Syntax::typesCastPriop[type22]) {\
			typee = type22;\
	} else if (Syntax::typesCastPriop[type11] == Syntax::typesCastPriop[type22] && size11 < size22) {\
		sizee = size22;\
	}\
\
Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{new Interpretation::VarInfo{new Generation::TypeInfo(typee, sizee, 0, false), (void*)a}});\
\


#define _CAST(a,type,size, b,type2, oper, type22, size22) \
if (type == "signed") {\
	if (size == 2) {\
		_CHECK(a,int16_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 4) {\
		_CHECK(a,int32_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 8) {\
		_CHECK(a,int64_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 16) {\
		_CHECK(a,int64_t, b,type2, ##oper, type, size,type22, size22  )\
	}\
} else if (type == "unsigned") {\
	if (size == 2) {\
		_CHECK(a,uint16_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 4) {\
		_CHECK(a,uint32_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 8) {\
		_CHECK(a,uint64_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 16) {\
		_CHECK(a,uint64_t, b,type2, ##oper, type, size,type22, size22  )\
	}\
} else if (type == "float") {\
	if (size == 4) {\
		_CHECK(a,float_t, b,type2, ##oper, type, size,type22, size22  )\
	} else if (size == 8) {\
		_CHECK(a,double_t, b,type2, ##oper, type, size,type22, size22 )\
	} else if (size == 16) {\
		_CHECK(a,double_t, b,type2, ##oper, type, size,type22, size22  )\
	}\
} else if (type == "char") {\
	_CHECK(a,char, b,type2, ##oper, type, size,type22, size22  )\
} else if (type == "bool") {\
	_CHECK(a,bool, b,type2, ##oper, type, size,type22, size22  )\
}\


#define _BY_TYPE(a,type1,size1,b,type2,size2, oper)\
if (type2 == "signed") {\
	if (size2 == 2) {\
		_CAST(a,type1, size1, b,int16_t, ##oper, type2, size2   )\
	} else if (size2 == 4) {\
		_CAST(a,type1, size1, b,int32_t, ##oper, type2, size2  )\
	} else if (size2 == 8) {\
		_CAST(a,type1, size1, b,int64_t, ##oper, type2, size2  )\
	} else if (size2 == 16) {\
		_CAST(a,type1, size1, b,int64_t, ##oper, type2, size2  )\
	}\
} else if (type2 == "unsigned") {\
	if (size2 == 2) {\
		_CAST(a,type1, size1, b,uint16_t, ##oper, type2, size2  )\
	} else if (size2 == 4) {\
		_CAST(a,type1, size1, b,uint32_t, ##oper, type2, size2 )\
	} else if (size2 == 8) {\
		_CAST(a,type1, size1, b,uint64_t, ##oper, type2, size2 )\
	} else if (size2 == 16) {\
		_CAST(a,type1, size1, b,uint64_t, ##oper, type2, size2  )\
	}\
} else if (type2 == "float") {\
	if (size2 == 4) {\
		_CAST(a,type1, size1, b,float_t, ##oper, type2, size2  )\
	} else if (size2 == 8) {\
		_CAST(a,type1, size1, b,double_t, ##oper, type2, size2  )\
	} else if (size2 == 16) {\
		_CAST(a,type1, size1, b,double_t, ##oper, type2, size2  )\
	}\
} else if (type2 == "char") {\
	_CAST(a,type1, size1, b,char, ##oper, type2, size2  )\
} else if (type2 == "bool") {\
	_CAST(a,type1, size1, b,bool, ##oper, type2, size2  )\
}\


Interpretation::Interpretation(Generation* gen) {
	_localScopes.push_back(std::map<std::string, VarInfo*>());
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
	(this->*_executeFunctions[upCode.code])(upCode, upCodeIndex);
}

void Interpretation::_executeUpCode_CREATE_STRUCT                 (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_CREATE_FIELD_STRUCT           (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_CONST_INT                (Generation::_upCode& upCode, int64_t& index){
	Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
		new Interpretation::VarInfo{ new Generation::TypeInfo("signed", 8), 
			new int64_t(dynamic_cast<Generation::_paramInt*>(upCode.param)->in) }});
}

void Interpretation::_executeUpCode_LOAD_CONST_CHAR               (Generation::_upCode& upCode, int64_t& index){
	Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
		new Interpretation::VarInfo{ new Generation::TypeInfo("char", 1),
			new char(dynamic_cast<Generation::_paramChar*>(upCode.param)->ch) }});
}

void Interpretation::_executeUpCode_LOAD_CONST_FLOAT              (Generation::_upCode& upCode, int64_t& index){
	Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
		new Interpretation::VarInfo{ new Generation::TypeInfo("float", 8),
			new double(dynamic_cast<Generation::_paramFloat*>(upCode.param)->fl) }});
}

void Interpretation::_executeUpCode_LOAD_CONST_STRING             (Generation::_upCode& upCode, int64_t& index){
	Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
		new Interpretation::VarInfo{ new Generation::TypeInfo("string", 0),
			new std::string(dynamic_cast<Generation::_paramStr*>(upCode.param)->str) }});
}

void Interpretation::_executeUpCode_LOAD_STRING                   (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_LOAD_NAME                     (Generation::_upCode& upCode, int64_t& index){
	if (_localScopes.back().contains(dynamic_cast<Generation::_paramStr*>(upCode.param)->str)) {
		auto elem = _localScopes.back()[dynamic_cast<Generation::_paramStr*>(upCode.param)->str];
		_interpretationStack.push(std::vector<VarInfo*>{
			new VarInfo{ new Generation::TypeInfo(elem->type),
			elem->value
		}});
	} else {
		auto elem = _localScopes[0][dynamic_cast<Generation::_paramStr*>(upCode.param)->str];
		_interpretationStack.push(std::vector<VarInfo*>{
			new VarInfo{ new Generation::TypeInfo(elem->type),
			elem->value
		}});
	}
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
	auto second = _interpretationStackTopWPop();
	auto first = _interpretationStackTopWPop();
	auto second1 = second.back();
	auto first1 = first.back();
	if (first1->type->points) {
		int64_t aa = *reinterpret_cast<int64_t*>(first1->value);
		int64_t bb = *reinterpret_cast<int64_t*>(second1->value);
		Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
			new Interpretation::VarInfo{ new Generation::TypeInfo("unsigned", 8, first1->type->points, first1->type->baseStep),
				new int64_t(aa + (bb * (first1->type->points < 2 ? first1->type->baseStep : 8))) }});
	} else if(second1->type->points) {
		int64_t aa = *reinterpret_cast<int64_t*>(first1->value);
		int64_t bb = *reinterpret_cast<int64_t*>(second1->value);
		Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
			new Interpretation::VarInfo{ new Generation::TypeInfo("unsigned", 8, second1->type->points, second1->type->baseStep),
				new int64_t(bb + (aa * (second1->type->points < 2 ? second1->type->baseStep : 8))) }});
	} else if (first1->type->type == "string" && second1->type->type == "string") {
		std::string aa = *reinterpret_cast<std::string*>(first1->value);
		std::string bb = *reinterpret_cast<std::string*>(second1->value);
		Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
			new Interpretation::VarInfo{ new Generation::TypeInfo("string", 0),
				new std::string(aa + bb) }});
	} else {
		_BY_TYPE1(first1->value, first1->type->type, first1->type->size,
			second1->value, second1->type->type, second1->type->size, +);
	}
	std::cout << *reinterpret_cast<int32_t*>(_interpretationStack.top().back()->value) << "\n";
}

void Interpretation::_executeUpCode_BINARY_SUBSTRACTION           (Generation::_upCode& upCode, int64_t& index){
	auto second = _interpretationStackTopWPop();
	auto first = _interpretationStackTopWPop();
	auto second1 = second.back();
	auto first1 = first.back();
	if (first1->type->points && second1->type->points) {
		int64_t aa = *reinterpret_cast<int64_t*>(first1->value);
		int64_t bb = *reinterpret_cast<int64_t*>(second1->value); 
		Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
			new Interpretation::VarInfo{ new Generation::TypeInfo("unsigned", 8),
				new int64_t(aa - bb)}});
	} else if (first1->type->points) {
		int64_t aa = *reinterpret_cast<int64_t*>(first1->value);
		int64_t bb = *reinterpret_cast<int64_t*>(second1->value);
		Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
			new Interpretation::VarInfo{ new Generation::TypeInfo("unsigned", 8),
				new int64_t(aa - (bb * (first1->type->points < 2 ? first1->type->baseStep : 8))) }});
	} else {
		_BY_TYPE1(first1->value, first1->type->type, first1->type->size,
			second1->value, second1->type->type, second1->type->size, -);
	}
	std::cout << *reinterpret_cast<int32_t*>(_interpretationStack.top().back()->value) << "\n";
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
	auto second = _interpretationStackTopWPop();
	auto first = _interpretationStackTopWPop();
	auto second1 = second.back();
	auto first1 = first.back();
	std::cout << *reinterpret_cast<int32_t*>(first1->value) << "\n";
	if (first1->type->points || second1->type->points) {
		int64_t* aa = reinterpret_cast<int64_t*>(first1->value);
		int64_t bb = *reinterpret_cast<int64_t*>(second1->value);
		*aa = bb;
		_interpretationStack.push(first);
	} else if (first1->type->type == "string" && second1->type->type == "string") {
		std::string aa = *reinterpret_cast<std::string*>(first1->value);
		std::string bb = *reinterpret_cast<std::string*>(second1->value);
		Interpretation::_interpretationStack.push(std::vector<Interpretation::VarInfo*>{
			new Interpretation::VarInfo{ new Generation::TypeInfo("string", 0),
				new std::string(aa + bb) }});
	} else {
		_BY_TYPE(first1->value, first1->type->type, first1->type->size,
			second1->value, second1->type->type, second1->type->size, =);
		_interpretationStack.top().back()->type->isReference = true;
	}
	std::cout << *reinterpret_cast<int32_t*>(_interpretationStack.top().back()->value) << "\n";
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
	auto IsReference = _interpretationStackTopWPop();
	auto type = _interpretationStackTopWPop();
	auto points = _interpretationStackTopWPop();
	auto size = _interpretationStackTopWPop();
	_localScopes.back().insert(std::pair{dynamic_cast<Generation::_paramStr*>(upCode.param)->str, 
		new Interpretation::VarInfo{ 
		new Generation::TypeInfo(*reinterpret_cast<std::string*>(type.back()->value), 
														 *reinterpret_cast<int64_t*>(size.back()->value),
														 *reinterpret_cast<int64_t*>(points.back()->value),
														 *reinterpret_cast<int64_t*>(IsReference.back()->value)),
		new char[*reinterpret_cast<int64_t*>(size.back()->value)]}});
	memset(_localScopes.back()[dynamic_cast<Generation::_paramStr*>(upCode.param)->str]->value, 0,
		*reinterpret_cast<int64_t*>(size.back()->value));
}

void Interpretation::_executeUpCode_INIT_VARIABLE_WITHOUT_CREATE  (Generation::_upCode& upCode, int64_t& index){
	return;
}

void Interpretation::_executeUpCode_JUMP_IF_FALSE                 (Generation::_upCode& upCode, int64_t& index){
	auto first = _interpretationStackTopWPop();
	bool c = reinterpret_cast<bool>(first.back()->value);
	for (auto& it : first) {
		delete it;
	}
	if (!c) {
		index = dynamic_cast<Generation::_paramInt*>(upCode.param)->in;
	}
}

void Interpretation::_executeUpCode_JUMP                          (Generation::_upCode& upCode, int64_t& index){
	index = dynamic_cast<Generation::_paramInt*>(upCode.param)->in;
}

void Interpretation::_executeUpCode_DUP                           (Generation::_upCode& upCode, int64_t& index){
	_interpretationStack.push(_interpretationStack.top());
}

void Interpretation::_executeUpCode_SWAP                          (Generation::_upCode& upCode, int64_t& index){
	auto second = _interpretationStackTopWPop();
	auto first = _interpretationStackTopWPop();
	_interpretationStack.push(second);
	_interpretationStack.push(first);
}

void Interpretation::_executeUpCode_POP                           (Generation::_upCode& upCode, int64_t& index){
	auto first = _interpretationStackTopWPop();
	for (auto& it : first) {
		delete it;
	}
}

void Interpretation::_executeUpCode_END                           (Generation::_upCode& upCode, int64_t& index){
	auto first = _interpretationStackTopWPop().back();
	index = *reinterpret_cast<int64_t*>(first->value);
	delete first;
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


