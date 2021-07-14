#include "PascalFrontend.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTime>

PascalParserTopDown::PascalParserTopDown(Scanner *scanner, QObject *parent)
    : Parser(scanner, parent), mErrorHandler(nullptr) {
  mErrorHandler = new PascalErrorHandler(this);
}

PascalParserTopDown::~PascalParserTopDown() {
  if (mErrorHandler != nullptr)
    delete mErrorHandler;
}

void PascalParserTopDown::parse() {
  auto token = std::make_unique<Token>();
  const int startTime = QTime::currentTime().msec();
  while (token->getTypeStr() != "EOF") {
    if (token->getTypeStr() == "ERROR" || token->getTypeStr() == "UNKNOWN") {
      mErrorHandler->flag(token->clone(),
                          PascalErrorCode(token->value().toInt()), this);
    } else {
      emit tokenMessage(token->lineNum(), token->position(),
                        token->getTypeStr(), token->text(), token->value());
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
  } else if (PascalToken::mSpecialSymbolsMap.find(QString(current_char)) !=
             PascalToken::mSpecialSymbolsMap.end()) {
    token = std::make_unique<PascalSpecialSymbolToken>(mSource);
  } else {
    token = std::make_unique<PascalErrorToken>(
        mSource, PascalErrorCode::INVALID_CHARACTER, QString(current_char));
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

Parser *createParser(const QString &language, const QString &type,
                     Source *source, QObject *parent) {
  if ((language.compare("Pascal", Qt::CaseInsensitive) == 0) &&
      (type.compare("top-down", Qt::CaseInsensitive) == 0)) {
    Scanner *scanner = new PascalScanner(source);
    Parser *parser = new PascalParserTopDown(scanner, parent);
    return parser;
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
  PascalParserTopDown *pascalParser =
      dynamic_cast<PascalParserTopDown *>(parser);
  emit pascalParser->syntaxErrorMessage(token->lineNum(), token->position(),
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

std::unordered_map<PascalErrorCode, QString> initErrorMessageMap() {
  std::unordered_map<PascalErrorCode, QString> errorMessageMap;
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

std::unordered_map<PascalErrorCode, QString>
    PascalErrorHandler::errorMessageMap = initErrorMessageMap();

PascalToken::PascalToken() : Token() { mTypeStr = "UNKNOWN"; }

PascalToken::PascalToken(Source *source) : Token(source) {
  mTypeStr = "UNKNOWN";
}

QString PascalToken::getTypeStr() const { return mTypeStr; }

unique_ptr<Token> PascalToken::clone() const {
  return std::make_unique<PascalToken>(*this);
}

std::unordered_map<QString, PascalTokenType> initReservedWordsMap() {
  std::unordered_map<QString, PascalTokenType> reservedWordsMap;
  // reserved words
  reservedWordsMap["and"] = PascalTokenType::AND;
  reservedWordsMap["array"] = PascalTokenType::ARRAY;
  reservedWordsMap["begin"] = PascalTokenType::BEGIN;
  reservedWordsMap["case"] = PascalTokenType::CASE;
  reservedWordsMap["const"] = PascalTokenType::CONST;
  reservedWordsMap["div"] = PascalTokenType::DIV;
  reservedWordsMap["do"] = PascalTokenType::DO;
  reservedWordsMap["downto"] = PascalTokenType::DOWNTO;
  reservedWordsMap["else"] = PascalTokenType::ELSE;
  reservedWordsMap["end"] = PascalTokenType::END;
  reservedWordsMap["file"] = PascalTokenType::FILE;
  reservedWordsMap["for"] = PascalTokenType::FOR;
  reservedWordsMap["function"] = PascalTokenType::FUNCTION;
  reservedWordsMap["goto"] = PascalTokenType::GOTO;
  reservedWordsMap["if"] = PascalTokenType::IF;
  reservedWordsMap["in"] = PascalTokenType::IN;
  reservedWordsMap["label"] = PascalTokenType::LABEL;
  reservedWordsMap["mod"] = PascalTokenType::MOD;
  reservedWordsMap["nil"] = PascalTokenType::NIL;
  reservedWordsMap["not"] = PascalTokenType::NOT;
  reservedWordsMap["of"] = PascalTokenType::OF;
  reservedWordsMap["or"] = PascalTokenType::OR;
  reservedWordsMap["packed"] = PascalTokenType::PACKED;
  reservedWordsMap["procedure"] = PascalTokenType::PROCEDURE;
  reservedWordsMap["program"] = PascalTokenType::PROGRAM;
  reservedWordsMap["record"] = PascalTokenType::RECORD;
  reservedWordsMap["repeat"] = PascalTokenType::REPEAT;
  reservedWordsMap["set"] = PascalTokenType::SET;
  reservedWordsMap["then"] = PascalTokenType::THEN;
  reservedWordsMap["to"] = PascalTokenType::TO;
  reservedWordsMap["type"] = PascalTokenType::TYPE;
  reservedWordsMap["until"] = PascalTokenType::UNTIL;
  reservedWordsMap["var"] = PascalTokenType::VAR;
  reservedWordsMap["while"] = PascalTokenType::WHILE;
  reservedWordsMap["with"] = PascalTokenType::WITH;
  reservedWordsMap["identifier"] = PascalTokenType::IDENTIFIER;
  reservedWordsMap["integer"] = PascalTokenType::INTEGER;
  reservedWordsMap["real"] = PascalTokenType::REAL;
  reservedWordsMap["string"] = PascalTokenType::STRING;
  reservedWordsMap["error"] = PascalTokenType::ERROR;
  reservedWordsMap["end_of_file"] = PascalTokenType::END_OF_FILE;
  return reservedWordsMap;
}

std::unordered_map<QString, PascalTokenType> initSpecialSymbolsMap() {
  std::unordered_map<QString, PascalTokenType> specialSymbolsMap;
  // special symbols
  specialSymbolsMap["+"] = PascalTokenType::PLUS;
  specialSymbolsMap["-"] = PascalTokenType::MINUS;
  specialSymbolsMap["*"] = PascalTokenType::STAR;
  specialSymbolsMap["/"] = PascalTokenType::SLASH;
  specialSymbolsMap[":="] = PascalTokenType::COLON_EQUALS;
  specialSymbolsMap["."] = PascalTokenType::DOT;
  specialSymbolsMap[","] = PascalTokenType::COMMA;
  specialSymbolsMap[";"] = PascalTokenType::SEMICOLON;
  specialSymbolsMap[":"] = PascalTokenType::COLON;
  specialSymbolsMap["'"] = PascalTokenType::QUOTE;
  specialSymbolsMap["="] = PascalTokenType::EQUALS;
  specialSymbolsMap["<>"] = PascalTokenType::NOT_EQUALS;
  specialSymbolsMap["<"] = PascalTokenType::LESS_THAN;
  specialSymbolsMap["<="] = PascalTokenType::LESS_EQUALS;
  specialSymbolsMap[">="] = PascalTokenType::GREATER_EQUALS;
  specialSymbolsMap[">"] = PascalTokenType::GREATER_THAN;
  specialSymbolsMap["("] = PascalTokenType::LEFT_PAREN;
  specialSymbolsMap[")"] = PascalTokenType::RIGHT_PAREN;
  specialSymbolsMap["["] = PascalTokenType::LEFT_BRACKET;
  specialSymbolsMap["]"] = PascalTokenType::RIGHT_BRACKET;
  specialSymbolsMap["{"] = PascalTokenType::LEFT_BRACE;
  specialSymbolsMap["}"] = PascalTokenType::RIGHT_BRACE;
  specialSymbolsMap["^"] = PascalTokenType::UP_ARROW;
  specialSymbolsMap[".."] = PascalTokenType::DOT_DOT;
  return specialSymbolsMap;
}

std::unordered_map<QString, PascalTokenType> PascalToken::mReservedWordsMap =
    initReservedWordsMap();
std::unordered_map<QString, PascalTokenType> PascalToken::mSpecialSymbolsMap =
    initSpecialSymbolsMap();

PascalErrorToken::PascalErrorToken() : PascalToken() { mTypeStr = "ERROR"; }

PascalErrorToken::PascalErrorToken(Source *source, PascalErrorCode errorCode,
                                   const QString &tokenText) {
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  // NOTE: C++ code should call extract() explicitly in the derived class
  this->mTypeStr = "ERROR";
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
  if (PascalToken::mReservedWordsMap.find(mText) !=
      PascalToken::mReservedWordsMap.end()) {
    mTypeStr = mText;
  } else {
    mTypeStr = "identifier";
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
  auto current_char = currentChar();
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
        current_char = currentChar(); // consume pair of quotes
        current_char = currentChar();
      }
    }
  } while ((current_char != '\'') && (current_char != EOF));
  if (current_char == '\'') {
    nextChar(); // consume the final quote
    text += '\'';
    mTypeStr = "string";
    mValue = value;
  } else {
    mTypeStr = "error";
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
  mTypeStr = "null";
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
    mTypeStr = "error";
    mValue = int(PascalErrorCode::INVALID_CHARACTER);
  }
  }
  if (mTypeStr == "null") {
    if (PascalToken::mSpecialSymbolsMap.find(mText) !=
        PascalToken::mSpecialSymbolsMap.end()) {
      mTypeStr = mText;
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
  // TODO
}
