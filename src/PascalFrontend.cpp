#include "PascalFrontend.h"
#include "Common.h"
#include "IntermediateImpl.h"
#include "Parsers/StatementParser.h"
#include "Parsers/BlockParser.h"

//#include <QCoreApplication>
#include <cctype>
#include <chrono>
#include <fmt/format.h>
#include <iostream>
#include <ratio>
#include <set>

std::map<PascalTokenTypeImpl, std::string> initReservedWordsMap() {
  std::map<PascalTokenTypeImpl, std::string> reservedWordsMap;
  // reserved words
  reservedWordsMap[PascalTokenTypeImpl::AND] = std::string("and");
  reservedWordsMap[PascalTokenTypeImpl::ARRAY] = std::string("array");
  reservedWordsMap[PascalTokenTypeImpl::BEGIN] = std::string("begin");
  reservedWordsMap[PascalTokenTypeImpl::CASE] = std::string("case");
  reservedWordsMap[PascalTokenTypeImpl::CONST] = std::string("const");
  reservedWordsMap[PascalTokenTypeImpl::DIV] = std::string("div");
  reservedWordsMap[PascalTokenTypeImpl::DO] = std::string("do");
  reservedWordsMap[PascalTokenTypeImpl::DOWNTO] = std::string("downto");
  reservedWordsMap[PascalTokenTypeImpl::ELSE] = std::string("else");
  reservedWordsMap[PascalTokenTypeImpl::END] = std::string("end");
  reservedWordsMap[PascalTokenTypeImpl::FILE] = std::string("file");
  reservedWordsMap[PascalTokenTypeImpl::FOR] = std::string("for");
  reservedWordsMap[PascalTokenTypeImpl::FUNCTION] = std::string("function");
  reservedWordsMap[PascalTokenTypeImpl::GOTO] = std::string("goto");
  reservedWordsMap[PascalTokenTypeImpl::IF] = std::string("if");
  reservedWordsMap[PascalTokenTypeImpl::IN] = std::string("in");
  reservedWordsMap[PascalTokenTypeImpl::LABEL] = std::string("label");
  reservedWordsMap[PascalTokenTypeImpl::MOD] = std::string("mod");
  reservedWordsMap[PascalTokenTypeImpl::NIL] = std::string("nil");
  reservedWordsMap[PascalTokenTypeImpl::NOT] = std::string("not");
  reservedWordsMap[PascalTokenTypeImpl::OF] = std::string("of");
  reservedWordsMap[PascalTokenTypeImpl::OR] = std::string("or");
  reservedWordsMap[PascalTokenTypeImpl::PACKED] = std::string("packed");
  reservedWordsMap[PascalTokenTypeImpl::PROCEDURE] = std::string("procedure");
  reservedWordsMap[PascalTokenTypeImpl::PROGRAM] = std::string("program");
  reservedWordsMap[PascalTokenTypeImpl::RECORD] = std::string("record");
  reservedWordsMap[PascalTokenTypeImpl::REPEAT] = std::string("repeat");
  reservedWordsMap[PascalTokenTypeImpl::SET] = std::string("set");
  reservedWordsMap[PascalTokenTypeImpl::THEN] = std::string("then");
  reservedWordsMap[PascalTokenTypeImpl::TO] = std::string("to");
  reservedWordsMap[PascalTokenTypeImpl::TYPE] = std::string("type");
  reservedWordsMap[PascalTokenTypeImpl::UNTIL] = std::string("until");
  reservedWordsMap[PascalTokenTypeImpl::VAR] = std::string("var");
  reservedWordsMap[PascalTokenTypeImpl::WHILE] = std::string("while");
  reservedWordsMap[PascalTokenTypeImpl::WITH] = std::string("with");
  return reservedWordsMap;
}

std::map<PascalTokenTypeImpl, std::string> initSpecialSymbolsMap() {
  std::map<PascalTokenTypeImpl, std::string> specialSymbolsMap;
  // special symbols
  specialSymbolsMap[PascalTokenTypeImpl::PLUS] = std::string("+");
  specialSymbolsMap[PascalTokenTypeImpl::MINUS] = std::string("-");
  specialSymbolsMap[PascalTokenTypeImpl::STAR] = std::string("*");
  specialSymbolsMap[PascalTokenTypeImpl::SLASH] = std::string("/");
  specialSymbolsMap[PascalTokenTypeImpl::COLON_EQUALS] = std::string(":=");
  specialSymbolsMap[PascalTokenTypeImpl::DOT] = std::string(".");
  specialSymbolsMap[PascalTokenTypeImpl::COMMA] = std::string(",");
  specialSymbolsMap[PascalTokenTypeImpl::SEMICOLON] = std::string(";");
  specialSymbolsMap[PascalTokenTypeImpl::COLON] = std::string(":");
  specialSymbolsMap[PascalTokenTypeImpl::QUOTE] = std::string("'");
  specialSymbolsMap[PascalTokenTypeImpl::EQUALS] = std::string("=");
  specialSymbolsMap[PascalTokenTypeImpl::NOT_EQUALS] = std::string("<>");
  specialSymbolsMap[PascalTokenTypeImpl::LESS_THAN] = std::string("<");
  specialSymbolsMap[PascalTokenTypeImpl::LESS_EQUALS] = std::string("<=");
  specialSymbolsMap[PascalTokenTypeImpl::GREATER_EQUALS] = std::string(">=");
  specialSymbolsMap[PascalTokenTypeImpl::GREATER_THAN] = std::string(">");
  specialSymbolsMap[PascalTokenTypeImpl::LEFT_PAREN] = std::string("(");
  specialSymbolsMap[PascalTokenTypeImpl::RIGHT_PAREN] = std::string(")");
  specialSymbolsMap[PascalTokenTypeImpl::LEFT_BRACKET] = std::string("[");
  specialSymbolsMap[PascalTokenTypeImpl::RIGHT_BRACKET] = std::string("]");
  specialSymbolsMap[PascalTokenTypeImpl::LEFT_BRACE] = std::string("{");
  specialSymbolsMap[PascalTokenTypeImpl::RIGHT_BRACE] = std::string("}");
  specialSymbolsMap[PascalTokenTypeImpl::UP_ARROW] = std::string("^");
  specialSymbolsMap[PascalTokenTypeImpl::DOT_DOT] = std::string("..");
  return specialSymbolsMap;
}

