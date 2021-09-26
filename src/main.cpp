#include "Pascal.h"
//#include <QCoreApplication>
//#include <QCommandLineParser>
//#include <QCommandLineOption>
//#include <QDebug>
//#include <QObject>

#include "CLI/CLI.hpp"

int main(int argc, char* argv[]) {
  CLI::App app{"Pascal Compiler and Interpreter"};
  std::string filename;
  bool show_parse_tree = false;
  bool show_reference_listing = false;
  app.require_subcommand(2);
  std::vector<std::string> subprogram_names{"compile", "interpret"};
  std::vector<CLI::App*> subprograms(2, nullptr);
  subprograms[0] = app.add_subcommand(subprogram_names[0], "Compile the Pascal source code");
  subprograms[1] = app.add_subcommand(subprogram_names[1], "Execute the Pascal source code");
  for (auto &i : subprograms) {
    i->add_option("-f,--filename", filename, "Input filename");
    i->add_option("-i", show_parse_tree, "Show the parse tree");
    i->add_option("-x", show_reference_listing, "Show the reference listing");
  }
  CLI11_PARSE(app, argc, argv);
  std::string flags;
  for (size_t i = 0; i < subprograms.size(); ++i) {
    if (subprograms[i]->parsed()) {
      if (show_parse_tree) flags += "i";
      if (show_reference_listing) flags += "x";
      Pascal p(subprogram_names[i], filename, flags);
      break;
    }
  }
  return 0;
}

//void initMetaTypes() {
//  qRegisterMetaType<PascalTokenTypeImpl>("PascalTokenTypeImpl");
//  qRegisterMetaType<PascalErrorCode>("PascalErrorCode");
//}

//int main(int argc, char *argv[])
//{
  // TODO: rework the handling of command line options
//  initMetaTypes();
//  QCoreApplication a(argc, argv);
//  a.setApplicationName("PascalPractice");
//  a.setApplicationVersion("0.1");
//  QCommandLineParser parser;
//  const QCommandLineOption compile_option("compile", "Compile the source file.");
//  const QCommandLineOption interpret_option("interpret", "Execute the source file directly.");
//  const QCommandLineOption reference_listing_option("x", "Generate reference listing.");
//  const QCommandLineOption intermediate_option("i", "Show the parse tree.");
//  parser.addHelpOption();
//  parser.addOption(compile_option);
//  parser.addOption(interpret_option);
//  parser.addOption(reference_listing_option);
//  parser.addOption(intermediate_option);
//  parser.addPositionalArgument("source file", "the pascal source file");
//  parser.process(a);
//  QString flags;
//  const auto& reference_listing_option_list = reference_listing_option.names();
//  if (parser.isSet(reference_listing_option)) {
//    flags += reference_listing_option_list.first();
//  }
//  const auto& intermediate_option_list = intermediate_option.names();
//  if (parser.isSet(intermediate_option)) {
//    flags += intermediate_option_list.first();
//  }
//  const QStringList files = parser.positionalArguments();
//  if (files.empty()) {
//    qWarning().noquote() << "No source file specified.";
//    return 1;
//  }
//  const QString& filePath = files.first();
//  if (parser.isSet(compile_option)) {
//    Pascal p("compile", filePath.toStdString(), flags.toStdString());
//  } else if (parser.isSet(interpret_option)) {
//    Pascal p("interpret", filePath.toStdString(), flags.toStdString());
//  } else {
//    parser.showHelp();
//    return 0;
//  }
//  return 0;
//}
