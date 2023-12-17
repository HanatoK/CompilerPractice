#include "Pascal.h"
#include "Utilities.h"
#include "Compiler.h"
#include "Interpreter.h"

#include <iostream>
#include <fmt/format.h>
#include <source_location>

Pascal::Pascal(const std::string &operation, const std::string &filePath,
               const std::string &flags)
    : mParser(nullptr),
      mSource(nullptr), mICode(nullptr), mSymbolTableStack(nullptr),
      mBackend(nullptr), mTextStream(std::make_shared<std::ifstream>()) {
  auto search_xref = flags.find('x');
  auto search_intermediate = flags.find('i');
  const bool xref = (search_xref == std::string::npos) ? false : true;
  const bool intermediate = (search_intermediate == std::string::npos) ? false : true;
  mTextStream->open(filePath.c_str());
  if (!mTextStream->is_open()) {
    std::cerr << "Cannot open " << filePath << std::endl;
    throw std::invalid_argument("Invalid filename, please see the error above.");
  }
  mSource = std::make_shared<Source>(mTextStream);
  mParser = createPascalParser("Pascal", "top-down", mSource);
  mSource->sendMessage.connect(std::bind(&Pascal::sourceMessage, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  mParser->parserSummary.connect(
      std::bind(&Pascal::parserSummary, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  mParser->syntaxErrorMessage.connect(std::bind(
      &Pascal::syntaxErrorMessage, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  mBackend = createBackend(operation);
  const std::string backend_type = mBackend->getType();
  if (boost::iequals(backend_type, "compiler")) {
    auto backend_ptr = dynamic_cast<CodeGenerator *>(mBackend.get());
    backend_ptr->summary.connect(std::bind(&Pascal::compilerSummary, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
  } else if (boost::iequals(backend_type, "interpreter")) {
    auto backend_ptr = dynamic_cast<Executor *>(mBackend.get());
    backend_ptr->summary.connect(
        std::bind(&Pascal::interpreterSummary, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    backend_ptr->assignmentMessage.connect(std::bind(&Pascal::assignmentMessage,
                                                     this, std::placeholders::_1,
                                                     std::placeholders::_2,
                                                     std::placeholders::_3));
    backend_ptr->runtimeErrorMessage.connect(std::bind(&Pascal::runtimeErrorMessage,
                                                       this, std::placeholders::_1,
                                                       std::placeholders::_2));
  }
  mParser->parse();
  if (mParser->errorCount() == 0) {
    mSymbolTableStack = mParser->getSymbolTableStack();
    const auto program_id = mSymbolTableStack->programId();
//    mICode = program_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_ICODE>();
    auto icode_opt = program_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_ICODE>();
    if (icode_opt) {
      mICode = icode_opt.value();
    } else {
      BUG("empty val");
    }
    if (xref) {
      CrossReferencer cross_referencer;
      cross_referencer.print(mSymbolTableStack);
    }
    if (intermediate) {
      ParseTreePrinter printer_xml(std::cout);
      printer_xml.print(mSymbolTableStack);
      std::ofstream ofs_dot((filePath + ".dot").c_str());
      ParseTreePrinterDot printer(ofs_dot);
      printer.print(mSymbolTableStack);
    }
    mBackend->process(mICode, mSymbolTableStack);
  } else {
    std::cout << "Number of error(s): " << mParser->errorCount() << "\n";
    std::cout << "Error(s) encountered, will not call backend\n";
  }
}

Pascal::~Pascal() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

void Pascal::sourceMessage(const int lineNumber, const std::string &line) const {
  fmt::print("{:03d} {}\n", lineNumber, line);
}

void Pascal::parserSummary(const int lineNumber, const int errorCount, const float elapsedTime) const {
  fmt::print("\n{:10d} source lines.", lineNumber);
  fmt::print("\n{:10d} syntax errors.", errorCount);
  fmt::print("\n{:10.5f} seconds total parsing time.\n\n", elapsedTime);
}

void Pascal::compilerSummary(const int instructionCount, const float elapsedTime) const {
  fmt::print("\n{:10d} instructions generated.", instructionCount);
  fmt::print("\n{:10.5f} seconds total code generation time.\n\n", elapsedTime);
}

void Pascal::interpreterSummary(const int executionCount, const int runtimeErrors,
                                const float elapsedTime) const {
  fmt::print("\n{:10d} statements executed.", executionCount);
  fmt::print("\n{:10d} runtime errors.", runtimeErrors);
  fmt::print("\n{:10.5f} seconds total execution time.\n\n", elapsedTime);
}

void Pascal::syntaxErrorMessage(const int lineNumber, const int position, const std::string& text,
                                const std::string& error) const {
  const int prefix_width = 4;
  const int space_count = prefix_width + position;
  fmt::print("{: >{}}^\n", "", space_count);
  fmt::print("***{} [at \"{}\"]\n", error, text);
}

void Pascal::assignmentMessage(const int line_number, const std::string& variable_name, const VariableValueT& value) const
{
  fmt::print("LINE {:3d}: {} = {}\n", line_number, variable_name, variable_value_to_string(value));
}

void Pascal::runtimeErrorMessage(const int line_number, const std::string& error_message) const
{
  fmt::print("*** RUNTIME ERROR AT LINE {:03d} {}\n", line_number, error_message);
}