std::map<PascalTokenTypeImpl, std::string> initSpecialWordsMap() {
  std::map<PascalTokenTypeImpl, std::string> specialWordsMap;
  // split this
  specialWordsMap[PascalTokenTypeImpl::IDENTIFIER] = std::string("identifier");
  specialWordsMap[PascalTokenTypeImpl::INTEGER] = std::string("integer");
  specialWordsMap[PascalTokenTypeImpl::REAL] = std::string("real");
  specialWordsMap[PascalTokenTypeImpl::STRING] = std::string("string");
  specialWordsMap[PascalTokenTypeImpl::ERROR] = std::string("error");
  specialWordsMap[PascalTokenTypeImpl::END_OF_FILE] =
      std::string("end_of_file");
  return specialWordsMap;
}

std::map<PascalTokenTypeImpl, std::string> reservedWordsMap =
    initReservedWordsMap();
std::map<PascalTokenTypeImpl, std::string> specialSymbolsMap =
    initSpecialSymbolsMap();
std::map<PascalTokenTypeImpl, std::string> specialWordsMap =
    initSpecialWordsMap();
std::map<std::string, PascalTokenTypeImpl> reservedWordsMapRev =
    reverse_map(reservedWordsMap);
std::map<std::string, PascalTokenTypeImpl> specialSymbolsMapRev =
    reverse_map(specialSymbolsMap);
std::map<std::string, PascalTokenTypeImpl> specialWordsMapRev =
    reverse_map(specialWordsMap);

PascalParserTopDown::PascalParserTopDown(std::shared_ptr<PascalScanner> scanner)
    : Parser(scanner), mErrorHandler(nullptr), mRoutineId(nullptr) {
  mErrorHandler = new PascalErrorHandler();
}

PascalParserTopDown::~PascalParserTopDown() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
  if (mErrorHandler != nullptr) {
    delete mErrorHandler;
    mErrorHandler = nullptr;
  }
}

void PascalParserTopDown::parse() {
  const auto start_time = std::chrono::high_resolution_clock::now();
  std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> intermediate_code = createICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>();
  // create a dummy program identifier symbol table entry
  mRoutineId = mSymbolTableStack->enterLocal(boost::algorithm::to_lower_copy(std::string{"DummyProgramName"}));
  mRoutineId->setDefinition(DefinitionImpl::PROGRAM);
  mSymbolTableStack->setProgramId(mRoutineId.get());
  // push a new symbol table into the stack and set the routine's symbol table and iCode
  mRoutineId->setAttribute(SymbolTableKeyTypeImpl::ROUTINE_SYMTAB, mSymbolTableStack->push());
  mRoutineId->setAttribute(SymbolTableKeyTypeImpl::ROUTINE_CODE, intermediate_code);
  // TODO: block parser
  BlockParser block_parser(*this);
  auto token = nextToken();
  // parse a block
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> root_node = block_parser.parse(token, mRoutineId);
  intermediate_code->setRoot(std::move(root_node));
  mSymbolTableStack->pop();
  // look for the final .
  token = currentToken();
  if (token->type() != PascalTokenTypeImpl::DOT) {
    mErrorHandler->flag(token, PascalErrorCode::MISSING_PERIOD, this);
  }
  token = currentToken();
//  while (!token->isEof()) {
//    if (token->type() != PascalTokenTypeImpl::ERROR) {
//      if (token->type() == PascalTokenTypeImpl::BEGIN) {
//        StatementParser statement_parser(*this);
//        root_node = statement_parser.parse(token);
//        token = currentToken();
//      } else {
//        mErrorHandler->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, this);
//      }
//      if (token->type() != PascalTokenTypeImpl::DOT) {
//        mErrorHandler->flag(token, PascalErrorCode::MISSING_PERIOD, this);
//      }
//      token = currentToken();
//      if (root_node != nullptr) {
//        mICode->setRoot(std::move(root_node));
//      }
//      //      if (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
//      //        const std::string name =
//      //        boost::algorithm::to_lower_copy(token->text()); auto
//      //        symbol_table_stack =
//      //            dynamic_cast<SymbolTableStackImpl
//      //            *>(mSymbolTableStack.get());
//      //        auto entry = symbol_table_stack->lookup(name);
//      //        if (entry == nullptr) {
//      //          entry = symbol_table_stack->enterLocal(name);
//      //        }
//      //        entry->appendLineNumber(token->lineNum());
//      //      }
//      pascalTokenMessage(token->lineNum(), token->position(), token->type(),
//                         token->text(), token->value());
//    } else {
//      mErrorHandler->flag(token, std::any_cast<PascalErrorCode>(token->value()),
//                          this);
//    }
//    token = nextToken();
//  }
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time_sec = end_time - start_time;
  parserSummary(token->lineNum(), errorCount(), elapsed_time_sec.count());
}

