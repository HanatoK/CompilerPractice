#ifndef ARRAYTYPEPARSER_H
#define ARRAYTYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

// Pascal array type examples:
// ========================================================
// type
//    ch_array = array[char] of 1..26;
// var
//    alphabet: ch_array;
// ========================================================
// type
//    color = ( red, black, blue, silver, beige);
//    car_color = array of [color] of boolean;
// var
//    car_body: car_color;
// multi-dimensional arrays are allowed.

class ArrayTypeParser : public PascalSubparserTopDownBase
{
public:
  explicit ArrayTypeParser(PascalParserTopDown& parent);
  virtual ~ArrayTypeParser();
  std::shared_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
private:
  std::shared_ptr<TypeSpecImplBase> parseIndexTypeList(
    std::shared_ptr<PascalToken>& token, std::shared_ptr<TypeSpecImplBase> array_type);
  void parseIndexType(std::shared_ptr<PascalToken>& token,
                      std::shared_ptr<TypeSpecImplBase> array_type);
};

#endif // ARRAYTYPEPARSER_H
