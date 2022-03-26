#include "TypeChecker.h"
#include "Predefined.h"

namespace TypeChecker::TypeChecking {
  bool isInteger(const TypeSpecPtr& type_spec)
  {
    return (type_spec != nullptr) && (type_spec->baseType() == Predefined::instance().integerType);
  }

  bool areBothInteger(const TypeSpecPtr& type_spec_a,
                                   const TypeSpecPtr& type_spec_b)
  {
    return isInteger(type_spec_a) && isInteger(type_spec_b);
  }

  bool isReal(const TypeSpecPtr& type_spec)
  {
    return (type_spec != nullptr) && (type_spec->baseType() == Predefined::instance().realType);
  }

  bool isIntegerOrReal(const TypeSpecPtr& type_spec)
  {
    return isInteger(type_spec) || isReal(type_spec);
  }

  bool isAtLeastOneReal(const TypeSpecPtr& type_spec_a,
                                     const TypeSpecPtr& type_spec_b)
  {
    return (isReal(type_spec_a) && isReal(type_spec_b)) ||
           (isReal(type_spec_a) && isInteger(type_spec_b)) ||
        (isInteger(type_spec_a) && isReal(type_spec_b));
  }

  bool isBoolean(const TypeSpecPtr& type_spec)
  {
    return (type_spec != nullptr) && (type_spec->baseType() == Predefined::instance().booleanType);
  }

  bool areBothBoolean(const TypeSpecPtr& type_spec_a,
                                   const TypeSpecPtr& type_spec_b)
  {
    return isBoolean(type_spec_a) && isBoolean(type_spec_b);
  }

  bool isChar(const TypeSpecPtr& type_spec)
  {
    return (type_spec != nullptr) && (type_spec->baseType() == Predefined::instance().charType);
  }
}

namespace TypeChecker::TypeCompatibility {
  using namespace TypeChecking;
  bool areAssignmentCompatible(const TypeSpecPtr& target_type_spec, const TypeSpecPtr& value_type_spec)
  {
    if (target_type_spec == nullptr || value_type_spec == nullptr) {
      return false;
    }
    const auto base_target_type_spec = target_type_spec->baseType();
    const auto base_value_type_spec = value_type_spec->baseType();
    bool compatible = false;
    if (base_target_type_spec == base_value_type_spec) {
      compatible = true;
    } else if (isReal(base_target_type_spec) && isInteger(base_value_type_spec)) {
      // allow real:=integer
      compatible = true;
    } else {
      if (base_target_type_spec->isPascalString() && base_value_type_spec->isPascalString()){
        compatible = true;
      }
    }
    return compatible;
  }

  bool areComparisonCompatible(const TypeSpecPtr& type_spec_a, const TypeSpecPtr& type_spec_b)
  {
    if (type_spec_a == nullptr || type_spec_b == nullptr) {
      return false;
    }
    const auto a_base_type_spec = type_spec_a->baseType();
    const auto b_base_type_spec = type_spec_b->baseType();
    bool compatible = false;
    const auto form = a_base_type_spec->form();
    if (a_base_type_spec == b_base_type_spec &&
        (form == TypeFormImpl::SCALAR || form == TypeFormImpl::ENUMERATION)) {
      compatible = true;
    } else if (isAtLeastOneReal(a_base_type_spec, b_base_type_spec)) {
      compatible = true;
    } else {
      if (a_base_type_spec->isPascalString() && b_base_type_spec->isPascalString()){
        compatible = true;
      }
    }
    return compatible;
  }

}
