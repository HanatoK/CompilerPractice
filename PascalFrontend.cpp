#include "PascalFrontend.h"
#include "IntermediateImpl.h"
#include "Common.h"
#include "StatementParser.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTime>
#include <cctype>
#include <fmt/format.h>


std::map<PascalTokenTypeImpl, QString> initReservedWordsMap() {
  std::map<PascalTokenTypeImpl, QString> reservedWordsMap;
  // reserved words
  reservedWordsMap[PascalTokenTypeImpl::AND] = QString("and");
  reservedWordsMap[PascalTokenTypeImpl::ARRAY] = QString("array");
  reservedWordsMap[PascalTokenTypeImpl::BEGIN] = QString("begin");
  reservedWordsMap[PascalTokenTypeImpl::CASE] = QString("case");
  reservedWordsMap[PascalTokenTypeImpl::CONST] = QString("const");
  reservedWordsMap[PascalTokenTypeImpl::DIV] = QString("div");
  reservedWordsMap[PascalTokenTypeImpl::DO] = QString("do");
  reservedWordsMap[PascalTokenTypeImpl::DOWNTO] = QString("downto");
  reservedWordsMap[PascalTokenTypeImpl::ELSE] = QString("else");
  reservedWordsMap[PascalTokenTypeImpl::END] = QString("end");
  reservedWordsMap[PascalTokenTypeImpl::FILE] = QString("file");
  reservedWordsMap[PascalTokenTypeImpl::FOR] = QString("for");
  reservedWordsMap[PascalTokenTypeImpl::FUNCTION] = QString("function");
  reservedWordsMap[PascalTokenTypeImpl::GOTO] = QString("goto");
  reservedWordsMap[PascalTokenTypeImpl::IF] = QString("if");
  reservedWordsMap[PascalTokenTypeImpl::IN] = QString("in");
  reservedWordsMap[PascalTokenTypeImpl::LABEL] = QString("label");
  reservedWordsMap[PascalTokenTypeImpl::MOD] = QString("mod");
  reservedWordsMap[PascalTokenTypeImpl::NIL] = QString("nil");
  reservedWordsMap[PascalTokenTypeImpl::NOT] = QString("not");
  reservedWordsMap[PascalTokenTypeImpl::OF] = QString("of");
  reservedWordsMap[PascalTokenTypeImpl::OR] = QString("or");
  reservedWordsMap[PascalTokenTypeImpl::PACKED] = QString("packed");
  reservedWordsMap[PascalTokenTypeImpl::PROCEDURE] = QString("procedure");
  reservedWordsMap[PascalTokenTypeImpl::PROGRAM] = QString("program");
  reservedWordsMap[PascalTokenTypeImpl::RECORD] = QString("record");
  reservedWordsMap[PascalTokenTypeImpl::REPEAT] = QString("repeat");
  reservedWordsMap[PascalTokenTypeImpl::SET] = QString("set");
  reservedWordsMap[PascalTokenTypeImpl::THEN] = QString("then");
  reservedWordsMap[PascalTokenTypeImpl::TO] = QString("to");
  reservedWordsMap[PascalTokenTypeImpl::TYPE] = QString("type");
  reservedWordsMap[PascalTokenTypeImpl::UNTIL] = QString("until");
  reservedWordsMap[PascalTokenTypeImpl::VAR] = QString("var");
  reservedWordsMap[PascalTokenTypeImpl::WHILE] = QString("while");
  reservedWordsMap[PascalTokenTypeImpl::WITH] = QString("with");
  return reservedWordsMap;
}

