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
  std::map<PascalTokenTypeImpl, std::string> reserve_words_map;
  // reserved words
  reserve_words_map[PascalTokenTypeImpl::AND] = std::string("and");
  reserve_words_map[PascalTokenTypeImpl::ARRAY] = std::string("array");
  reserve_words_map[PascalTokenTypeImpl::BEGIN] = std::string("begin");
  reserve_words_map[PascalTokenTypeImpl::CASE] = std::string("case");
  reserve_words_map[PascalTokenTypeImpl::CONST] = std::string("const");
  reserve_words_map[PascalTokenTypeImpl::DIV] = std::string("div");
  reserve_words_map[PascalTokenTypeImpl::DO] = std::string("do");
  reserve_words_map[PascalTokenTypeImpl::DOWNTO] = std::string("downto");
  reserve_words_map[PascalTokenTypeImpl::ELSE] = std::string("else");
  reserve_words_map[PascalTokenTypeImpl::END] = std::string("end");
  reserve_words_map[PascalTokenTypeImpl::FILE] = std::string("file");
  reserve_words_map[PascalTokenTypeImpl::FOR] = std::string("for");
  reserve_words_map[PascalTokenTypeImpl::FUNCTION] = std::string("function");
  reserve_words_map[PascalTokenTypeImpl::GOTO] = std::string("goto");
  reserve_words_map[PascalTokenTypeImpl::IF] = std::string("if");
  reserve_words_map[PascalTokenTypeImpl::IN] = std::string("in");
  reserve_words_map[PascalTokenTypeImpl::LABEL] = std::string("label");
  reserve_words_map[PascalTokenTypeImpl::MOD] = std::string("mod");
  reserve_words_map[PascalTokenTypeImpl::NIL] = std::string("nil");
  reserve_words_map[PascalTokenTypeImpl::NOT] = std::string("not");
  reserve_words_map[PascalTokenTypeImpl::OF] = std::string("of");
  reserve_words_map[PascalTokenTypeImpl::OR] = std::string("or");
  reserve_words_map[PascalTokenTypeImpl::PACKED] = std::string("packed");
  reserve_words_map[PascalTokenTypeImpl::PROCEDURE] = std::string("procedure");
  reserve_words_map[PascalTokenTypeImpl::PROGRAM] = std::string("program");
  reserve_words_map[PascalTokenTypeImpl::RECORD] = std::string("record");
  reserve_words_map[PascalTokenTypeImpl::REPEAT] = std::string("repeat");
  reserve_words_map[PascalTokenTypeImpl::SET] = std::string("set");
  reserve_words_map[PascalTokenTypeImpl::THEN] = std::string("then");
  reserve_words_map[PascalTokenTypeImpl::TO] = std::string("to");
  reserve_words_map[PascalTokenTypeImpl::TYPE] = std::string("type");
  reserve_words_map[PascalTokenTypeImpl::UNTIL] = std::string("until");
  reserve_words_map[PascalTokenTypeImpl::VAR] = std::string("var");
  reserve_words_map[PascalTokenTypeImpl::WHILE] = std::string("while");
  reserve_words_map[PascalTokenTypeImpl::WITH] = std::string("with");
  return reserve_words_map;
}

