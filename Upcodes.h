#include "EnumToString.h"
#include "Lex.h"


#if PRINT_GEN == 0
#undef  GENERATE_ENUM_STRINGS
#endif

#if ( !defined(UPCODES_H) || defined(GENERATE_ENUM_STRINGS) )

#if (!defined(GENERATE_ENUM_STRINGS))
#define UPCODES_H
#endif
BEGIN_ENUM(UPCODES) {
    DECL_ENUM_ELEMENT(CREATE_STRUCT),
    DECL_ENUM_ELEMENT(CREATE_FIELD_STRUCT),

    DECL_ENUM_ELEMENT(LOAD_CONST_INT),
    DECL_ENUM_ELEMENT(LOAD_CONST_CHAR),
    DECL_ENUM_ELEMENT(LOAD_CONST_FLOAT),
    DECL_ENUM_ELEMENT(LOAD_CONST_STRING),
    DECL_ENUM_ELEMENT(LOAD_STRING),
    DECL_ENUM_ELEMENT(LOAD_NAME),

    DECL_ENUM_ELEMENT(CAST),

    DECL_ENUM_ELEMENT(CALL_FUNCTION),

    DECL_ENUM_ELEMENT(SUFFIX_INCREMENT),
    DECL_ENUM_ELEMENT(SUFFIX_DECREMENT),

    DECL_ENUM_ELEMENT(PREFFIX_INCREMENT),
    DECL_ENUM_ELEMENT(PREFFIX_DECREMENT),

    DECL_ENUM_ELEMENT(POINT),
    DECL_ENUM_ELEMENT(INDEX_OPERATOR),

    DECL_ENUM_ELEMENT(UNARY_ADD),
    DECL_ENUM_ELEMENT(UNARY_SUBSTRACTION),

    DECL_ENUM_ELEMENT(BINARY_ADD),
    DECL_ENUM_ELEMENT(BINARY_SUBSTRACTION),

    DECL_ENUM_ELEMENT(LOGIC_NOT),
    DECL_ENUM_ELEMENT(BIT_NOT),

    DECL_ENUM_ELEMENT(UNARY_DEREFERENCING),
    DECL_ENUM_ELEMENT(UNARY_GET_ADDRESS),

    DECL_ENUM_ELEMENT(POWER),

    DECL_ENUM_ELEMENT(MILTIPLY),
    DECL_ENUM_ELEMENT(DIVISION),
    DECL_ENUM_ELEMENT(MOD),

    DECL_ENUM_ELEMENT(COMPARISION_GREATER),
    DECL_ENUM_ELEMENT(COMPARISION_LESS),
    DECL_ENUM_ELEMENT(COMPARISION_GREATER_EQUAL),
    DECL_ENUM_ELEMENT(COMPARISION_LESS_EQUAL),
    DECL_ENUM_ELEMENT(COMPARISION_EQUAL),
    DECL_ENUM_ELEMENT(COMPARISION_NOT_EQUAL),

    DECL_ENUM_ELEMENT(BIT_AND),
    DECL_ENUM_ELEMENT(BIT_OR),

    DECL_ENUM_ELEMENT(LOGIC_AND),
    DECL_ENUM_ELEMENT(LOGIC_OR),

    DECL_ENUM_ELEMENT(ADD_ASSIGN),
    DECL_ENUM_ELEMENT(SUBSTRACTION_ASSIGN),
    DECL_ENUM_ELEMENT(MILTIPLY_ASSIGN),
    DECL_ENUM_ELEMENT(DIVISION_ASSIGN),
    DECL_ENUM_ELEMENT(MOD_ASSIGN),
    DECL_ENUM_ELEMENT(BIT_AND_ASSIGN),
    DECL_ENUM_ELEMENT(BIT_OR_ASSIGN),
    DECL_ENUM_ELEMENT(POWER_ASSIGN),
    DECL_ENUM_ELEMENT(ASSIGN),
    
    DECL_ENUM_ELEMENT(COMMA),

    DECL_ENUM_ELEMENT(PRINT),
    DECL_ENUM_ELEMENT(READ),

    DECL_ENUM_ELEMENT(ASSIGN_BY_ADDRESS),

    DECL_ENUM_ELEMENT(INIT_VARIABLE),
    DECL_ENUM_ELEMENT(INIT_VARIABLE_WITHOUT_CREATE),

    DECL_ENUM_ELEMENT(JUMP_IF_FALSE),
    DECL_ENUM_ELEMENT(JUMP),

    DECL_ENUM_ELEMENT(DUP),
    DECL_ENUM_ELEMENT(SWAP),
    DECL_ENUM_ELEMENT(POP),
    DECL_ENUM_ELEMENT(END),

    DECL_ENUM_ELEMENT(START_PROGRAM),
    DECL_ENUM_ELEMENT(NOT_PRE_INIT_BLOCK),
    DECL_ENUM_ELEMENT(END_NOT_PRE_INIT_BLOCK)

}
END_ENUM(UPCODES)

#endif // (!defined(DAYS_H) || defined(GENERATE_ENUM_STRINGS))