int PascalParserTopDown::errorCount() const {
  return mErrorHandler->errorCount();
}

std::shared_ptr<PascalToken> PascalParserTopDown::synchronize(
    const std::set<PascalTokenTypeImpl> &sync_set) {
  auto token = currentToken();
  // if the current token is not in the synchronization set,
  // then it is unexpected and the parser must recover
  auto search = sync_set.find(token->type());
  if (search == sync_set.end()) {
    // flag the unexpected token
    mErrorHandler->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, this);
    // recover by skipping tokens that are not in the synchronization set
    do {
      token = nextToken();
      search = sync_set.find(token->type());
    } while (!token->isEof() && search == sync_set.end());
  }
  return token;
}

PascalScanner::PascalScanner() : Scanner(nullptr) {}

PascalScanner::PascalScanner(std::shared_ptr<Source> source)
    : Scanner(source) {}

PascalScanner::~PascalScanner() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

std::shared_ptr<PascalToken> PascalScanner::extractToken() {
  skipWhiteSpace();
  unique_ptr<PascalToken> token = nullptr;
  auto current_char = currentChar();
  // Construct the next token
  // The current character determines the token type.
  if (current_char == EOF) {
    token = std::make_unique<PascalEofToken>(mSource);
    std::cerr << "Reach EOF\n";
  } else if (std::isalpha(current_char)) {
    token = std::make_unique<PascalWordToken>(mSource);
  } else if (std::isdigit(current_char)) {
    token = std::make_unique<PascalNumberToken>(mSource);
  } else if (current_char == '\'') {
    token = std::make_unique<PascalStringToken>(mSource);
  } else if (specialSymbolsMapRev.find(std::string{current_char}) !=
             specialSymbolsMapRev.end()) {
    token = std::make_unique<PascalSpecialSymbolToken>(mSource);
  } else {
    token = std::make_unique<PascalErrorToken>(
        mSource, PascalErrorCode::INVALID_CHARACTER, std::string{current_char});
    nextChar();
  }
  return std::move(token);
}

void PascalScanner::skipWhiteSpace() {
  auto current_char = currentChar();
  // isWhiteSpace in Java also checks tabulation
  while (std::isspace(current_char) || current_char == '{') {
    // consume the comment characters
    if (current_char == '{') {
      do {
        current_char = nextChar();
      } while ((current_char != '}') && (current_char != EOF));
      if (current_char == '}') {
        current_char = nextChar();
      }
    } else { // not a comment
      current_char = nextChar();
    }
  }
}

std::unique_ptr<PascalParserTopDown>
createPascalParser(const std::string &language, const std::string &type,
                   std::shared_ptr<Source> source) {
  if (boost::iequals(language, "Pascal") && boost::iequals(type, "top-down")) {
    std::unique_ptr<PascalScanner> scanner =
        std::make_unique<PascalScanner>(source);
    return std::make_unique<PascalParserTopDown>(std::move(scanner));
  } else if (boost::iequals(language, "Pascal") == false) {
    std::cerr << "Invalid language: " << language.c_str();
    return nullptr;
  } else {
    std::cerr << "Invalid type: " << type.c_str();
    return nullptr;
  }
}

PascalErrorHandler::PascalErrorHandler() { mErrorCount = 0; }

PascalErrorHandler::~PascalErrorHandler() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

void PascalErrorHandler::flag(const std::shared_ptr<PascalToken> &token,
                              const PascalErrorCode errorCode,
                              const PascalParserTopDown *parser) {
  parser->syntaxErrorMessage(token->lineNum(), token->position(), token->text(),
                             errorMessageMap[errorCode]);
  if (++mErrorCount > maxError) {
    abortTranslation(PascalErrorCode::TOO_MANY_ERRORS, parser);
  }
}

void PascalErrorHandler::abortTranslation(const PascalErrorCode errorCode,
                                          const PascalParserTopDown *parser) {
  const std::string fatalText = "FATAL ERROR: " + errorMessageMap[errorCode];
  const PascalParserTopDown *pascalParser =
      dynamic_cast<const PascalParserTopDown *>(parser);
  pascalParser->syntaxErrorMessage(0, 0, "", fatalText);
  std::exit(int(errorCode));
}

int PascalErrorHandler::errorCount() const { return mErrorCount; }