std::map<PascalTokenTypeImpl, std::string> initSpecialSymbolsMap() {
  std::map<PascalTokenTypeImpl, std::string> special_symbols_map;
  // special symbols
  special_symbols_map[PascalTokenTypeImpl::PLUS] = std::string("+");
  special_symbols_map[PascalTokenTypeImpl::MINUS] = std::string("-");
  special_symbols_map[PascalTokenTypeImpl::STAR] = std::string("*");
  special_symbols_map[PascalTokenTypeImpl::SLASH] = std::string("/");
  special_symbols_map[PascalTokenTypeImpl::COLON_EQUALS] = std::string(":=");
  special_symbols_map[PascalTokenTypeImpl::DOT] = std::string(".");
  special_symbols_map[PascalTokenTypeImpl::COMMA] = std::string(",");
  special_symbols_map[PascalTokenTypeImpl::SEMICOLON] = std::string(";");
  special_symbols_map[PascalTokenTypeImpl::COLON] = std::string(":");
  special_symbols_map[PascalTokenTypeImpl::QUOTE] = std::string("'");
  special_symbols_map[PascalTokenTypeImpl::EQUALS] = std::string("=");
  special_symbols_map[PascalTokenTypeImpl::NOT_EQUALS] = std::string("<>");
  special_symbols_map[PascalTokenTypeImpl::LESS_THAN] = std::string("<");
  special_symbols_map[PascalTokenTypeImpl::LESS_EQUALS] = std::string("<=");
  special_symbols_map[PascalTokenTypeImpl::GREATER_EQUALS] = std::string(">=");
  special_symbols_map[PascalTokenTypeImpl::GREATER_THAN] = std::string(">");
  special_symbols_map[PascalTokenTypeImpl::LEFT_PAREN] = std::string("(");
  special_symbols_map[PascalTokenTypeImpl::RIGHT_PAREN] = std::string(")");
  special_symbols_map[PascalTokenTypeImpl::LEFT_BRACKET] = std::string("[");
  special_symbols_map[PascalTokenTypeImpl::RIGHT_BRACKET] = std::string("]");
  special_symbols_map[PascalTokenTypeImpl::LEFT_BRACE] = std::string("{");
  special_symbols_map[PascalTokenTypeImpl::RIGHT_BRACE] = std::string("}");
  special_symbols_map[PascalTokenTypeImpl::UP_ARROW] = std::string("^");
  special_symbols_map[PascalTokenTypeImpl::DOT_DOT] = std::string("..");
  return special_symbols_map;
}

