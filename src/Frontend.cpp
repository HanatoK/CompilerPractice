#include "Frontend.h"

#include <QString>
#include <istream>

Source::Source(std::ifstream &ifs): mStream(ifs)
{
  mLineNum = 0;
  mCurrentPos = -2;
  mReadOk = false;
}

char Source::currentChar()
{
  if (mCurrentPos == -2) {
    // first time?
    readLine();
    return nextChar();
  } else if (!mReadOk) {
    // at the end of file?
    return EOF;
  } else if ((mCurrentPos == -1) || (mCurrentPos == mLine.size())) {
    // at the end of line?
    return EOL;
  } else if (mCurrentPos > mLine.length()) {
    // need to read the next line?
    readLine();
    return nextChar();
  } else {
    return mLine.at(mCurrentPos);
  }
}

char Source::nextChar()
{
  ++mCurrentPos;
  return currentChar();
}

char Source::peekChar()
{
  currentChar();
  if (!mReadOk) {
    return EOF;
  }
  const int nextPos = mCurrentPos + 1;
  if (nextPos < mLine.size()) {
    return mLine.at(nextPos);
  } else {
    return EOL;
  }
}

void Source::readLine()
{
  mReadOk = std::getline(mStream, mLine).good();
  mCurrentPos = -1;
  if (mReadOk) {
    // not EOF or error
    ++mLineNum;
    clear_line_ending(mLine);
    sendMessage(mLineNum, mLine);
  } else {
    mLine = "";
  }
}

int Source::currentPos() const
{
  return mCurrentPos;
}

Source::~Source()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

int Source::lineNum() const
{
  return mLineNum;
}
