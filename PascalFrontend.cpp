#include "PascalFrontend.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTime>

// assume no key shares a same value
template <typename T1, typename T2>
std::map<T2, T1> reverse_map(const std::map<T1, T2>& map_in) {
  std::map<T2, T1> map_out;
  for (auto it = map_in.begin(); it != map_in.end(); ++it) {
    map_out[it->second] = it->first;
  }
  return map_out;
}

PascalParserTopDown::PascalParserTopDown(Scanner *scanner, QObject *parent)
    : Parser(scanner, parent), mErrorHandler(nullptr) {
  mErrorHandler = new PascalErrorHandler(this);
}

PascalParserTopDown::~PascalParserTopDown() {
  if (mErrorHandler != nullptr)
    delete mErrorHandler;
}

void PascalParserTopDown::parse() {
  auto token = nextToken();
  const int startTime = QTime::currentTime().msec();
  while (token->getTypeStr() != "EOF") {
    if ((token->getTypeStr().compare("error", Qt::CaseInsensitive) == 0) ||
        (token->getTypeStr().compare("unknown", Qt::CaseInsensitive) == 0)) {
      mErrorHandler->flag(token->clone(),
                          PascalErrorCode(token->value().toInt()), this);
    } else {
      const auto pascal_token = dynamic_cast<PascalToken*>(token.get());
      emit pascalTokenMessage(token->lineNum(), token->position(),
                              pascal_token->type(), token->text(), token->value());
    }
    token = nextToken();
  }
  const int endTime = QTime::currentTime().msec();
  const float elapsedTime = (endTime - startTime) / 1000.0;
  emit parserSummary(token->lineNum(), errorCount(), elapsedTime);
}

int PascalParserTopDown::errorCount() { return 0; }

PascalScanner::PascalScanner(QObject *parent) : Scanner(parent) {}

PascalScanner::PascalScanner(Source *source, QObject *parent)
    : Scanner(source, parent) {}

PascalScanner::~PascalScanner() {}

unique_ptr<Token> PascalScanner::extractToken() {
  skipWhiteSpace();
  unique_ptr<Token> token = nullptr;
  auto current_char = currentChar();
  // Construct the next token
  // The current character determines the token type.
  if (current_char == EOF) {
    token = std::make_unique<EofToken>(mSource);
  } else if (current_char.isLetter()) {
    token = std::make_unique<PascalWordToken>(mSource);
  } else if (current_char.isDigit()) {
    token = std::make_unique<PascalNumberToken>(mSource);
  } else if (current_char == '\'') {
    token = std::make_unique<PascalStringToken>(mSource);
  } else if (PascalToken::mSpecialSymbolsMapRev.find(QString(current_char)) !=
             PascalToken::mSpecialSymbolsMapRev.end()) {
    token = std::make_unique<PascalSpecialSymbolToken>(mSource);
  } else {
    token = std::make_unique<PascalErrorToken>(
        mSource, PascalErrorCode::INVALID_CHARACTER, QString(current_char));
    nextChar();
  }
  return std::move(token);
}

