#include "Pascal.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  a.setApplicationName("PascalPractice");
  a.setApplicationVersion("0.1");
  QCommandLineParser parser;
  const QCommandLineOption compileOption("compile", "compile the source file");
  const QCommandLineOption intepretOption("intepret", "execute the source file directly");
  parser.addHelpOption();
  parser.addOption(compileOption);
  parser.addOption(intepretOption);
  parser.addPositionalArgument("source file", "the pascal source file");
  parser.process(a);
  Pascal* pascal = nullptr;
  const QStringList files = parser.positionalArguments();
  if (files.empty()) {
    qWarning().noquote() << "No source file specified.";
    a.exit(1);
    return 1;
  }
  const QString& filePath = files.first();
  if (parser.isSet(compileOption)) {
    pascal = new Pascal("compile", filePath, "ix", &a);
  } else if (parser.isSet(intepretOption)) {
    pascal = new Pascal("intepret", filePath, "ix", &a);
  } else {
    parser.showHelp();
  }
  delete pascal;
  pascal = nullptr;
  return a.exec();
}
