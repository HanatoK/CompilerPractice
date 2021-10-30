#include "Predefined.h"
#include <stdexcept>

Predefined& Predefined::instance(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack)
{
  static Predefined s;
  static bool firsttime = true;
  if (firsttime) {
    s.initialize(symbol_table_stack);
    _instance = &s;
    firsttime = false;
  }
  return s;
}

Predefined& Predefined::instance()
{
  if (_instance == nullptr) {
    throw std::runtime_error("Uninitialized singleton of Predefined!");
  }
  return *_instance;
}

Predefined::Predefined()
{
}

void Predefined::initialize(std::shared_ptr<SymbolTableStackImplBase> &symbol_table_stack) {
  integerType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  realType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  booleanType = std::make_unique<TypeSpecImpl>(TypeFormImpl::ENUMERATION);
  charType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  undefinedType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  integerId = symbol_table_stack->enterLocal("integer");
  realId = symbol_table_stack->enterLocal("real");
  booleanId = symbol_table_stack->enterLocal("boolean");
  charId = symbol_table_stack->enterLocal("char");
  falseId = symbol_table_stack->enterLocal("false");
  trueId= symbol_table_stack->enterLocal("true");
  // initialize predefined types
  integerId->setDefinition(DefinitionImpl::TYPE);
  realId->setDefinition(DefinitionImpl::TYPE);
  booleanId->setDefinition(DefinitionImpl::TYPE);
  charId->setDefinition(DefinitionImpl::TYPE);
  falseId->setDefinition(DefinitionImpl::ENUMERATION_CONSTANT);
  trueId->setDefinition(DefinitionImpl::ENUMERATION_CONSTANT);
  integerId->setTypeSpec(integerType);
  integerType->setIdentifier(integerId.get());
  realId->setTypeSpec(realType);
  realType->setIdentifier(realId.get());
  booleanId->setTypeSpec(booleanType);
  booleanType->setIdentifier(booleanId.get());
  charId->setTypeSpec(charType);
  charType->setIdentifier(charId.get());
  // initialize predefined constants
  falseId->setTypeSpec(booleanType);
  falseId->setAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE, 0ll);
  trueId->setTypeSpec(booleanType);
  trueId->setAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE, 1ll);
  std::vector<decltype (falseId)> constants;
  constants.push_back(falseId);
  constants.push_back(trueId);
  booleanType->setAttribute(TypeKeyImpl::ENUMERATION_CONSTANTS, constants);
}