std::map<PascalErrorCode, std::string> initErrorMessageMap() {
  std::map<PascalErrorCode, std::string> errorMessageMap;
  errorMessageMap[PascalErrorCode::ALREADY_FORWARDED] =
      "Already specified in FORWARD";
  errorMessageMap[PascalErrorCode::IDENTIFIER_REDEFINED] =
      "Redefined identifier";
  errorMessageMap[PascalErrorCode::IDENTIFIER_UNDEFINED] =
      "Undefined identifier";
  errorMessageMap[PascalErrorCode::INCOMPATIBLE_ASSIGNMENT] =
      "Incompatible assignment";
  errorMessageMap[PascalErrorCode::INCOMPATIBLE_TYPES] = "Incompatible types";
  errorMessageMap[PascalErrorCode::INVALID_ASSIGNMENT] =
      "Invalid assignment statement";
  errorMessageMap[PascalErrorCode::INVALID_CHARACTER] = "Invalid character";
  errorMessageMap[PascalErrorCode::INVALID_CONSTANT] = "Invalid constant";
  errorMessageMap[PascalErrorCode::INVALID_EXPONENT] = "Invalid exponent";
  errorMessageMap[PascalErrorCode::INVALID_EXPRESSION] = "Invalid expression";
  errorMessageMap[PascalErrorCode::INVALID_FIELD] = "Invalid field";
  errorMessageMap[PascalErrorCode::INVALID_FRACTION] = "Invalid fraction";
  errorMessageMap[PascalErrorCode::INVALID_IDENTIFIER_USAGE] =
      "Invalid identifier usage";
  errorMessageMap[PascalErrorCode::INVALID_INDEX_TYPE] = "Invalid index type";
  errorMessageMap[PascalErrorCode::INVALID_NUMBER] = "Invalid number";
  errorMessageMap[PascalErrorCode::INVALID_STATEMENT] = "Invalid statement";
  errorMessageMap[PascalErrorCode::INVALID_SUBRANGE_TYPE] =
      "Invalid subrange type";
  errorMessageMap[PascalErrorCode::INVALID_TARGET] =
      "Invalid assignment target";
  errorMessageMap[PascalErrorCode::INVALID_TYPE] = "Invalid type";
  errorMessageMap[PascalErrorCode::INVALID_VAR_PARM] = "Invalid VAR parameter";
  errorMessageMap[PascalErrorCode::MIN_GT_MAX] =
      "Min limit greater than max limit";
  errorMessageMap[PascalErrorCode::MISSING_BEGIN] = "Missing BEGIN";
  errorMessageMap[PascalErrorCode::MISSING_COLON] = "Missing :";
  errorMessageMap[PascalErrorCode::MISSING_COLON_EQUALS] = "Missing :=";
  errorMessageMap[PascalErrorCode::MISSING_COMMA] = "Missing ,";
  errorMessageMap[PascalErrorCode::MISSING_CONSTANT] = "Missing constant";
  errorMessageMap[PascalErrorCode::MISSING_DO] = "Missing DO";
  errorMessageMap[PascalErrorCode::MISSING_DOT_DOT] = "Missing ..";
  errorMessageMap[PascalErrorCode::MISSING_END] = "Missing END";
  errorMessageMap[PascalErrorCode::MISSING_EQUALS] = "Missing =";
  errorMessageMap[PascalErrorCode::MISSING_FOR_CONTROL] =
      "Invalid FOR control variable";
  errorMessageMap[PascalErrorCode::MISSING_IDENTIFIER] = "Missing identifier";
  errorMessageMap[PascalErrorCode::MISSING_LEFT_BRACKET] = "Missing [";
  errorMessageMap[PascalErrorCode::MISSING_OF] = "Missing OF";
  errorMessageMap[PascalErrorCode::MISSING_PERIOD] = "Missing .";
  errorMessageMap[PascalErrorCode::MISSING_PROGRAM] = "Missing PROGRAM";
  errorMessageMap[PascalErrorCode::MISSING_RIGHT_BRACKET] = "Missing ]";
  errorMessageMap[PascalErrorCode::MISSING_RIGHT_PAREN] = "Missing )";
  errorMessageMap[PascalErrorCode::MISSING_SEMICOLON] = "Missing ;";
  errorMessageMap[PascalErrorCode::MISSING_THEN] = "Missing THEN";
  errorMessageMap[PascalErrorCode::MISSING_TO_DOWNTO] = "Missing TO or DOWNTO";
  errorMessageMap[PascalErrorCode::MISSING_UNTIL] = "Missing UNTIL";
  errorMessageMap[PascalErrorCode::MISSING_VARIABLE] = "Missing variable";
  errorMessageMap[PascalErrorCode::CASE_CONSTANT_REUSED] =
      "CASE constant reused";
  errorMessageMap[PascalErrorCode::NOT_CONSTANT_IDENTIFIER] =
      "Not a constant identifier";
  errorMessageMap[PascalErrorCode::NOT_RECORD_VARIABLE] =
      "Not a record variable";
  errorMessageMap[PascalErrorCode::NOT_TYPE_IDENTIFIER] =
      "Not a type identifier";
  errorMessageMap[PascalErrorCode::RANGE_INTEGER] =
      "Integer literal out of range";
  errorMessageMap[PascalErrorCode::RANGE_REAL] = "Real literal out of range";
  errorMessageMap[PascalErrorCode::STACK_OVERFLOW] = "Stack overflow";
  errorMessageMap[PascalErrorCode::TOO_MANY_LEVELS] = "Nesting level too deep";
  errorMessageMap[PascalErrorCode::TOO_MANY_SUBSCRIPTS] = "Too many subscripts";
  errorMessageMap[PascalErrorCode::UNEXPECTED_EOF] = "Unexpected end of file";
  errorMessageMap[PascalErrorCode::UNEXPECTED_TOKEN] = "Unexpected token";
  errorMessageMap[PascalErrorCode::UNIMPLEMENTED] = "Unimplemented feature";
  errorMessageMap[PascalErrorCode::UNRECOGNIZABLE] = "Unrecognizable input";
  errorMessageMap[PascalErrorCode::WRONG_NUMBER_OF_PARMS] =
      "Wrong number of actual parameters";
  errorMessageMap[PascalErrorCode::IO_ERROR] = "Object I/O error";
  errorMessageMap[PascalErrorCode::TOO_MANY_ERRORS] = "Too many syntax errors";
  return errorMessageMap;
}

