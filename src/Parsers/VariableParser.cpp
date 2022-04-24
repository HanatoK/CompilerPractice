#include "VariableParser.h"
#include "ExpressionParser.h"
#include "TypeChecker.h"

PascalSubparserTopDownBase::TokenTypeSet VariableParser::subscriptFieldStartSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
    PascalTokenTypeImpl::LEFT_BRACKET,
    PascalTokenTypeImpl::DOT
  };
  return s;
}

VariableParser::VariableParser(const std::shared_ptr<PascalParserTopDown> &parent) : PascalSubparserTopDownBase(
    parent) {

}

VariableParser::~VariableParser() {
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::shared_ptr<ICodeNodeImplBase> VariableParser::parse(std::shared_ptr<PascalToken> token) {
//  std::cerr << "parseVariable should be called instead of parse for VariableParser."
//  return PascalSubparserTopDownBase::parse(token);
  // lookup the identifier in the symbol table stack
  const auto name = boost::to_lower_copy(token->text());
  auto variable_id = getSymbolTableStack()->lookup(name);
  // if not found, flag the error and enter the identifier
  // as an undefined identifier with an undefined type
  if (variable_id == nullptr) {
    errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_UNDEFINED, currentParser());
    variable_id = getSymbolTableStack()->enterLocal(name);
    variable_id->setDefinition(DefinitionImpl::UNDEFINED);
    variable_id->setTypeSpec(Predefined::instance().undefinedType);
  }
  return parseVariable(token, variable_id);
}

std::shared_ptr<ICodeNodeImplBase> VariableParser::parseVariable(std::shared_ptr<PascalToken> token,
                                                                 std::shared_ptr<SymbolTableEntryImplBase> id) {
  // check how the variable is defined
  const auto definition_code = id->getDefinition();
  if (definition_code != DefinitionImpl::VARIABLE &&
      definition_code != DefinitionImpl::VALUE_PARM &&
      definition_code != DefinitionImpl::VAR_PARM) {
    errorHandler()->flag(token, PascalErrorCode::INVALID_IDENTIFIER_USAGE, currentParser());
  }
  id->appendLineNumber(token->lineNum());
  auto variable_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::VARIABLE));
  variable_node->setAttribute<ICodeKeyTypeImpl::ID>(id);
  // consume the identifier
  token = nextToken();
  // parse array subscripts or record fields
  auto variable_type = id->getTypeSpec();
  const auto start_set = subscriptFieldStartSet();
  while (start_set.contains(token->type())) {
    auto subscript_field_node = (token->type() == PascalTokenTypeImpl::LEFT_BRACKET) ?
                                parseSubscripts(variable_type) : parseField(variable_type);
    token = currentToken();
    // update the variable's type
    // the variable node adopts the subscripts or field node
    variable_type = subscript_field_node->getTypeSpec();
    variable_node->addChild(subscript_field_node);
  }
  variable_node->setTypeSpec(variable_type);
  return variable_node;
}

std::shared_ptr<ICodeNodeImplBase> VariableParser::parseSubscripts(std::shared_ptr<TypeSpecImplBase> variable_type) {
  ExpressionParser expression_parser(currentParser());
  auto subscripts_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::SUBSCRIPTS));
  std::shared_ptr<PascalToken> token;
  do {
    // consume the [ or , token
    token = nextToken();
    // the current variable is an array
    if (variable_type->form() == TypeFormImpl::ARRAY) {
      auto expression_node = expression_parser.parse(token);
      const auto expression_type = (expression_node != nullptr) ?
                                                    expression_node->getTypeSpec() :
                                                    Predefined::instance().undefinedType;
      // the subscript expression type must be assignment compatible with the array index type
      const auto index_type = variable_type->getAttribute<TypeKeyImpl::ARRAY_INDEX_TYPE>();
      using namespace TypeChecker::TypeCompatibility;
      if (!areAssignmentCompatible(index_type, expression_type)) {
        // TODO: there is a bug of incompatible type!
        errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
      }
      // the subscripts node adopts the subscript expression tree
      subscripts_node->addChild(std::move(expression_node));
      variable_type = variable_type->getAttribute<TypeKeyImpl::ARRAY_ELEMENT_TYPE>();
    } else {
      // not an array type, so too many subscripts
      errorHandler()->flag(token, PascalErrorCode::TOO_MANY_SUBSCRIPTS, currentParser());
      expression_parser.parse(token);
    }
    token = currentToken();
  } while (token->type() == PascalTokenTypeImpl::COMMA);
  // synchronize at the ] token
  token = synchronize(rightBracketSet());
  if (token->type() == PascalTokenTypeImpl::RIGHT_BRACKET) {
    // consume ]
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_RIGHT_BRACKET, currentParser());
  }
  subscripts_node->setTypeSpec(variable_type);
  return subscripts_node;
}

PascalSubparserTopDownBase::TokenTypeSet VariableParser::rightBracketSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
      PascalTokenTypeImpl::RIGHT_BRACKET,
      PascalTokenTypeImpl::EQUALS,
      PascalTokenTypeImpl::SEMICOLON
  };
  return s;
}

std::shared_ptr<ICodeNodeImplBase> VariableParser::parseField(std::shared_ptr<TypeSpecImplBase> variable_type) {
  // create field node
  auto field_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::FIELD));
  // consume . token
  auto token = nextToken();
  const auto token_type = token->type();
  const auto variable_form = variable_type->form();
  if (token_type == PascalTokenTypeImpl::IDENTIFIER && variable_form == TypeFormImpl::RECORD) {
    auto symbol_table = variable_type->getAttribute<TypeKeyImpl::RECORD_SYMTAB>();
    const auto field_name = boost::to_lower_copy(token->text());
    auto field_id = symbol_table->lookup(field_name);
    if (field_id != nullptr) {
      field_id->appendLineNumber(token->lineNum());
      variable_type = field_id->getTypeSpec();
      // set the field identifier's name
      field_node->setAttribute<ICodeKeyTypeImpl::ID>(field_id);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_FIELD, currentParser());
    }
  } else {
    errorHandler()->flag(token, PascalErrorCode::INVALID_FIELD, currentParser());
  }
  token = nextToken();
  field_node->setTypeSpec(variable_type);
  return field_node;
}