std::map<PascalTokenTypeImpl, QString> initSpecialSymbolsMap() {
  std::map<PascalTokenTypeImpl, QString> specialSymbolsMap;
  // special symbols
  specialSymbolsMap[PascalTokenTypeImpl::PLUS]            = QString("+");
  specialSymbolsMap[PascalTokenTypeImpl::MINUS]           = QString("-");
  specialSymbolsMap[PascalTokenTypeImpl::STAR]            = QString("*");
  specialSymbolsMap[PascalTokenTypeImpl::SLASH]           = QString("/");
  specialSymbolsMap[PascalTokenTypeImpl::COLON_EQUALS]    = QString(":=");
  specialSymbolsMap[PascalTokenTypeImpl::DOT]             = QString(".");
  specialSymbolsMap[PascalTokenTypeImpl::COMMA]           = QString(",");
  specialSymbolsMap[PascalTokenTypeImpl::SEMICOLON]       = QString(";");
  specialSymbolsMap[PascalTokenTypeImpl::COLON]           = QString(":");
  specialSymbolsMap[PascalTokenTypeImpl::QUOTE]           = QString("'");
  specialSymbolsMap[PascalTokenTypeImpl::EQUALS]          = QString("=");
  specialSymbolsMap[PascalTokenTypeImpl::NOT_EQUALS]      = QString("<>");
  specialSymbolsMap[PascalTokenTypeImpl::LESS_THAN]       = QString("<");
  specialSymbolsMap[PascalTokenTypeImpl::LESS_EQUALS]     = QString("<=");
  specialSymbolsMap[PascalTokenTypeImpl::GREATER_EQUALS]  = QString(">=");
  specialSymbolsMap[PascalTokenTypeImpl::GREATER_THAN]    = QString(">");
  specialSymbolsMap[PascalTokenTypeImpl::LEFT_PAREN]      = QString("(");
  specialSymbolsMap[PascalTokenTypeImpl::RIGHT_PAREN]     = QString(")");
  specialSymbolsMap[PascalTokenTypeImpl::LEFT_BRACKET]    = QString("[");
  specialSymbolsMap[PascalTokenTypeImpl::RIGHT_BRACKET]   = QString("]");
  specialSymbolsMap[PascalTokenTypeImpl::LEFT_BRACE]      = QString("{");
  specialSymbolsMap[PascalTokenTypeImpl::RIGHT_BRACE]     = QString("}");
  specialSymbolsMap[PascalTokenTypeImpl::UP_ARROW]        = QString("^");
  specialSymbolsMap[PascalTokenTypeImpl::DOT_DOT]         = QString("..");
  return specialSymbolsMap;
}

std::map<PascalTokenTypeImpl, QString> initSpecialWordsMap() {
  std::map<PascalTokenTypeImpl, QString> specialWordsMap;
  // split this
  specialWordsMap[PascalTokenTypeImpl::IDENTIFIER] = QString("identifier");
  specialWordsMap[PascalTokenTypeImpl::INTEGER] = QString("integer");
  specialWordsMap[PascalTokenTypeImpl::REAL] = QString("real");
  specialWordsMap[PascalTokenTypeImpl::STRING] = QString("string");
  specialWordsMap[PascalTokenTypeImpl::ERROR] = QString("error");
  specialWordsMap[PascalTokenTypeImpl::END_OF_FILE] = QString("end_of_file");
  return specialWordsMap;
}

std::map<PascalTokenTypeImpl, QString> mReservedWordsMap = initReservedWordsMap();
std::map<PascalTokenTypeImpl, QString> mSpecialSymbolsMap = initSpecialSymbolsMap();
std::map<PascalTokenTypeImpl, QString> mSpecialWordsMap = initSpecialWordsMap();
std::map<QString, PascalTokenTypeImpl> mReservedWordsMapRev = reverse_map(mReservedWordsMap);
std::map<QString, PascalTokenTypeImpl> mSpecialSymbolsMapRev = reverse_map(mSpecialSymbolsMap);
std::map<QString, PascalTokenTypeImpl> mSpecialWordsMapRev = reverse_map(mSpecialWordsMap);

PascalParserTopDown::PascalParserTopDown(std::shared_ptr<PascalScanner> scanner)
    : Parser(scanner), mErrorHandler(nullptr) {
  mErrorHandler = new PascalErrorHandler();
}