std::map<PascalErrorCode, std::string> PascalErrorHandler::errorMessageMap =
    initErrorMessageMap();

std::string typeToStr(const PascalTokenTypeImpl &tokenType, bool *ok) {
  const auto s1 = reservedWordsMap.find(tokenType);
  if (s1 != reservedWordsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s1->second;
  }
  const auto s2 = specialSymbolsMap.find(tokenType);
  if (s2 != specialSymbolsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s2->second;
  }
  const auto s3 = specialSymbolsMap.find(tokenType);
  if (s3 != specialSymbolsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s3->second;
  }
  if (ok != nullptr) {
    *ok = false;
  }
  return "UNKNOWN";
}

PascalErrorToken::PascalErrorToken() : PascalToken() {
  mType = PascalTokenTypeImpl::ERROR;
}

PascalErrorToken::PascalErrorToken(std::shared_ptr<Source> source,
                                   const PascalErrorCode errorCode,
                                   const std::string &tokenText)
    : PascalToken(source, false) {
  this->mType = PascalTokenTypeImpl::ERROR;
  this->mValue = errorCode;
  this->mText = tokenText;
  PascalErrorToken::extract();
}

unique_ptr<PascalToken> PascalErrorToken::clone() const {
  return std::make_unique<PascalErrorToken>(*this);
}

void PascalErrorToken::extract() {}

PascalWordToken::PascalWordToken(std::shared_ptr<Source> source)
    : PascalToken(source, false) {
  PascalWordToken::extract();
  mValue.reset();
}

unique_ptr<PascalToken> PascalWordToken::clone() const {
  return std::make_unique<PascalWordToken>(*this);
}

void PascalWordToken::extract() {
  std::string s;
  auto current_char = currentChar();
  while (std::isalnum(current_char)) {
    s += current_char;
    current_char = nextChar();
  }
  mText = s;
  const auto lower_str = boost::algorithm::to_lower_copy(mText);
  if (reservedWordsMapRev.find(lower_str) != reservedWordsMapRev.end()) {
    mType = reservedWordsMapRev[lower_str];
  } else {
    mType = PascalTokenTypeImpl::IDENTIFIER;
  }
}

PascalStringToken::PascalStringToken(std::shared_ptr<Source> source)
    : PascalToken(source, false) {
  PascalStringToken::extract();
}

unique_ptr<PascalToken> PascalStringToken::clone() const {
  return std::make_unique<PascalStringToken>(*this);
}

void PascalStringToken::extract() {
  std::string text, value;
  auto current_char = nextChar();
  text += '\'';
  do {
    // replace any whitespace character with a blank (why?)
    if (std::isspace(current_char)) {
      current_char = ' ';
    }
    if (current_char != '\'' && current_char != EOF) {
      text += current_char;
      value += current_char;
      current_char = nextChar();
    }
    // quote?
    if (current_char == '\'') {
      while (current_char == '\'' && peekChar() == '\'') {
        text += "''";
        value += current_char;     // append single-quote
        current_char = nextChar(); // consume pair of quotes
        current_char = nextChar();
      }
    }
  } while ((current_char != '\'') && (current_char != EOF));
  if (current_char == '\'') {
    nextChar(); // consume the final quote
    text += '\'';
    mType = PascalTokenTypeImpl::STRING;
    mValue = value;
  } else {
    mType = PascalTokenTypeImpl::ERROR;
    mValue = PascalErrorCode::UNEXPECTED_EOF;
  }
  mText = text;
}

PascalSpecialSymbolToken::PascalSpecialSymbolToken(
    std::shared_ptr<Source> source)
    : PascalToken(source, false) {
  PascalSpecialSymbolToken::extract();
}

unique_ptr<PascalToken> PascalSpecialSymbolToken::clone() const {
  return std::make_unique<PascalSpecialSymbolToken>(*this);
}

void PascalSpecialSymbolToken::extract() {
  auto current_char = currentChar();
  mText = std::string{current_char};
  bool invalid_type = false;
  //  mTypeStr = "null";
  switch (current_char) {
  // single-character special symbols
  case '+':
  case '-':
  case '*':
  case '/':
  case ',':
  case ';':
  case '\'':
  case '=':
  case '(':
  case ')':
  case '[':
  case ']':
  case '{':
  case '}':
  case '^': {
    nextChar();
    break;
  }
  // : or :=
  case ':': {
    current_char = nextChar(); // consume ':'
    if (current_char == '=') {
      mText += current_char;
      nextChar(); // consume '='
    }
    break;
  }
  // < or <= or <>
  case '<': {
    current_char = nextChar(); // consume '>'
    if (current_char == '=' || current_char == '>') {
      mText += current_char;
      nextChar(); // consume '=' or '>'
    }
    break;
  }
  // > or >=
  case '>': {
    current_char = nextChar(); // consume '>'
    if (current_char == '=') {
      mText += current_char;
      nextChar(); // consume '='
    }
    break;
  }
  // . or ..
  case '.': {
    current_char = nextChar(); // consume '.'
    if (current_char == '.') {
      mText += current_char;
      nextChar(); // consume '.'
    }
    break;
  }
  default: {
    nextChar(); // consume bad character
    invalid_type = true;
    mType = PascalTokenTypeImpl::ERROR;
    mValue = PascalErrorCode::INVALID_CHARACTER;
  }
  }
  if (!invalid_type) {
    if (specialSymbolsMapRev.find(mText) != specialSymbolsMapRev.end()) {
      mType = specialSymbolsMapRev[mText];
    }
  }
}

