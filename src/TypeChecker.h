#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "Intermediate.h"

class TypeChecker
{
public:
  TypeChecker();
  static bool isInteger(const std::shared_ptr<TypeSpecImplBase>& type_spec);
};

#endif // TYPECHECKER_H
