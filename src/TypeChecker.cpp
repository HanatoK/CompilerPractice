#include "TypeChecker.h"
#include "Predefined.h"

TypeChecker::TypeChecker()
{

}

bool TypeChecker::isInteger(const std::shared_ptr<TypeSpecImplBase>& type_spec)
{
  return (type_spec != nullptr) && (type_spec->baseType() == Predefined::instance().integerType);
}