PascalNumberToken::PascalNumberToken(std::shared_ptr<Source> source)
    : PascalToken(source, false) {
  PascalNumberToken::extract();
}

unique_ptr<PascalToken> PascalNumberToken::clone() const {
  return std::make_unique<PascalNumberToken>(*this);
}

void PascalNumberToken::extract() {
  std::string text;
  PascalNumberToken::extractNumber(text);
  mText = text;
}

void PascalNumberToken::extractNumber(std::string &text) {
  // TODO: is it possible to parse C-style numbers
  std::string whole_digits, fraction_digits, exponent_digits;
  char exponent_sign = '+';
  bool dot_dot = false;
  char current_char;
  mType = PascalTokenTypeImpl::INTEGER;
  whole_digits = PascalNumberToken::unsignedIntegerDigits(text);
  if (mType == PascalTokenTypeImpl::ERROR) {
    return;
  }
  current_char = currentChar();
  if (current_char == '.') {
    if (peekChar() == '.') {
      dot_dot = true;
    } else {
      mType = PascalTokenTypeImpl::REAL;
      text += current_char;
      current_char = nextChar();
      if (!(std::isspace(peekChar()))) {
        fraction_digits = PascalNumberToken::unsignedIntegerDigits(text);
      }
      if (mType == PascalTokenTypeImpl::ERROR) {
        return;
      }
    }
  }
  current_char = currentChar();
  if (!dot_dot && (current_char == 'E' || current_char == 'e')) {
    mType = PascalTokenTypeImpl::REAL;
    text += current_char;
    current_char = nextChar();
    if (current_char == '+' || current_char == '-') {
      text += current_char;
      exponent_sign = current_char;
      current_char = nextChar();
    }
    exponent_digits = PascalNumberToken::unsignedIntegerDigits(text);
  }
  if (mType == PascalTokenTypeImpl::INTEGER) {
    const long long integer_value = computeIntegerValue(whole_digits);
    if (mType != PascalTokenTypeImpl::ERROR) {
      mValue = integer_value;
    }
  } else {
    const double float_value = computeFloatValue(
        whole_digits, fraction_digits, exponent_digits, exponent_sign);
    if (mType != PascalTokenTypeImpl::ERROR) {
      mValue = float_value;
    }
  }
}

std::string PascalNumberToken::unsignedIntegerDigits(std::string &text) {
  char current_char = currentChar();
  if (!std::isdigit(current_char)) {
    mType = PascalTokenTypeImpl::ERROR;
    mValue = PascalErrorCode::INVALID_NUMBER;
    return "null";
  }
  std::string digits;
  while (std::isdigit(current_char)) {
    text += current_char;
    digits += current_char;
    current_char = nextChar();
  }
  return digits;
}

long long PascalNumberToken::computeIntegerValue(const std::string &digits) {
  // does not consume characters
  bool ok = true;
  // TODO: try to implement toInt without Qt
  long long result = 0;
  try {
    result = std::stoll(digits);
  } catch (const std::exception& e) {
    ok = false;
  }
  if (ok) {
    return result;
  } else {
    mType = PascalTokenTypeImpl::ERROR;
    // TODO: check if integer out of range
    mValue = PascalErrorCode::INVALID_NUMBER;
    return 0;
  }
}

double PascalNumberToken::computeFloatValue(const std::string &whole_digits,
                                            const std::string &fraction_digits,
                                            const std::string &exponent_digits,
                                            char exponent_sign) {
  std::string s = whole_digits;
  if (!fraction_digits.empty()) {
    s += "." + fraction_digits;
  }
  if (!exponent_digits.empty()) {
    s += 'e' + std::string{exponent_sign} + exponent_digits;
  }
  bool ok = true;
  // TODO: try to implement toDouble without Qt
  double result = 0;
  try {
    result = std::stod(s);
  } catch (const std::exception& e) {
    ok = false;
  }
  if (ok) {
    return result;
  } else {
    mType = PascalTokenTypeImpl::ERROR;
    // TODO: check if integer out of range
    mValue = PascalErrorCode::INVALID_NUMBER;
    return 0;
  }
}

const std::set<PascalTokenTypeImpl>
    PascalSubparserTopDownBase::mStatementStartSet{
        PascalTokenTypeImpl::BEGIN,      PascalTokenTypeImpl::CASE,
        PascalTokenTypeImpl::FOR,        PascalTokenTypeImpl::IF,
        PascalTokenTypeImpl::REPEAT,     PascalTokenTypeImpl::WHILE,
        PascalTokenTypeImpl::IDENTIFIER, PascalTokenTypeImpl::SEMICOLON};

