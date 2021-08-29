#ifndef COMMON_H
#define COMMON_H

#include <map>
#include <any>
#include <string>
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

enum class SymbolTableKeyTypeImpl {
  // Constant
  CONSTANT_VALUE,
  // Procedure of function
  ROUTINE_CODE, ROUTINE_SYMTAB, ROUTINE_ICODE,
  ROUTINE_PARMS, ROUTINE_ROUTINES,
  // Variable or record field value
  DATA_VALUE
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

#endif // COMMON_H