std::map<PascalTokenTypeImpl, std::string> initSpecialWordsMap() {
  std::map<PascalTokenTypeImpl, std::string> special_words_map;
  // split this
  special_words_map[PascalTokenTypeImpl::IDENTIFIER] = std::string("identifier");
  special_words_map[PascalTokenTypeImpl::INTEGER] = std::string("integer");
  special_words_map[PascalTokenTypeImpl::REAL] = std::string("real");
  special_words_map[PascalTokenTypeImpl::STRING] = std::string("string");
  special_words_map[PascalTokenTypeImpl::ERROR] = std::string("error");
  special_words_map[PascalTokenTypeImpl::END_OF_FILE] =
      std::string("end_of_file");
  return special_words_map;
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
#ifdef DEBUG
  if (sync_set.empty()) {
    qDebug() << "Empty synchronization set!";
  }
#endif
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
    const auto integer_value = computeIntegerValue(whole_digits);
    if (mType != PascalTokenTypeImpl::ERROR) {
      mValue = integer_value;
    }
  } else {
    const PascalFloat float_value = computeFloatValue(
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

PascalInteger PascalNumberToken::computeIntegerValue(const std::string &digits) {
  // does not consume characters
  bool ok = true;
  // TODO: try to implement toInt without Qt
  PascalInteger result = 0;
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

PascalFloat PascalNumberToken::computeFloatValue(const std::string &whole_digits,
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
  PascalFloat result = 0;
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
PascalSubparserTopDownBase::statementStartSet{
  PascalTokenTypeImpl::BEGIN,      PascalTokenTypeImpl::CASE,
  PascalTokenTypeImpl::FOR,        PascalTokenTypeImpl::IF,
  PascalTokenTypeImpl::REPEAT,     PascalTokenTypeImpl::WHILE,
  PascalTokenTypeImpl::IDENTIFIER, PascalTokenTypeImpl::SEMICOLON};

const std::set<PascalTokenTypeImpl>
PascalSubparserTopDownBase::statementFollowSet{
  PascalTokenTypeImpl::SEMICOLON, PascalTokenTypeImpl::END,
  PascalTokenTypeImpl::ELSE,      PascalTokenTypeImpl::UNTIL,
  PascalTokenTypeImpl::DOT};

const std::set<PascalTokenTypeImpl>
PascalSubparserTopDownBase::expressionStartSet{
  PascalTokenTypeImpl::PLUS,        PascalTokenTypeImpl::MINUS,
  PascalTokenTypeImpl::IDENTIFIER,  PascalTokenTypeImpl::INTEGER,
  PascalTokenTypeImpl::REAL,        PascalTokenTypeImpl::STRING,
  PascalTokenTypeImpl::NOT,         PascalTokenTypeImpl::LEFT_PAREN};

decltype(PascalSubparserTopDownBase::constantStartSet)
PascalSubparserTopDownBase::constantStartSet{
  PascalTokenTypeImpl::IDENTIFIER,  PascalTokenTypeImpl::INTEGER,
  PascalTokenTypeImpl::PLUS,        PascalTokenTypeImpl::MINUS,
  PascalTokenTypeImpl::STRING};

decltype(PascalSubparserTopDownBase::declarationStartSet)
PascalSubparserTopDownBase::declarationStartSet{
  PascalTokenTypeImpl::CONST,     PascalTokenTypeImpl::TYPE,
  PascalTokenTypeImpl::VAR,       PascalTokenTypeImpl::PROCEDURE,
  PascalTokenTypeImpl::FUNCTION,  PascalTokenTypeImpl::BEGIN
};

decltype(PascalSubparserTopDownBase::enumConstantStartSet)
PascalSubparserTopDownBase::enumConstantStartSet{
  PascalTokenTypeImpl::IDENTIFIER, PascalTokenTypeImpl::COMMA
};

decltype(PascalSubparserTopDownBase::rightBracketSet)
PascalSubparserTopDownBase::rightBracketSet{
  PascalTokenTypeImpl::RIGHT_BRACKET, PascalTokenTypeImpl::OF,
  PascalTokenTypeImpl::SEMICOLON
};

decltype(PascalSubparserTopDownBase::indexEndSet)
PascalSubparserTopDownBase::indexEndSet{
  PascalTokenTypeImpl::RIGHT_BRACKET, PascalTokenTypeImpl::SEMICOLON
};

auto initColonEqualsSet() {
  auto s(PascalSubparserTopDownBase::expressionStartSet);
  s.insert(PascalTokenTypeImpl::COLON_EQUALS);
  s.insert(PascalSubparserTopDownBase::statementFollowSet.begin(),
           PascalSubparserTopDownBase::statementFollowSet.end());
  return s;
}

auto initOfSet() {
  auto s(PascalSubparserTopDownBase::constantStartSet);
  s.insert(PascalTokenTypeImpl::OF);
  s.insert(PascalSubparserTopDownBase::statementFollowSet.begin(),
           PascalSubparserTopDownBase::statementFollowSet.end());
  return s;
}

auto initCommaSet() {
  auto s(PascalSubparserTopDownBase::constantStartSet);
  s.insert({PascalTokenTypeImpl::COMMA, PascalTokenTypeImpl::COLON});
  s.insert(PascalSubparserTopDownBase::statementStartSet.begin(),
           PascalSubparserTopDownBase::statementStartSet.end());
  s.insert(PascalSubparserTopDownBase::statementFollowSet.begin(),
           PascalSubparserTopDownBase::statementFollowSet.end());
  return s;
}

auto initToDowntoSet() {
  auto s(PascalSubparserTopDownBase::expressionStartSet);
  s.insert(PascalTokenTypeImpl::TO);
  s.insert(PascalTokenTypeImpl::DOWNTO);
  s.insert(PascalSubparserTopDownBase::statementFollowSet.begin(),
           PascalSubparserTopDownBase::statementFollowSet.end());
  return s;
}

auto initDoSet() {
  auto s(PascalSubparserTopDownBase::statementStartSet);
  s.insert(PascalTokenTypeImpl::DO);
  s.insert(PascalSubparserTopDownBase::statementFollowSet.begin(),
           PascalSubparserTopDownBase::statementFollowSet.end());
  return s;
}

auto initThenSet() {
  auto s(PascalSubparserTopDownBase::statementStartSet);
  s.insert(PascalTokenTypeImpl::THEN);
  s.insert(PascalSubparserTopDownBase::statementFollowSet.begin(),
           PascalSubparserTopDownBase::statementFollowSet.end());
  return s;
}

auto initTypeStartSet() {
  auto s = PascalSubparserTopDownBase::declarationStartSet;
  s.erase(PascalTokenTypeImpl::CONST);
  return s;
}

auto initVarStartSet() {
  auto s = PascalSubparserTopDownBase::typeStartSet;
  s.erase(PascalTokenTypeImpl::TYPE);
  return s;
}

auto initRoutineStartSet() {
  auto s = PascalSubparserTopDownBase::typeStartSet;
  s.erase(PascalTokenTypeImpl::TYPE);
  s.erase(PascalTokenTypeImpl::VAR);
  return s;
}

auto initIdentifierSet() {
  auto s = PascalSubparserTopDownBase::typeStartSet;
  s.insert(PascalTokenTypeImpl::IDENTIFIER);
  return s;
}

auto initEqualsSet() {
  auto s = PascalSubparserTopDownBase::constantStartSet;
  s.insert(PascalTokenTypeImpl::EQUALS);
  s.insert(PascalTokenTypeImpl::SEMICOLON);
  return s;
}

auto initNextStartSet() {
  auto s = PascalSubparserTopDownBase::typeStartSet;
  s.insert(PascalTokenTypeImpl::IDENTIFIER);
  s.insert(PascalTokenTypeImpl::SEMICOLON);
  return s;
}

auto initSimpleTypeStartSet() {
  auto s = PascalSubparserTopDownBase::constantStartSet;
  s.insert(PascalTokenTypeImpl::LEFT_PAREN);
  s.insert(PascalTokenTypeImpl::COMMA);
  s.insert(PascalTokenTypeImpl::SEMICOLON);
  return s;
}

auto initEnumDefinitionFollowSet() {
  auto s = PascalSubparserTopDownBase::varStartSet;
  s.insert(PascalTokenTypeImpl::RIGHT_PAREN);
  s.insert(PascalTokenTypeImpl::SEMICOLON);
  return s;
}

auto initLeftBracketSet() {
  auto s = PascalSubparserTopDownBase::simpleTypeStartSet;
  s.insert(PascalTokenTypeImpl::LEFT_BRACKET);
  s.insert(PascalTokenTypeImpl::RIGHT_BRACKET);
  return s;
}

auto initArrayTypeOfSet() {
  auto s = PascalSubparserTopDownBase::typeStartSet;
  s.insert(PascalTokenTypeImpl::OF);
  s.insert(PascalTokenTypeImpl::SEMICOLON);
  return s;
}

auto initIndexStartSet() {
  auto s = PascalSubparserTopDownBase::simpleTypeStartSet;
  s.insert(PascalTokenTypeImpl::COMMA);
  return s;
}

auto initIndexFollowSet() {
  auto s = PascalSubparserTopDownBase::indexStartSet;
  s.insert(PascalSubparserTopDownBase::indexEndSet.begin(),
           PascalSubparserTopDownBase::indexEndSet.end());
  return s;
}

auto initRecordEndSet() {
  auto s = PascalSubparserTopDownBase::varStartSet;
  s.insert(PascalTokenTypeImpl::END);
  s.insert(PascalTokenTypeImpl::SEMICOLON);
  return s;
}

decltype(PascalSubparserTopDownBase::colonEqualsSet)
PascalSubparserTopDownBase::colonEqualsSet = initColonEqualsSet();

decltype(PascalSubparserTopDownBase::ofSet)
PascalSubparserTopDownBase::ofSet = initOfSet();

decltype(PascalSubparserTopDownBase::commaSet)
PascalSubparserTopDownBase::commaSet = initCommaSet();

decltype(PascalSubparserTopDownBase::toDowntoSet)
PascalSubparserTopDownBase::toDowntoSet = initToDowntoSet();

decltype(PascalSubparserTopDownBase::doSet)
PascalSubparserTopDownBase::doSet = initDoSet();

decltype(PascalSubparserTopDownBase::thenSet)
PascalSubparserTopDownBase::thenSet = initThenSet();

decltype(PascalSubparserTopDownBase::typeStartSet)
PascalSubparserTopDownBase::typeStartSet = initTypeStartSet();

decltype(PascalSubparserTopDownBase::varStartSet)
PascalSubparserTopDownBase::varStartSet = initVarStartSet();

decltype(PascalSubparserTopDownBase::routineStartSet)
PascalSubparserTopDownBase::routineStartSet = initRoutineStartSet();

decltype(PascalSubparserTopDownBase::equalsSet)
PascalSubparserTopDownBase::equalsSet = initEqualsSet();

decltype(PascalSubparserTopDownBase::nextStartSet)
PascalSubparserTopDownBase::nextStartSet = initNextStartSet();

decltype(PascalSubparserTopDownBase::simpleTypeStartSet)
PascalSubparserTopDownBase::simpleTypeStartSet = initSimpleTypeStartSet();

decltype(PascalSubparserTopDownBase::enumDefinitionFollowSet)
PascalSubparserTopDownBase::enumDefinitionFollowSet = initEnumDefinitionFollowSet();

decltype(PascalSubparserTopDownBase::leftBracketSet)
PascalSubparserTopDownBase::leftBracketSet = initLeftBracketSet();

decltype(PascalSubparserTopDownBase::arrayTypeOfSet)
PascalSubparserTopDownBase::arrayTypeOfSet = initArrayTypeOfSet();

decltype(PascalSubparserTopDownBase::indexStartSet)
PascalSubparserTopDownBase::indexStartSet = initIndexStartSet();

decltype(PascalSubparserTopDownBase::indexFollowSet)
PascalSubparserTopDownBase::indexFollowSet = initIndexFollowSet();

decltype(PascalSubparserTopDownBase::recordEndSet)
PascalSubparserTopDownBase::recordEndSet = initRecordEndSet();

const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> PascalSubparserTopDownBase::relOpsMap =
  {{PascalTokenTypeImpl::EQUALS,          ICodeNodeTypeImpl::EQ},
   {PascalTokenTypeImpl::NOT_EQUALS,      ICodeNodeTypeImpl::NE},
   {PascalTokenTypeImpl::LESS_THAN,       ICodeNodeTypeImpl::LT},
   {PascalTokenTypeImpl::LESS_EQUALS,     ICodeNodeTypeImpl::LE},
   {PascalTokenTypeImpl::GREATER_THAN,    ICodeNodeTypeImpl::GT},
   {PascalTokenTypeImpl::GREATER_EQUALS,  ICodeNodeTypeImpl::GE}};

const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> PascalSubparserTopDownBase::addOpsMap =
  {{PascalTokenTypeImpl::PLUS,  ICodeNodeTypeImpl::ADD},
   {PascalTokenTypeImpl::MINUS, ICodeNodeTypeImpl::SUBTRACT},
   {PascalTokenTypeImpl::OR,    ICodeNodeTypeImpl::OR}};

const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> PascalSubparserTopDownBase::multOpsMap =
  {{PascalTokenTypeImpl::STAR,  ICodeNodeTypeImpl::MULTIPLY},
   {PascalTokenTypeImpl::SLASH, ICodeNodeTypeImpl::FLOAT_DIVIDE},
   {PascalTokenTypeImpl::DIV,   ICodeNodeTypeImpl::INTEGER_DIVIDE},
   {PascalTokenTypeImpl::MOD,   ICodeNodeTypeImpl::MOD},
   {PascalTokenTypeImpl::AND,   ICodeNodeTypeImpl::AND}};

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

//std::shared_ptr<ICodeImplBase> PascalSubparserTopDownBase::getICode() const {
//  return mPascalParser.getICode();
//}

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