const std::set<PascalTokenTypeImpl>
    PascalSubparserTopDownBase::mStatementFollowSet{
        PascalTokenTypeImpl::SEMICOLON, PascalTokenTypeImpl::END,
        PascalTokenTypeImpl::ELSE, PascalTokenTypeImpl::UNTIL,
        PascalTokenTypeImpl::DOT};

const std::set<PascalTokenTypeImpl>
    PascalSubparserTopDownBase::mExpressionStartSet{
        PascalTokenTypeImpl::PLUS, PascalTokenTypeImpl::MINUS,
        PascalTokenTypeImpl::IDENTIFIER, PascalTokenTypeImpl::INTEGER,
        PascalTokenTypeImpl::REAL, PascalTokenTypeImpl::STRING,
        PascalTokenTypeImpl::NOT, PascalTokenTypeImpl::LEFT_PAREN};

decltype(PascalSubparserTopDownBase::mConstantStartSet)
    PascalSubparserTopDownBase::mConstantStartSet{
        PascalTokenTypeImpl::IDENTIFIER, PascalTokenTypeImpl::INTEGER,
        PascalTokenTypeImpl::PLUS, PascalTokenTypeImpl::MINUS,
        PascalTokenTypeImpl::STRING};

decltype(PascalSubparserTopDownBase::mDeclarationStartSet)
PascalSubparserTopDownBase::mDeclarationStartSet{
  PascalTokenTypeImpl::CONST, PascalTokenTypeImpl::TYPE,
  PascalTokenTypeImpl::VAR, PascalTokenTypeImpl::PROCEDURE,
  PascalTokenTypeImpl::FUNCTION, PascalTokenTypeImpl::BEGIN
};

std::set<PascalTokenTypeImpl> initColonEqualsSet() {
  std::set<PascalTokenTypeImpl> s(PascalSubparserTopDownBase::mExpressionStartSet);
  s.insert(PascalTokenTypeImpl::COLON_EQUALS);
  s.insert(PascalSubparserTopDownBase::mStatementFollowSet.begin(),
           PascalSubparserTopDownBase::mStatementFollowSet.end());
  return s;
}

decltype(PascalSubparserTopDownBase::mOfSet) initOfSet() {
  std::remove_const_t<decltype(PascalSubparserTopDownBase::mOfSet)> s(
      PascalSubparserTopDownBase::mConstantStartSet);
  s.insert(PascalTokenTypeImpl::OF);
  s.insert(PascalSubparserTopDownBase::mStatementFollowSet.begin(),
           PascalSubparserTopDownBase::mStatementFollowSet.end());
  return s;
}

decltype(PascalSubparserTopDownBase::mCommaSet) initCommaSet() {
  std::remove_const_t<decltype(PascalSubparserTopDownBase::mCommaSet)> s(
      PascalSubparserTopDownBase::mConstantStartSet);
  s.insert({PascalTokenTypeImpl::COMMA, PascalTokenTypeImpl::COLON});
  s.insert(PascalSubparserTopDownBase::mStatementStartSet.begin(),
           PascalSubparserTopDownBase::mStatementStartSet.end());
  s.insert(PascalSubparserTopDownBase::mStatementFollowSet.begin(),
           PascalSubparserTopDownBase::mStatementFollowSet.end());
  return s;
}

std::set<PascalTokenTypeImpl> initToDowntoSet() {
  std::set<PascalTokenTypeImpl> s(PascalSubparserTopDownBase::mExpressionStartSet);
  s.insert(PascalTokenTypeImpl::TO);
  s.insert(PascalTokenTypeImpl::DOWNTO);
  s.insert(PascalSubparserTopDownBase::mStatementFollowSet.begin(),
           PascalSubparserTopDownBase::mStatementFollowSet.end());
  return s;
}

std::set<PascalTokenTypeImpl> initDoSet() {
  std::set<PascalTokenTypeImpl> s(PascalSubparserTopDownBase::mStatementStartSet);
  s.insert(PascalTokenTypeImpl::DO);
  s.insert(PascalSubparserTopDownBase::mStatementFollowSet.begin(),
           PascalSubparserTopDownBase::mStatementFollowSet.end());
  return s;
}

std::set<PascalTokenTypeImpl> initThenSet() {
  std::set<PascalTokenTypeImpl> s(PascalSubparserTopDownBase::mStatementStartSet);
  s.insert(PascalTokenTypeImpl::THEN);
  s.insert(PascalSubparserTopDownBase::mStatementFollowSet.begin(),
           PascalSubparserTopDownBase::mStatementFollowSet.end());
  return s;
}

std::set<PascalTokenTypeImpl> initTypeStartSet() {
  auto s = PascalSubparserTopDownBase::mTypeStartSet;
  s.erase(PascalTokenTypeImpl::CONST);
  return s;
}

std::set<PascalTokenTypeImpl> initVarStartSet() {
  auto s = PascalSubparserTopDownBase::mTypeStartSet;
  s.erase(PascalTokenTypeImpl::TYPE);
  return s;
}

std::set<PascalTokenTypeImpl> initRoutineStartSet() {
  auto s = PascalSubparserTopDownBase::mTypeStartSet;
  s.erase(PascalTokenTypeImpl::TYPE);
  s.erase(PascalTokenTypeImpl::VAR);
  return s;
}

decltype(PascalSubparserTopDownBase::mColonEqualsSet)
PascalSubparserTopDownBase::mColonEqualsSet = initColonEqualsSet();