PascalParserTopDown::~PascalParserTopDown() {
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

void PascalParserTopDown::parse() {
  const int startTime = QTime::currentTime().msec();
  mICode = createICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>();
  auto token = nextToken();
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> root_node = nullptr;
  while (!token->isEof()) {
    if (token->type() != PascalTokenTypeImpl::ERROR) {
//      if (pascal_token->type() == PascalTokenType::BEGIN) {
//        StatementParser statement_parser(*this);
//        root_node = statement_parser.parse(token);
//        token = currentToken();
//      } else {
//        mErrorHandler->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, this);
//      }
//      if (pascal_token->type() != PascalTokenType::DOT) {
//        mErrorHandler->flag(token, PascalErrorCode::MISSING_PERIOD, this);
//      }
//      token = currentToken();
//      if (root_node != nullptr) {
//        mICode->setRoot(std::move(root_node));
//      }
      if (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
        const QString name = token->text().toLower();
        auto symbol_table_stack = dynamic_cast<SymbolTableStackImpl*>(mSymbolTableStack.get());
        auto entry = symbol_table_stack->lookup(name);
        if (entry == nullptr) {
          entry = symbol_table_stack->enterLocal(name);
        }
        entry->appendLineNumber(token->lineNum());
      }
      pascalTokenMessage(token->lineNum(), token->position(),
                         token->type(), token->text(), token->value());
    } else {
      mErrorHandler->flag(token, PascalErrorCode(token->value().toInt()), this);
    }
    token = nextToken();
  }
  const int endTime = QTime::currentTime().msec();
  const float elapsedTime = (endTime - startTime) / 1000.0;
  parserSummary(token->lineNum(), errorCount(), elapsedTime);
}

int PascalParserTopDown::errorCount() {
  return mErrorHandler->errorCount();
}

PascalScanner::PascalScanner() : Scanner(nullptr) {}

PascalScanner::PascalScanner(std::shared_ptr<Source> source)
    : Scanner(source) {}

PascalScanner::~PascalScanner() {
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
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
    qDebug() << "Reach EOF\n";
  } else if (std::isalpha(current_char)) {
    token = std::make_unique<PascalWordToken>(mSource);
  } else if (std::isdigit(current_char)) {
    token = std::make_unique<PascalNumberToken>(mSource);
  } else if (current_char == '\'') {
    token = std::make_unique<PascalStringToken>(mSource);
  } else if (mSpecialSymbolsMapRev.find(QString{current_char}) !=
             mSpecialSymbolsMapRev.end()) {
    token = std::make_unique<PascalSpecialSymbolToken>(mSource);
  } else {
    token = std::make_unique<PascalErrorToken>(
        mSource, PascalErrorCode::INVALID_CHARACTER, QString{current_char});
    nextChar();
  }
  return std::move(token);
}