void PascalScanner::skipWhiteSpace() {
  auto current_char = currentChar();
  // isWhiteSpace in Java also checks tabulation
  while (current_char.isSpace() || current_char == QChar::Tabulation ||
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

PascalParserTopDown *createPascalParser(
  const QString &language, const QString &type,
  Source *source, QObject *parent) {
  if ((language.compare("Pascal", Qt::CaseInsensitive) == 0) &&
      (type.compare("top-down", Qt::CaseInsensitive) == 0)) {
    Scanner *scanner = new PascalScanner(source);
    return new PascalParserTopDown(scanner, parent);
  } else if (language.compare("Pascal", Qt::CaseInsensitive) != 0) {
    qDebug() << "Invalid language: " << language;
    return nullptr;
  } else {
    qDebug() << "Invalid type: " << type;
    return nullptr;
  }
}

PascalErrorHandler::PascalErrorHandler(QObject *parent) : QObject(parent) {
  mErrorCount = 0;
}

PascalErrorHandler::~PascalErrorHandler() {}

void PascalErrorHandler::flag(unique_ptr<Token> token,
                              PascalErrorCode errorCode, Parser *parser) {
  emit parser->syntaxErrorMessage(token->lineNum(), token->position(),
                                  token->text(),
                                  errorMessageMap[errorCode]);
  if (++mErrorCount > maxError) {
    abortTranslation(PascalErrorCode::TOO_MANY_ERRORS, parser);
  }
}

void PascalErrorHandler::abortTranslation(PascalErrorCode errorCode,
                                          Parser *parser) {
  const QString fatalText = "FATAL ERROR: " + errorMessageMap[errorCode];
  PascalParserTopDown *pascalParser =
      dynamic_cast<PascalParserTopDown *>(parser);
  emit pascalParser->syntaxErrorMessage(0, 0, "", fatalText);
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

PascalToken::PascalToken() : Token() { mType = PascalTokenType::ERROR; }

PascalToken::PascalToken(Source *source) : Token(source) {
  mType = PascalTokenType::ERROR;
}

QString PascalToken::getTypeStr() const {
  return PascalToken::typeToStr(mType, nullptr);
}

unique_ptr<Token> PascalToken::clone() const {
  return std::make_unique<PascalToken>(*this);
}

QString PascalToken::typeToStr(const PascalTokenType &tokenType, bool *ok)
{
  const auto s1 = PascalToken::mReservedWordsMap.find(tokenType);
  if (s1 != PascalToken::mReservedWordsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s1->second;
  }
  const auto s2 = PascalToken::mSpecialSymbolsMap.find(tokenType);
  if (s2 != PascalToken::mSpecialSymbolsMap.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s2->second;
  }
  if (ok != nullptr) {
    *ok = false;
  }
  return "UNKNOWN";
}

PascalTokenType PascalToken::strToType(const QString &str, bool *ok)
{
  const auto s1 = PascalToken::mReservedWordsMapRev.find(str);
  if (s1 != PascalToken::mReservedWordsMapRev.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s1->second;
  }
  const auto s2 = PascalToken::mSpecialSymbolsMapRev.find(str);
  if (s2 != PascalToken::mSpecialSymbolsMapRev.end()) {
    if (ok != nullptr) {
      *ok = true;
    }
    return s2->second;
  }
  if (ok != nullptr) {
    *ok = false;
  }
  return PascalTokenType::ERROR;
}

PascalTokenType PascalToken::type() const
{
  return mType;
}

std::map<PascalTokenType, QString> initReservedWordsMap() {
  std::map<PascalTokenType, QString> reservedWordsMap;
  // reserved words
  reservedWordsMap[PascalTokenType::AND] = QString("and");
  reservedWordsMap[PascalTokenType::ARRAY] = QString("array");
  reservedWordsMap[PascalTokenType::BEGIN] = QString("begin");
  reservedWordsMap[PascalTokenType::CASE] = QString("case");
  reservedWordsMap[PascalTokenType::CONST] = QString("const");
  reservedWordsMap[PascalTokenType::DIV] = QString("div");
  reservedWordsMap[PascalTokenType::DO] = QString("do");
  reservedWordsMap[PascalTokenType::DOWNTO] = QString("downto");
  reservedWordsMap[PascalTokenType::ELSE] = QString("else");
  reservedWordsMap[PascalTokenType::END] = QString("end");
  reservedWordsMap[PascalTokenType::FILE] = QString("file");
  reservedWordsMap[PascalTokenType::FOR] = QString("for");
  reservedWordsMap[PascalTokenType::FUNCTION] = QString("function");
  reservedWordsMap[PascalTokenType::GOTO] = QString("goto");
  reservedWordsMap[PascalTokenType::IF] = QString("if");
  reservedWordsMap[PascalTokenType::IN] = QString("in");
  reservedWordsMap[PascalTokenType::LABEL] = QString("label");
  reservedWordsMap[PascalTokenType::MOD] = QString("mod");
  reservedWordsMap[PascalTokenType::NIL] = QString("nil");
  reservedWordsMap[PascalTokenType::NOT] = QString("not");
  reservedWordsMap[PascalTokenType::OF] = QString("of");
  reservedWordsMap[PascalTokenType::OR] = QString("or");
  reservedWordsMap[PascalTokenType::PACKED] = QString("packed");
  reservedWordsMap[PascalTokenType::PROCEDURE] = QString("procedure");
  reservedWordsMap[PascalTokenType::PROGRAM] = QString("program");
  reservedWordsMap[PascalTokenType::RECORD] = QString("record");
  reservedWordsMap[PascalTokenType::REPEAT] = QString("repeat");
  reservedWordsMap[PascalTokenType::SET] = QString("set");
  reservedWordsMap[PascalTokenType::THEN] = QString("then");
  reservedWordsMap[PascalTokenType::TO] = QString("to");
  reservedWordsMap[PascalTokenType::TYPE] = QString("type");
  reservedWordsMap[PascalTokenType::UNTIL] = QString("until");
  reservedWordsMap[PascalTokenType::VAR] = QString("var");
  reservedWordsMap[PascalTokenType::WHILE] = QString("while");
  reservedWordsMap[PascalTokenType::WITH] = QString("with");
  reservedWordsMap[PascalTokenType::IDENTIFIER] = QString("identifier");
  reservedWordsMap[PascalTokenType::INTEGER] = QString("integer");
  reservedWordsMap[PascalTokenType::REAL] = QString("real");
  reservedWordsMap[PascalTokenType::STRING] = QString("string");
  reservedWordsMap[PascalTokenType::ERROR] = QString("error");
  reservedWordsMap[PascalTokenType::END_OF_FILE] = QString("end_of_file");
  return reservedWordsMap;
}

std::map<PascalTokenType, QString> initSpecialSymbolsMap() {
  std::map<PascalTokenType, QString> specialSymbolsMap;
  // special symbols
  specialSymbolsMap[PascalTokenType::PLUS]            = QString("+");
  specialSymbolsMap[PascalTokenType::MINUS]           = QString("-");
  specialSymbolsMap[PascalTokenType::STAR]            = QString("*");
  specialSymbolsMap[PascalTokenType::SLASH]           = QString("/");
  specialSymbolsMap[PascalTokenType::COLON_EQUALS]    = QString(":=");
  specialSymbolsMap[PascalTokenType::DOT]             = QString(".");
  specialSymbolsMap[PascalTokenType::COMMA]           = QString(",");
  specialSymbolsMap[PascalTokenType::SEMICOLON]       = QString(";");
  specialSymbolsMap[PascalTokenType::COLON]           = QString(":");
  specialSymbolsMap[PascalTokenType::QUOTE]           = QString("'");
  specialSymbolsMap[PascalTokenType::EQUALS]          = QString("=");
  specialSymbolsMap[PascalTokenType::NOT_EQUALS]      = QString("<>");
  specialSymbolsMap[PascalTokenType::LESS_THAN]       = QString("<");
  specialSymbolsMap[PascalTokenType::LESS_EQUALS]     = QString("<=");
  specialSymbolsMap[PascalTokenType::GREATER_EQUALS]  = QString(">=");
  specialSymbolsMap[PascalTokenType::GREATER_THAN]    = QString(">");
  specialSymbolsMap[PascalTokenType::LEFT_PAREN]      = QString("(");
  specialSymbolsMap[PascalTokenType::RIGHT_PAREN]     = QString(")");
  specialSymbolsMap[PascalTokenType::LEFT_BRACKET]    = QString("[");
  specialSymbolsMap[PascalTokenType::RIGHT_BRACKET]   = QString("]");
  specialSymbolsMap[PascalTokenType::LEFT_BRACE]      = QString("{");
  specialSymbolsMap[PascalTokenType::RIGHT_BRACE]     = QString("}");
  specialSymbolsMap[PascalTokenType::UP_ARROW]        = QString("^");
  specialSymbolsMap[PascalTokenType::DOT_DOT]         = QString("..");
  return specialSymbolsMap;
}

std::map<PascalTokenType, QString> PascalToken::mReservedWordsMap = initReservedWordsMap();
std::map<PascalTokenType, QString> PascalToken::mSpecialSymbolsMap = initSpecialSymbolsMap();
std::map<QString, PascalTokenType> PascalToken::mReservedWordsMapRev = reverse_map(PascalToken::mReservedWordsMap);
std::map<QString, PascalTokenType> PascalToken::mSpecialSymbolsMapRev = reverse_map(PascalToken::mSpecialSymbolsMap);

PascalErrorToken::PascalErrorToken() : PascalToken() {
  mType = PascalTokenType::ERROR;
}

PascalErrorToken::PascalErrorToken(Source *source, PascalErrorCode errorCode,
                                   const QString &tokenText) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  // NOTE: C++ code should call extract() explicitly in the derived class
  this->mType = PascalTokenType::ERROR;
  this->mValue = int(errorCode);
  this->mText = tokenText;
  PascalErrorToken::extract();
}

unique_ptr<Token> PascalErrorToken::clone() const {
  return std::make_unique<PascalErrorToken>(*this);
}

void PascalErrorToken::extract() {}

PascalWordToken::PascalWordToken(Source *source) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalWordToken::extract();
  // mValue is uninitialized!
}

unique_ptr<Token> PascalWordToken::clone() const {
  return std::make_unique<PascalWordToken>(*this);
}

void PascalWordToken::extract() {
  QString s;
  auto current_char = currentChar();
  while (current_char.isLetterOrNumber()) {
    s += current_char;
    current_char = nextChar();
  }
  mText = s;
  if (PascalToken::mReservedWordsMapRev.find(mText) !=
      PascalToken::mReservedWordsMapRev.end()) {
    mType = mReservedWordsMapRev[mText];
  } else {
    mType = PascalTokenType::IDENTIFIER;
  }
}

PascalStringToken::PascalStringToken(Source *source) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalStringToken::extract();
}

