#ifndef COMMON_H
#define COMMON_H

#include <map>
#include <any>
#include <string>
#include <type_traits>
#include <boost/algorithm/string.hpp>
#include <boost/current_function.hpp>

enum class PascalTokenTypeImpl {
  // reserved words
  AND, ARRAY, BEGIN, CASE, CONST, DIV, DO, DOWNTO, ELSE, END,
  FILE, FOR, FUNCTION, GOTO, IF, IN, LABEL, MOD, NIL, NOT,
  OF, OR, PACKED, PROCEDURE, PROGRAM, RECORD, REPEAT, SET,
  THEN, TO, TYPE, UNTIL, VAR, WHILE, WITH, IDENTIFIER, INTEGER,
  REAL, STRING, ERROR, END_OF_FILE,
  // special symbols
  PLUS, MINUS, STAR, SLASH, COLON_EQUALS, DOT, COMMA, SEMICOLON,
  COLON, QUOTE, EQUALS, NOT_EQUALS, LESS_THAN, LESS_EQUALS,
  GREATER_EQUALS, GREATER_THAN, LEFT_PAREN, RIGHT_PAREN,
  LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE,
  UP_ARROW, DOT_DOT,
  // unknown type
  UNKNOWN
};

enum class PascalErrorCode {
  ALREADY_FORWARDED,
  IDENTIFIER_REDEFINED,
  IDENTIFIER_UNDEFINED,
  INCOMPATIBLE_ASSIGNMENT,
  INCOMPATIBLE_TYPES,
  INVALID_ASSIGNMENT,
  INVALID_CHARACTER,
  INVALID_CONSTANT,
  INVALID_EXPONENT,
  INVALID_EXPRESSION,
  INVALID_FIELD,
  INVALID_FRACTION,
  INVALID_IDENTIFIER_USAGE,
  INVALID_INDEX_TYPE,
  INVALID_NUMBER,
  INVALID_STATEMENT,
  INVALID_SUBRANGE_TYPE,
  INVALID_TARGET,
  INVALID_TYPE,
  INVALID_VAR_PARM,
  MIN_GT_MAX,
  MISSING_BEGIN,
  MISSING_COLON,
  MISSING_COLON_EQUALS,
  MISSING_COMMA,
  MISSING_CONSTANT,
  MISSING_DO,
  MISSING_DOT_DOT,
  MISSING_END,
  MISSING_EQUALS,
  MISSING_FOR_CONTROL,
  MISSING_IDENTIFIER,
  MISSING_LEFT_BRACKET,
  MISSING_OF,
  MISSING_PERIOD,
  MISSING_PROGRAM,
  MISSING_RIGHT_BRACKET,
  MISSING_RIGHT_PAREN,
  MISSING_SEMICOLON,
  MISSING_THEN,
  MISSING_TO_DOWNTO,
  MISSING_UNTIL,
  MISSING_VARIABLE,
  CASE_CONSTANT_REUSED,
  NOT_CONSTANT_IDENTIFIER,
  NOT_RECORD_VARIABLE,
  NOT_TYPE_IDENTIFIER,
  RANGE_INTEGER,
  RANGE_REAL,
  STACK_OVERFLOW,
  TOO_MANY_LEVELS,
  TOO_MANY_SUBSCRIPTS,
  UNEXPECTED_EOF,
  UNEXPECTED_TOKEN,
  UNIMPLEMENTED,
  UNRECOGNIZABLE,
  WRONG_NUMBER_OF_PARMS,
  IO_ERROR,
  TOO_MANY_ERRORS
};

enum class SymbolTableKeyTypeImpl {
  // Constant
  CONSTANT_VALUE,
  // Procedure of function
  ROUTINE_CODE, ROUTINE_SYMTAB, ROUTINE_ICODE,
  ROUTINE_PARMS, ROUTINE_ROUTINES,
  // Variable or record field value
  DATA_VALUE, DATA_TYPE
};

enum class ICodeKeyTypeImpl {
  LINE, ID, VALUE
};

enum class ICodeNodeTypeImpl {
  // program structures
  PROGRAM, PROCEDURE, FUNCTION,
  // statements
  COMPOUND, ASSIGN, LOOP, TEST, CALL, PARAMETERS,
  IF, SELECT, SELECT_BRANCH, SELECT_CONSTANTS, NO_OP,
  // relational operators
  EQ, NE, LT, LE, GT, GE, NOT,
  // additive operators
  ADD, SUBTRACT, OR, NEGATE,
  // multiplicative operatros
  MULTIPLY, INTEGER_DIVIDE, FLOAT_DIVIDE, MOD, AND,
  // operands
  VARIABLE, SUBSCRIPTS, FIELD,
  INTEGER_CONSTANT, REAL_CONSTANT,
  STRING_CONSTANT, BOOLEAN_CONSTANT
};

enum class VariableType {
  INTEGER, FLOAT, BOOLEAN, STRING, UNKNOWN
};

extern std::map<PascalTokenTypeImpl, std::string> reservedWordsMap;
extern std::map<std::string, PascalTokenTypeImpl> reservedWordsMapRev;
extern std::map<PascalTokenTypeImpl, std::string> specialSymbolsMap;
extern std::map<std::string, PascalTokenTypeImpl> specialSymbolsMapRev;
extern std::map<PascalTokenTypeImpl, std::string> specialWordsMap;
extern std::map<std::string, PascalTokenTypeImpl> specialWordsMapRev;

// assume no key shares a same value
template <typename T1, typename T2>
std::map<T2, T1> reverse_map(const std::map<T1, T2>& map_in) {
  std::map<T2, T1> map_out;
  for (auto it = map_in.begin(); it != map_in.end(); ++it) {
    map_out[it->second] = it->first;
  }
  return map_out;
}

std::string any_to_string(const std::any& a);

void clear_line_ending(std::string& line);

#endif // COMMON_H
