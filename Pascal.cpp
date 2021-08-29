#include "Pascal.h"
#include "Utilities.h"

#include <iostream>
#include <fmt/format.h>

Pascal::Pascal(const std::string &operation, const std::string &filePath,
               const std::string &flags, QObject *parent)
    : mParser(nullptr),
      mSource(nullptr), mICode(nullptr), mSymbolTableStack(nullptr),
      mBackend(nullptr), mSourceFile(nullptr), mTextStream(nullptr),
      QObject(parent) {
  // what are these flags??
  //  const bool intermediate = flags.indexOf('i') > -1;
//  const bool xref = flags.indexOf('x') > -1;
  auto search_xref = flags.find('x');
  const bool xref = (search_xref == std::string::npos) ? false : true;
  mSourceFile = new QFile(filePath.c_str(), nullptr);
  mSourceFile->open(QIODevice::ReadOnly);
  mTextStream = new QTextStream(mSourceFile);
  mSource = std::make_shared<Source>(*mTextStream);
  mParser = createPascalParser("Pascal", "top-down", mSource);
  mSource->sendMessage.connect(std::bind(&Pascal::sourceMessage, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  mParser->parserSummary.connect(
      std::bind(&Pascal::parserSummary, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  mParser->pascalTokenMessage.connect(std::bind(
      &Pascal::tokenMessage, this, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
  mParser->syntaxErrorMessage.connect(std::bind(
      &Pascal::syntaxErrorMessage, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  mBackend = createBackend(operation);
  const std::string backend_type = mBackend->getType();
  if (boost::iequals(backend_type, "compiler")) {
    auto backend_ptr = dynamic_cast<Compiler::CodeGenerator *>(mBackend.get());
    backend_ptr->summary.connect(std::bind(&Pascal::compilerSummary, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
  } else if (boost::iequals(backend_type, "interpreter")) {
    auto backend_ptr = dynamic_cast<Interpreter::Executor *>(mBackend.get());
    backend_ptr->summary.connect(
        std::bind(&Pascal::interpreterSummary, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
  }
  mParser->parse();
  if (xref) {
    CrossReferencer::print(mParser->getSymbolTableStack());
  }
  mICode = mParser->getICode();
  mSymbolTableStack = mParser->getSymbolTableStack();
  mBackend->process(mICode, mSymbolTableStack);
}

Pascal::~Pascal() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
  if (mTextStream != nullptr) {
    delete mTextStream;
    mTextStream = nullptr;
  }
  if (mSourceFile != nullptr) {
    delete mSourceFile;
    mSourceFile = nullptr;
  }
//  if (mSource != nullptr) {
//    //    delete mSource;
//    //    mSource = nullptr;
//    qDebug() << "Shared pointer mSource use count: " << mSource.use_count();
//  }
  //  if (mParser != nullptr) {
  //    delete mParser;
  //    mParser = nullptr;
  //  }
}

void Pascal::sourceMessage(int lineNumber, std::string line) {
  fmt::print("{:03d} {}\n", lineNumber, line);
}

void Pascal::parserSummary(int lineNumber, int errorCount, float elapsedTime) {
  fmt::print("\n{:10d} source lines.", lineNumber);
  fmt::print("\n{:10d} syntax errors.", errorCount);
  fmt::print("\n{:10.5f} seconds total parsing time.\n\n", elapsedTime);
}

void Pascal::compilerSummary(int instructionCount, float elapsedTime) {
  fmt::print("\n{:10d} instructions generated.", instructionCount);
  fmt::print("\n{:10.5f} seconds total code generation time.\n\n", elapsedTime);
}

void Pascal::interpreterSummary(int executionCount, int runtimeErrors,
                                float elapsedTime) {
  fmt::print("\n{:10d} statements executed.", executionCount);
  fmt::print("\n{:10d} runtime errors.", runtimeErrors);
  fmt::print("\n{:10.5f} seconds total execution time.\n\n", elapsedTime);
}

void Pascal::tokenMessage(int lineNumber, int position,
                          PascalTokenTypeImpl tokenType, std::string text,
                          std::any value) {
  const auto type_str = typeToStr(tokenType);
  fmt::print(">>> {:->15s} line = {:05d}, pos = {:3d}, text = {}",
             type_str, lineNumber, position, text);
  if (value.has_value()) {
    fmt::print(", value = {}", any_to_string(value));
  }
  fmt::print("\n");
}

void Pascal::syntaxErrorMessage(int lineNumber, int position, std::string text,
                                std::string error) {
  const int prefix_width = 4;
  const int space_count = prefix_width + position;
  fmt::print("{: >{}}^\n", "", space_count);
  fmt::print("***{} [at \"{}\"]\n", error, text);
}
