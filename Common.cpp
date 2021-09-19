#include "Common.h"

#include <fmt/format.h>

std::string any_to_string(const std::any &a)
{
  if (a.type() == typeid(bool)) {
    const bool value = std::any_cast<bool>(a);
    if (value == true) {
      return "true";
    } else {
      return "false";
    }
  } else if (a.type() ==  typeid(int)) {
    return std::to_string(std::any_cast<int>(a));
  } else if (a.type() == typeid(long long)) {
    return std::to_string(std::any_cast<long long>(a));
  } else if (a.type() == typeid(float)) {
    return fmt::format("{:.9f}", std::any_cast<float>(a));
  } else if (a.type() == typeid(double)) {
    return fmt::format("{:.9f}", std::any_cast<double>(a));
//    return std::to_string(std::any_cast<double>(a));
  } else if (a.type() == typeid(std::string)) {
    return std::any_cast<std::string>(a);
  } else if (a.type() == typeid(unsigned long long)) {
    return std::to_string(std::any_cast<unsigned long long>(a));
  } else if (a.type() == typeid(unsigned long)) {
    return std::to_string(std::any_cast<unsigned long>(a));
  } else {
    return "Failed to cast std::any!";
  }
}

void clear_line_ending(std::string& line) {
  const auto ending = line.find_last_of("\r\n");
  if (ending != std::string::npos) {
    line.erase(line.begin() + ending);
  }
}
