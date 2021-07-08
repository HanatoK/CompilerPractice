#include "PascalFrontend.h"

#include <QTime>
#include <QDebug>
#include <QCoreApplication>

PascalParserTopDown::PascalParserTopDown(Scanner *scanner,
                                         QObject *parent):
  Parser(scanner, parent), mErrorHandler(nullptr)
{
  mErrorHandler = new PascalErrorHandler(this);
}

PascalParserTopDown::~PascalParserTopDown()
{
  if (mErrorHandler != nullptr) delete mErrorHandler;
}

void PascalParserTopDown::parse()
{
  auto token = std::make_unique<Token>();
  const int startTime = QTime::currentTime().msec();
  while (token->getTypeStr() != "EOF") {
    if (token->getTypeStr() != "ERROR") {
      emit tokenMessage(token->lineNum(), token->position(),
                        token->getTypeStr(), token->text(), token->value());
    } else {
      mErrorHandler->flag(token->clone(),
                          PascalErrorCode(token->value().toInt()),
                          this);
    }
    token = nextToken();
  }
  const int endTime = QTime::currentTime().msec();
  const float elapsedTime = (endTime - startTime) / 1000.0;
  emit parserSummary(token->lineNum(), errorCount(), elapsedTime);
}

int PascalParserTopDown::errorCount()
{
  return 0;
}


PascalScanner::PascalScanner(QObject *parent): Scanner(parent)
{

}

PascalScanner::PascalScanner(Source *source, QObject *parent): Scanner(source, parent)
{

}

PascalScanner::~PascalScanner()
{

}

unique_ptr<Token> PascalScanner::extractToken()
{
  unique_ptr<Token> token = nullptr;
  const auto current_char = currentChar();
  // Construct the next token
  // The current character determines the token type.
  if (current_char == QChar(EOF)) {
    token = std::make_unique<EofToken>(mSource);
  } else {
    token = std::make_unique<Token>(mSource);
  }
  return std::move(token);
}

Parser *createParser(const QString &language, const QString &type,
                     Source *source, QObject *parent)
{
  if ((language.compare("Pascal", Qt::CaseInsensitive) == 0) &&
      (type.compare("top-down", Qt::CaseInsensitive) == 0)) {
    Scanner* scanner = new PascalScanner(source);
    Parser* parser = new PascalParserTopDown(scanner, parent);
    return parser;
  } else if (language.compare("Pascal", Qt::CaseInsensitive) != 0) {
    qDebug() << "Invalid language: " << language;
    return nullptr;
  } else {
    qDebug() << "Invalid type: " << type;
    return nullptr;
  }
}

PascalErrorHandler::PascalErrorHandler(QObject* parent): QObject(parent)
{
  mErrorCount = 0;
}

PascalErrorHandler::~PascalErrorHandler()
{

}

void PascalErrorHandler::flag(unique_ptr<Token> token,
                              PascalErrorCode errorCode, Parser *parser)
{
  PascalParserTopDown* pascalParser = dynamic_cast<PascalParserTopDown*>(parser);
  emit pascalParser->syntaxErrorMessage(token->lineNum(), token->position(),
                                        token->text(), errorMessageMap[errorCode]);
  if (++mErrorCount > maxError) {
    abortTranslation(PascalErrorCode::TOO_MANY_ERRORS, parser);
  }
}

void PascalErrorHandler::abortTranslation(PascalErrorCode errorCode, Parser* parser)
{
  const QString fatalText = "FATAL ERROR: " + errorMessageMap[errorCode];
  PascalParserTopDown* pascalParser = dynamic_cast<PascalParserTopDown*>(parser);
  emit pascalParser->syntaxErrorMessage(0, 0, "", fatalText);
  QCoreApplication::exit(int(errorCode));
}

int PascalErrorHandler::errorCount() const
{
  return mErrorCount;
}

