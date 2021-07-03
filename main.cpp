#include "Pascal.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QObject>

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  a.setApplicationName("PascalPractice");
  a.setApplicationVersion("0.1");
  QCommandLineParser parser;
  const QCommandLineOption compileOption("compile", "compile the source file");
  const QCommandLineOption interpretOption("interpret", "execute the source file directly");
  parser.addHelpOption();
  parser.addOption(compileOption);
  parser.addOption(interpretOption);
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
    QObject::connect(pascal, &Pascal::exit, &a, &QCoreApplication::exit);
    emit pascal->exit(0);
    return 0;
  } else if (parser.isSet(interpretOption)) {
    pascal = new Pascal("interpret", filePath, "ix", &a);
    QObject::connect(pascal, &Pascal::exit, &a, &QCoreApplication::exit);
    emit pascal->exit(0);
    return 0;
  } else {
    parser.showHelp();
    return 0;
  }
  return a.exec();
}