decltype(PascalSubparserTopDownBase::mOfSet)
PascalSubparserTopDownBase::mOfSet = initOfSet();

decltype(PascalSubparserTopDownBase::mCommaSet)
PascalSubparserTopDownBase::mCommaSet = initCommaSet();

decltype(PascalSubparserTopDownBase::mToDowntoSet)
PascalSubparserTopDownBase::mToDowntoSet = initToDowntoSet();

decltype(PascalSubparserTopDownBase::mDoSet)
PascalSubparserTopDownBase::mDoSet = initDoSet();

decltype(PascalSubparserTopDownBase::mThenSet)
PascalSubparserTopDownBase::mThenSet = initThenSet();

decltype(PascalSubparserTopDownBase::mTypeStartSet)
PascalSubparserTopDownBase::mTypeStartSet = initTypeStartSet();

decltype(PascalSubparserTopDownBase::mVarStartSet)
PascalSubparserTopDownBase::mVarStartSet = initVarStartSet();

decltype(PascalSubparserTopDownBase::mRoutineStartSet)
PascalSubparserTopDownBase::mRoutineStartSet = initRoutineStartSet();

const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> PascalSubparserTopDownBase::mRelOpsMap =
  {{PascalTokenTypeImpl::EQUALS, ICodeNodeTypeImpl::EQ},
   {PascalTokenTypeImpl::NOT_EQUALS, ICodeNodeTypeImpl::NE},
   {PascalTokenTypeImpl::LESS_THAN, ICodeNodeTypeImpl::LT},
   {PascalTokenTypeImpl::LESS_EQUALS, ICodeNodeTypeImpl::LE},
   {PascalTokenTypeImpl::GREATER_THAN, ICodeNodeTypeImpl::GT},
   {PascalTokenTypeImpl::GREATER_EQUALS, ICodeNodeTypeImpl::GE}};

const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> PascalSubparserTopDownBase::mAddOpsMap =
  {{PascalTokenTypeImpl::PLUS, ICodeNodeTypeImpl::ADD},
   {PascalTokenTypeImpl::MINUS, ICodeNodeTypeImpl::SUBTRACT},
   {PascalTokenTypeImpl::OR, ICodeNodeTypeImpl::OR}};

const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> PascalSubparserTopDownBase::mMultOpsMap =
  {{PascalTokenTypeImpl::STAR, ICodeNodeTypeImpl::MULTIPLY},
   {PascalTokenTypeImpl::SLASH, ICodeNodeTypeImpl::FLOAT_DIVIDE},
   {PascalTokenTypeImpl::DIV, ICodeNodeTypeImpl::INTEGER_DIVIDE},
   {PascalTokenTypeImpl::MOD, ICodeNodeTypeImpl::MOD},
   {PascalTokenTypeImpl::AND, ICodeNodeTypeImpl::AND}};

PascalSubparserTopDownBase::PascalSubparserTopDownBase(
    PascalParserTopDown &pascal_parser)
    : mPascalParser(pascal_parser) {}

PascalSubparserTopDownBase::~PascalSubparserTopDownBase() {}

std::shared_ptr<PascalToken> PascalSubparserTopDownBase::currentToken() const {
  return mPascalParser.currentToken();
}

std::shared_ptr<PascalToken> PascalSubparserTopDownBase::nextToken() {
  return mPascalParser.nextToken();
}

std::shared_ptr<PascalToken> PascalSubparserTopDownBase::synchronize(
    const std::set<PascalTokenTypeImpl> &sync_set) {
  return mPascalParser.synchronize(sync_set);
}

std::shared_ptr<SymbolTableStackImplBase> PascalSubparserTopDownBase::getSymbolTableStack() {
  return mPascalParser.getSymbolTableStack();
}

std::shared_ptr<ICodeImplBase> PascalSubparserTopDownBase::getICode() const {
  return mPascalParser.getICode();
}

std::shared_ptr<PascalScanner> PascalSubparserTopDownBase::scanner() const {
  return mPascalParser.scanner();
}

int PascalSubparserTopDownBase::errorCount() {
  return mPascalParser.errorCount();
}

PascalErrorHandler *PascalSubparserTopDownBase::errorHandler() {
  return mPascalParser.mErrorHandler;
}

PascalParserTopDown *PascalSubparserTopDownBase::currentParser() {
  return &mPascalParser;
}

std::unique_ptr<ICodeNodeImplBase> PascalSubparserTopDownBase::parse(std::shared_ptr<PascalToken> token)
{
  // should not enter this function
  // the parser does not implement parse(std::shared_ptr<PascalToken>)!
  std::cerr << "BUG: Unimplemented parse!\n";
  std::cerr << BOOST_CURRENT_FUNCTION << '\n';
  std::abort();
  return nullptr;
}

void PascalSubparserTopDownBase::setLineNumber(
    std::unique_ptr<ICodeNodeImplBase> &node,
    const std::shared_ptr<PascalToken> &token) {
  if (node != nullptr) {
    node->setAttribute(ICodeKeyTypeImpl::LINE, token->lineNum());
  }
}

PascalEofToken::PascalEofToken() { mType = PascalTokenTypeImpl::END_OF_FILE; }

PascalEofToken::PascalEofToken(std::shared_ptr<Source> source)
    : EofToken<PascalTokenTypeImpl>(source) {
  mType = PascalTokenTypeImpl::END_OF_FILE;
}
