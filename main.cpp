#include "Pascal.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QObject>

void initMetaTypes() {
  qRegisterMetaType<PascalTokenType>("PascalTokenType");
  qRegisterMetaType<PascalErrorCode>("PascalErrorCode");
}

int main(int argc, char *argv[])
{
  initMetaTypes();
  QCoreApplication a(argc, argv);
  a.setApplicationName("PascalPractice");
  a.setApplicationVersion("0.1");
  QCommandLineParser parser;
  const QCommandLineOption compile_option("compile", "Compile the source file.");
  const QCommandLineOption interpret_option("interpret", "Execute the source file directly.");
  const QCommandLineOption reference_listing_option("x", "Generate reference listing.");
  parser.addHelpOption();
  parser.addOption(compile_option);
  parser.addOption(interpret_option);
  parser.addOption(reference_listing_option);
  parser.addPositionalArgument("source file", "the pascal source file");
  parser.process(a);
  Pascal* pascal = nullptr;
  QString flags;
  if (parser.isSet(reference_listing_option)) {
    flags += reference_listing_option.names().first();
  }
  const QStringList files = parser.positionalArguments();
  if (files.empty()) {
    qWarning().noquote() << "No source file specified.";
    a.exit(1);
    return 1;
  }
  const QString& filePath = files.first();
  if (parser.isSet(compile_option)) {
    pascal = new Pascal("compile", filePath, flags, &a);
    QObject::connect(pascal, &Pascal::exit, &a, &QCoreApplication::exit);
    emit pascal->exit(0);
    return 0;
  } else if (parser.isSet(interpret_option)) {
    pascal = new Pascal("interpret", filePath, flags, &a);
    QObject::connect(pascal, &Pascal::exit, &a, &QCoreApplication::exit);
    emit pascal->exit(0);
    return 0;
  } else {
    parser.showHelp();
    return 0;
  }
  return a.exec();
}