unique_ptr<Token> PascalStringToken::clone() const {
  return std::make_unique<PascalStringToken>(*this);
}

void PascalStringToken::extract() {
  QString text, value;
  auto current_char = nextChar();
  text += '\'';
  do {
    // replace any whitespace character with a blank (why?)
    if (current_char.isSpace() || current_char == QChar::Tabulation) {
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
    mType = PascalTokenType::STRING;
    mValue = value;
  } else {
    mType = PascalTokenType::ERROR;
    mValue = int(PascalErrorCode::UNEXPECTED_EOF);
  }
  mText = text;
}

PascalSpecialSymbolToken::PascalSpecialSymbolToken(Source *source) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalSpecialSymbolToken::extract();
}

unique_ptr<Token> PascalSpecialSymbolToken::clone() const {
  return std::make_unique<PascalSpecialSymbolToken>(*this);
}

void PascalSpecialSymbolToken::extract() {
  auto current_char = currentChar();
  mText = QString(current_char);
  bool invalid_type = false;
//  mTypeStr = "null";
  switch (current_char.unicode()) {
  // single-character special symbols
  case u'+': case u'-': case u'*': case u'/':
  case u',': case u';': case u'\'': case u'=':
  case u'(': case u')': case u'[': case u']':
  case u'{': case u'}': case u'^': {
    nextChar();
    break;
  }
  // : or :=
  case u':': {
    current_char = nextChar(); // consume ':'
    if (current_char == '=') {
      mText += current_char;
      nextChar(); // consume '='
    }
    break;
  }
  // < or <= or <>
  case u'<': {
    current_char = nextChar(); // consume '>'
    if (current_char == '=' ||
        current_char == '>') {
      mText += current_char;
      nextChar(); // consume '=' or '>'
    }
    break;
  }
  // > or >=
  case u'>': {
    current_char = nextChar(); // consume '>'
    if (current_char == '=') {
      mText += current_char;
      nextChar(); // consume '='
    }
    break;
  }
  // . or ..
  case u'.': {
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
    mType = PascalTokenType::ERROR;
    mValue = int(PascalErrorCode::INVALID_CHARACTER);
  }
  }
  if (!invalid_type) {
    if (PascalToken::mSpecialSymbolsMapRev.find(mText) !=
        PascalToken::mSpecialSymbolsMapRev.end()) {
      mType = PascalToken::mSpecialSymbolsMapRev[mText];
    }
  }
}