void PascalScanner::skipWhiteSpace() {
  auto current_char = currentChar();
  // isWhiteSpace in Java also checks tabulation
  while (std::isspace(current_char) ||
         current_char == '{') {
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

std::unique_ptr<PascalParserTopDown> createPascalParser(
    const QString &language, const QString &type, std::shared_ptr<Source> source) {
  if ((language.compare("Pascal", Qt::CaseInsensitive) == 0) &&
      (type.compare("top-down", Qt::CaseInsensitive) == 0)) {
    std::unique_ptr<PascalScanner> scanner = std::make_unique<PascalScanner>(source);
    return std::make_unique<PascalParserTopDown>(std::move(scanner));
  } else if (language.compare("Pascal", Qt::CaseInsensitive) != 0) {
    qDebug() << "Invalid language: " << language;
    return nullptr;
  } else {
    qDebug() << "Invalid type: " << type;
    return nullptr;
  }
}

PascalErrorHandler::PascalErrorHandler() {
  mErrorCount = 0;
}

PascalErrorHandler::~PascalErrorHandler() {
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

void PascalErrorHandler::flag(const std::shared_ptr<PascalToken> &token,
                              PascalErrorCode errorCode, PascalParserTopDown *parser) {
  parser->syntaxErrorMessage(
      token->lineNum(), token->position(), token->text(), errorMessageMap[errorCode]);
  if (++mErrorCount > maxError) {
    abortTranslation(PascalErrorCode::TOO_MANY_ERRORS, parser);
  }
}

void PascalErrorHandler::abortTranslation(PascalErrorCode errorCode,
                                          PascalParserTopDown *parser) {
  const QString fatalText = "FATAL ERROR: " + errorMessageMap[errorCode];
  PascalParserTopDown *pascalParser =
      dynamic_cast<PascalParserTopDown *>(parser);
  pascalParser->syntaxErrorMessage(0, 0, "", fatalText);
  QCoreApplication::exit(int(errorCode));
}

int PascalErrorHandler::errorCount() const { return mErrorCount; }

std::map<PascalErrorCode, QString> initErrorMessageMap() {
  std::map<PascalErrorCode, QString> errorMessageMap;
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

std::map<PascalErrorCode, QString>
    PascalErrorHandler::errorMessageMap = initErrorMessageMap();

QString typeToStr(const PascalTokenTypeImpl &tokenType, bool *ok)
{
  const auto s1 = mReservedWordsMap.find(tokenType);
  if (s1 != mReservedWordsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s1->second;
  }
  const auto s2 = mSpecialSymbolsMap.find(tokenType);
  if (s2 != mSpecialSymbolsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s2->second;
  }
  const auto s3 = mSpecialWordsMap.find(tokenType);
  if (s3 != mSpecialSymbolsMap.end()) {
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

PascalTokenTypeImpl strToType(const QString &str, bool *ok)
{
  const auto s1 = mReservedWordsMapRev.find(str.toLower());
  if (s1 != mReservedWordsMapRev.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s1->second;
  }
  const auto s2 = mSpecialSymbolsMapRev.find(str.toLower());
  if (s2 != mSpecialSymbolsMapRev.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s2->second;
  }
  const auto s3 = mSpecialWordsMapRev.find(str.toLower());
  if (s3 != mSpecialWordsMapRev.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s3->second;
  }
  if (ok != nullptr) {
    *ok = false;
  }
  return PascalTokenTypeImpl::ERROR;
}


PascalErrorToken::PascalErrorToken() : PascalToken() {
  mType = PascalTokenTypeImpl::ERROR;
}

PascalErrorToken::PascalErrorToken(std::shared_ptr<Source> source, PascalErrorCode errorCode,
                                   const QString &tokenText) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  // NOTE: C++ code should call extract() explicitly in the derived class
  this->mType = PascalTokenTypeImpl::ERROR;
  this->mValue = int(errorCode);
  this->mText = tokenText;
  PascalErrorToken::extract();
}

unique_ptr<PascalToken> PascalErrorToken::clone() const {
  return std::make_unique<PascalErrorToken>(*this);
}

void PascalErrorToken::extract() {}

PascalWordToken::PascalWordToken(std::shared_ptr<Source> source) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalWordToken::extract();
  // mValue is uninitialized!
}

unique_ptr<PascalToken> PascalWordToken::clone() const {
  return std::make_unique<PascalWordToken>(*this);
}

void PascalWordToken::extract() {
  QString s;
  auto current_char = currentChar();
  while (std::isalnum(current_char)) {
    s += current_char;
    current_char = nextChar();
  }
  mText = s;
  if (mReservedWordsMapRev.find(mText.toLower()) !=
      mReservedWordsMapRev.end()) {
    mType = mReservedWordsMapRev[mText.toLower()];
  } else {
    mType = PascalTokenTypeImpl::IDENTIFIER;
  }
}

PascalStringToken::PascalStringToken(std::shared_ptr<Source> source) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalStringToken::extract();
}

unique_ptr<PascalToken> PascalStringToken::clone() const {
  return std::make_unique<PascalStringToken>(*this);
}

void PascalStringToken::extract() {
  QString text, value;
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
        value += current_char;        // append single-quote
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
    mValue = int(PascalErrorCode::UNEXPECTED_EOF);
  }
  mText = text;
}

PascalSpecialSymbolToken::PascalSpecialSymbolToken(std::shared_ptr<Source> source) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalSpecialSymbolToken::extract();
}

unique_ptr<PascalToken> PascalSpecialSymbolToken::clone() const {
  return std::make_unique<PascalSpecialSymbolToken>(*this);
}

void PascalSpecialSymbolToken::extract() {
  auto current_char = currentChar();
  mText = QString{current_char};
  bool invalid_type = false;
//  mTypeStr = "null";
  switch (current_char) {
  // single-character special symbols
  case '+': case '-': case '*': case '/':
  case ',': case ';': case '\'': case '=':
  case '(': case ')': case '[': case ']':
  case '{': case '}': case '^': {
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
    if (current_char == '=' ||
        current_char == '>') {
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
    mValue = int(PascalErrorCode::INVALID_CHARACTER);
  }
  }
  if (!invalid_type) {
    if (mSpecialSymbolsMapRev.find(mText) !=
        mSpecialSymbolsMapRev.end()) {
      mType = mSpecialSymbolsMapRev[mText];
    }
  }
}

PascalNumberToken::PascalNumberToken(std::shared_ptr<Source> source)
{
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalNumberToken::extract();
}

unique_ptr<PascalToken> PascalNumberToken::clone() const
{
  return std::make_unique<PascalNumberToken>(*this);
}

void PascalNumberToken::extract()
{
  QString text;
  PascalNumberToken::extractNumber(text);
  mText = text;
}

void PascalNumberToken::extractNumber(QString &text)
{
  // TODO: is it possible to parse C-style numbers
  QString whole_digits, fraction_digits, exponent_digits;
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
    if (current_char == '+' ||
        current_char == '-') {
      text += current_char;
      exponent_sign = current_char;
      current_char = nextChar();
    }
    exponent_digits = PascalNumberToken::unsignedIntegerDigits(text);
  }
  if (mType == PascalTokenTypeImpl::INTEGER) {
    const qulonglong integer_value = computeIntegerValue(whole_digits);
    if (mType != PascalTokenTypeImpl::ERROR) {
      mValue = integer_value;
    }
  } else {
    const double float_value = computeFloatValue(whole_digits, fraction_digits,
                                                 exponent_digits, exponent_sign);
    if (mType != PascalTokenTypeImpl::ERROR) {
      mValue = float_value;
    }
  }
}

QString PascalNumberToken::unsignedIntegerDigits(QString &text)
{
  char current_char = currentChar();
  if (!std::isdigit(current_char)) {
    mType = PascalTokenTypeImpl::ERROR;
    mValue = int(PascalErrorCode::INVALID_NUMBER);
    return "null";
  }
  QString digits;
  while (std::isdigit(current_char)) {
    text += current_char;
    digits += current_char;
    current_char = nextChar();
  }
  return digits;
}

qulonglong PascalNumberToken::computeIntegerValue(QString &digits)
{
  // does not consume characters
  bool ok = true;
  // TODO: try to implement toInt without Qt
  const qulonglong result = digits.toULongLong(&ok);
  if (ok) {
    return result;
  } else {
    mType = PascalTokenTypeImpl::ERROR;
    // TODO: check if integer out of range
    mValue = int(PascalErrorCode::INVALID_NUMBER);
    return 0;
  }
}

double PascalNumberToken::computeFloatValue(QString &whole_digits, QString &fraction_digits,
    QString &exponent_digits, char exponent_sign)
{
  QString s = whole_digits;
  if (!fraction_digits.isEmpty()) {
    s += "." + fraction_digits;
  }
  if (!exponent_digits.isEmpty()) {
    s += 'e' + QString{exponent_sign} + exponent_digits;
  }
  bool ok = true;
  // TODO: try to implement toDouble without Qt
  const double result = s.toDouble(&ok);
  if (ok) {
    return result;
  } else {
    mType = PascalTokenTypeImpl::ERROR;
    // TODO: check if integer out of range
    mValue = int(PascalErrorCode::INVALID_NUMBER);
    return 0;
  }
}

PascalSubparserTopDownBase::PascalSubparserTopDownBase(PascalParserTopDown &pascal_parser): mPascalParser(pascal_parser)
{

}

PascalSubparserTopDownBase::~PascalSubparserTopDownBase()
{

}

std::shared_ptr<PascalToken> PascalSubparserTopDownBase::currentToken() const
{
  return mPascalParser.currentToken();
}

std::shared_ptr<PascalToken> PascalSubparserTopDownBase::nextToken()
{
  return mPascalParser.nextToken();
}

std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl> > PascalSubparserTopDownBase::getSymbolTableStack() const
{
  return mPascalParser.getSymbolTableStack();
}

std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > PascalSubparserTopDownBase::getICode() const
{
  return mPascalParser.getICode();
}

std::shared_ptr<PascalScanner> PascalSubparserTopDownBase::scanner() const
{
  return mPascalParser.scanner();
}

int PascalSubparserTopDownBase::errorCount()
{
  return mPascalParser.errorCount();
}

PascalErrorHandler *PascalSubparserTopDownBase::errorHandler()
{
  return mPascalParser.mErrorHandler;
}

PascalParserTopDown *PascalSubparserTopDownBase::currentParser()
{
  return &mPascalParser;
}