std::unordered_map<PascalErrorCode, QString> initErrorMessageMap()
{
  std::unordered_map<PascalErrorCode, QString> errorMessageMap;
  errorMessageMap[PascalErrorCode::ALREADY_FORWARDED] = "Already specified in FORWARD";
  errorMessageMap[PascalErrorCode::IDENTIFIER_REDEFINED] = "Redefined identifier";
  errorMessageMap[PascalErrorCode::IDENTIFIER_UNDEFINED] = "Undefined identifier";
  errorMessageMap[PascalErrorCode::INCOMPATIBLE_ASSIGNMENT] = "Incompatible assignment";
  errorMessageMap[PascalErrorCode::INCOMPATIBLE_TYPES] = "Incompatible types";
  errorMessageMap[PascalErrorCode::INVALID_ASSIGNMENT] = "Invalid assignment statement";
  errorMessageMap[PascalErrorCode::INVALID_CHARACTER] = "Invalid character";
  errorMessageMap[PascalErrorCode::INVALID_CONSTANT] = "Invalid constant";
  errorMessageMap[PascalErrorCode::INVALID_EXPONENT] = "Invalid exponent";
  errorMessageMap[PascalErrorCode::INVALID_EXPRESSION] = "Invalid expression";
  errorMessageMap[PascalErrorCode::INVALID_FIELD] = "Invalid field";
  errorMessageMap[PascalErrorCode::INVALID_FRACTION] = "Invalid fraction";
  errorMessageMap[PascalErrorCode::INVALID_IDENTIFIER_USAGE] = "Invalid identifier usage";
  errorMessageMap[PascalErrorCode::INVALID_INDEX_TYPE] = "Invalid index type";
  errorMessageMap[PascalErrorCode::INVALID_NUMBER] = "Invalid number";
  errorMessageMap[PascalErrorCode::INVALID_STATEMENT] = "Invalid statement";
  errorMessageMap[PascalErrorCode::INVALID_SUBRANGE_TYPE] = "Invalid subrange type";
  errorMessageMap[PascalErrorCode::INVALID_TARGET] = "Invalid assignment target";
  errorMessageMap[PascalErrorCode::INVALID_TYPE] = "Invalid type";
  errorMessageMap[PascalErrorCode::INVALID_VAR_PARM] = "Invalid VAR parameter";
  errorMessageMap[PascalErrorCode::MIN_GT_MAX] = "Min limit greater than max limit";
  errorMessageMap[PascalErrorCode::MISSING_BEGIN] = "Missing BEGIN";
  errorMessageMap[PascalErrorCode::MISSING_COLON] = "Missing :";
  errorMessageMap[PascalErrorCode::MISSING_COLON_EQUALS] = "Missing :=";
  errorMessageMap[PascalErrorCode::MISSING_COMMA] = "Missing ,";
  errorMessageMap[PascalErrorCode::MISSING_CONSTANT] = "Missing constant";
  errorMessageMap[PascalErrorCode::MISSING_DO] = "Missing DO";
  errorMessageMap[PascalErrorCode::MISSING_DOT_DOT] = "Missing ..";
  errorMessageMap[PascalErrorCode::MISSING_END] = "Missing END";
  errorMessageMap[PascalErrorCode::MISSING_EQUALS] = "Missing =";
  errorMessageMap[PascalErrorCode::MISSING_FOR_CONTROL] = "Invalid FOR control variable";
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
  errorMessageMap[PascalErrorCode::CASE_CONSTANT_REUSED] = "CASE constant reused";
  errorMessageMap[PascalErrorCode::NOT_CONSTANT_IDENTIFIER] = "Not a constant identifier";
  errorMessageMap[PascalErrorCode::NOT_RECORD_VARIABLE] = "Not a record variable";
  errorMessageMap[PascalErrorCode::NOT_TYPE_IDENTIFIER] = "Not a type identifier";
  errorMessageMap[PascalErrorCode::RANGE_INTEGER] = "Integer literal out of range";
  errorMessageMap[PascalErrorCode::RANGE_REAL] = "Real literal out of range";
  errorMessageMap[PascalErrorCode::STACK_OVERFLOW] = "Stack overflow";
  errorMessageMap[PascalErrorCode::TOO_MANY_LEVELS] = "Nesting level too deep";
  errorMessageMap[PascalErrorCode::TOO_MANY_SUBSCRIPTS] = "Too many subscripts";
  errorMessageMap[PascalErrorCode::UNEXPECTED_EOF] = "Unexpected end of file";
  errorMessageMap[PascalErrorCode::UNEXPECTED_TOKEN] = "Unexpected token";
  errorMessageMap[PascalErrorCode::UNIMPLEMENTED] = "Unimplemented feature";
  errorMessageMap[PascalErrorCode::UNRECOGNIZABLE] = "Unrecognizable input";
  errorMessageMap[PascalErrorCode::WRONG_NUMBER_OF_PARMS] = "Wrong number of actual parameters";
  errorMessageMap[PascalErrorCode::IO_ERROR] = "Object I/O error";
  errorMessageMap[PascalErrorCode::TOO_MANY_ERRORS] = "Too many syntax errors";
  return errorMessageMap;
}

std::unordered_map<PascalErrorCode, QString> PascalErrorHandler::errorMessageMap = initErrorMessageMap();

PascalToken::PascalToken(): Token()
{
//  initTextTokenMap();
}

PascalToken::PascalToken(Source *source): Token(source)
{
//  initTextTokenMap();
}

QString PascalToken::getTypeStr() const
{
  return "";
}

unique_ptr<Token> PascalToken::clone() const
{
  return std::make_unique<PascalToken>(*this);
}

std::unordered_map<QString, PascalTokenType> initReservedWordsMap()
{
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

std::unordered_map<QString, PascalTokenType> PascalToken::mReservedWordsMap = initReservedWordsMap();
std::unordered_map<QString, PascalTokenType> PascalToken::mSpecialSymbolsMap = initSpecialSymbolsMap();
