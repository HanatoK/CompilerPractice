#include "Pascal.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QObject>

void initMetaTypes() {
  qRegisterMetaType<PascalTokenTypeImpl>("PascalTokenTypeImpl");
  qRegisterMetaType<PascalErrorCode>("PascalErrorCode");
}

int main(int argc, char *argv[])
{
  // TODO: rework the handling of command line options
  initMetaTypes();
  QCoreApplication a(argc, argv);
  a.setApplicationName("PascalPractice");
  a.setApplicationVersion("0.1");
  QCommandLineParser parser;
  const QCommandLineOption compile_option("compile", "Compile the source file.");
  const QCommandLineOption interpret_option("interpret", "Execute the source file directly.");
  const QCommandLineOption reference_listing_option("x", "Generate reference listing.");
  const QCommandLineOption intermediate_option("i", "Show the parse tree.");
  parser.addHelpOption();
  parser.addOption(compile_option);
  parser.addOption(interpret_option);
  parser.addOption(reference_listing_option);
  parser.addOption(intermediate_option);
  parser.addPositionalArgument("source file", "the pascal source file");
  parser.process(a);
  Pascal* pascal = nullptr;
  QString flags;
  const auto& reference_listing_option_list = reference_listing_option.names();
  if (parser.isSet(reference_listing_option)) {
    flags += reference_listing_option_list.first();
  }
  const auto& intermediate_option_list = intermediate_option.names();
  if (parser.isSet(intermediate_option)) {
    flags += intermediate_option_list.first();
  }
  const QStringList files = parser.positionalArguments();
  if (files.empty()) {
    qWarning().noquote() << "No source file specified.";
    a.exit(1);
    return 1;
  }
  const QString& filePath = files.first();
  if (parser.isSet(compile_option)) {
    pascal = new Pascal("compile", filePath.toStdString(), flags.toStdString(), &a);
    QObject::connect(pascal, &Pascal::exit, &a, &QCoreApplication::exit);
    emit pascal->exit(0);
    return 0;
  } else if (parser.isSet(interpret_option)) {
    pascal = new Pascal("interpret", filePath.toStdString(), flags.toStdString(), &a);
    QObject::connect(pascal, &Pascal::exit, &a, &QCoreApplication::exit);
    emit pascal->exit(0);
    return 0;
  } else {
    parser.showHelp();
    return 0;
  }
  return a.exec();
}