PascalNumberToken::PascalNumberToken(Source *source)
{
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  PascalNumberToken::extract();
}

unique_ptr<Token> PascalNumberToken::clone() const
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
  QChar exponent_sign = '+';
  bool dot_dot = false;
  QChar current_char;
  mType = PascalTokenType::INTEGER;
  whole_digits = PascalNumberToken::unsignedIntegerDigits(text);
  if (mType == PascalTokenType::ERROR) {
    return;
  }
  current_char = currentChar();
  if (current_char == '.') {
    if (peekChar() == '.') {
      dot_dot = true;
    } else {
      mType = PascalTokenType::REAL;
      text += current_char;
      current_char = nextChar();
      if (!peekChar().isSpace()) {
        fraction_digits = PascalNumberToken::unsignedIntegerDigits(text);
      }
      if (mType == PascalTokenType::ERROR) {
        return;
      }
    }
  }
  current_char = currentChar();
  if (!dot_dot && (current_char == 'E' || current_char == 'e')) {
    mType = PascalTokenType::REAL;
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
  if (mType == PascalTokenType::INTEGER) {
    const qulonglong integer_value = computeIntegerValue(whole_digits);
    if (mType != PascalTokenType::ERROR) {
      mValue = integer_value;
    }
  } else {
    const double float_value = computeFloatValue(whole_digits, fraction_digits,
                                                 exponent_digits, exponent_sign);
    if (mType != PascalTokenType::ERROR) {
      mValue = float_value;
    }
  }
}

QString PascalNumberToken::unsignedIntegerDigits(QString &text)
{
  QChar current_char = currentChar();
  if (!current_char.isDigit()) {
    mType = PascalTokenType::ERROR;
    mValue = int(PascalErrorCode::INVALID_NUMBER);
    return "null";
  }
  QString digits;
  while (current_char.isDigit()) {
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
    mType = PascalTokenType::ERROR;
    // TODO: check if integer out of range
    mValue = int(PascalErrorCode::INVALID_NUMBER);
    return 0;
  }
}

double PascalNumberToken::computeFloatValue(
    QString &whole_digits, QString &fraction_digits,
    QString &exponent_digits, QChar exponent_sign)
{
  QString s = whole_digits;
  if (!fraction_digits.isEmpty()) {
    s += "." + fraction_digits;
  }
  if (!exponent_digits.isEmpty()) {
    s += 'e' + exponent_sign + exponent_digits;
  }
  bool ok = true;
  // TODO: try to implement toDouble without Qt
  const double result = s.toDouble(&ok);
  if (ok) {
    return result;
  } else {
    mType = PascalTokenType::ERROR;
    // TODO: check if integer out of range
    mValue = int(PascalErrorCode::INVALID_NUMBER);
    return 0;
  }
}
