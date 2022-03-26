#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "Intermediate.h"

namespace TypeChecker
{
  using TypeSpecPtr = std::shared_ptr<TypeSpecImplBase>;
  namespace TypeChecking {
    bool isInteger(const TypeSpecPtr& type_spec);
    bool areBothInteger(const TypeSpecPtr& type_spec_a, const TypeSpecPtr& type_spec_b);
    bool isReal(const TypeSpecPtr& type_spec);
    bool isIntegerOrReal(const TypeSpecPtr& type_spec);
    bool isAtLeastOneReal(const TypeSpecPtr& type_spec_a, const TypeSpecPtr& type_spec_b);
    bool isBoolean(const TypeSpecPtr& type_spec);
    bool areBothBoolean(const TypeSpecPtr& type_spec_a, const TypeSpecPtr& type_spec_b);
    bool isChar(const TypeSpecPtr& type_spec);
  }
  namespace TypeCompatibility {
    bool areAssignmentCompatible(const TypeSpecPtr& target_type_spec, const TypeSpecPtr& value_type_spec);
    bool areComparisonCompatible(const TypeSpecPtr& type_spec_a, const TypeSpecPtr& type_spec_b);
  }
};

#endif // TYPECHECKER_H
