#include "BlockParser.h"
#include "StatementParser.h"
#include "DeclarationsParser.h"

BlockParser::BlockParser(const std::shared_ptr<PascalParserTopDown>& parent)
    : PascalSubparserTopDownBase(parent) {}

/* In pascal, the statements are followed by variable declarations.
 * An example of a pascal function:
=============================================
function max(num1, num2: integer): integer;

var
   (* local variable declaration *)
   result: integer;

begin
   if (num1 > num2) then
      result := num1

   else
      result := num2;
   max := result;
end;
=============================================
 */
std::shared_ptr<ICodeNodeImplBase> BlockParser::parse(std::shared_ptr<PascalToken> token,
                   std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  DeclarationsParser declarations_parser(currentParser());
  StatementParser statement_parser(currentParser());
  // parse any declarations
  declarations_parser.parse(token, parent_id);
  token = synchronize(StatementParser::statementStartSet());
  const auto token_type = token->type();
  std::shared_ptr<ICodeNodeImplBase> root_node = nullptr;
  if (token_type == PascalTokenTypeImpl::BEGIN) {
    // TODO: in listing 11-9 parent_id is missing, check it.
    root_node = statement_parser.parse(token, parent_id);
  } else {
    // missing BEGIN: attempt to parse anyway if possible
    errorHandler()->flag(token, PascalErrorCode::MISSING_BEGIN, currentParser());
    if (StatementParser::statementStartSet().contains(token_type)) {
      root_node = createICodeNode(ICodeNodeTypeImpl::COMPOUND);
      statement_parser.parseList(token, root_node, PascalTokenTypeImpl::END, PascalErrorCode::MISSING_END);
    }
  }
  return root_node;
}
