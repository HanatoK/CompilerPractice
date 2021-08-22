#ifndef COMMON_H
#define COMMON_H

#include <map>

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

// assume no key shares a same value
template <typename T1, typename T2>
std::map<T2, T1> reverse_map(const std::map<T1, T2>& map_in) {
  std::map<T2, T1> map_out;
  for (auto it = map_in.begin(); it != map_in.end(); ++it) {
    map_out[it->second] = it->first;
  }
  return map_out;
}

#endif // COMMON_H
