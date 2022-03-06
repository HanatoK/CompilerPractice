#include "Common.h"

#include <fmt/format.h>

std::string any_to_string(const std::any &a)
{
  if (a.type() == typeid(bool)) {
    const bool value = std::any_cast<bool>(a);
    if (value) {
      return "true";
    } else {
      return "false";
    }
  } else if (a.type() ==  typeid(int)) {
    return std::to_string(std::any_cast<int>(a));
  } else if (a.type() == typeid(long)) {
    return std::to_string(std::any_cast<long>(a));
  } else if (a.type() == typeid(long long)) {
    return std::to_string(std::any_cast<long long>(a));
  } else if (a.type() == typeid(float)) {
    return fmt::format("{:.9f}", std::any_cast<float>(a));
  } else if (a.type() == typeid(double)) {
    return fmt::format("{:.9f}", std::any_cast<double>(a));
  } else if (a.type() == typeid(std::string)) {
    return std::any_cast<std::string>(a);
  } else if (a.type() == typeid(unsigned long)) {
    return std::to_string(std::any_cast<unsigned long>(a));
  } else if (a.type() == typeid(unsigned long long)) {
    return std::to_string(std::any_cast<unsigned long long>(a));
  } else if (a.type() == typeid(unsigned int)) {
    return std::to_string(std::any_cast<unsigned int>(a));
  } else if (a.type() == typeid(const char*)) {
    return std::string{std::any_cast<const char*>(a)};
  } else {
    return "Failed to cast std::any!";
  }
}

void clear_line_ending(std::string& line) {
  const auto ending = line.find_last_of("\r\n");
  if (ending != std::string::npos) {
    line.erase(line.begin() + static_cast<int>(ending));
  }
}

bool compare_any(const std::any &a, const std::any &b)
{
  if (a.type() != b.type()) {
    return false;
  } else {
    if (!a.has_value() && !b.has_value()) {
      return true;
    } else {
      if (a.has_value() && b.has_value()) {
        // try type casting
        if (a.type() == typeid(bool)) {
          const auto value_a = std::any_cast<bool>(a);
          const auto value_b = std::any_cast<bool>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(int)) {
          const auto value_a = std::any_cast<int>(a);
          const auto value_b = std::any_cast<int>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(long)) {
          const auto value_a = std::any_cast<long>(a);
          const auto value_b = std::any_cast<long>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(long long)) {
          const auto value_a = std::any_cast<long long>(a);
          const auto value_b = std::any_cast<long long>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(float)) {
          const auto value_a = std::any_cast<float>(a);
          const auto value_b = std::any_cast<float>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(double)) {
          const auto value_a = std::any_cast<double>(a);
          const auto value_b = std::any_cast<double>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(std::string)) {
          const auto value_a = std::any_cast<std::string>(a);
          const auto value_b = std::any_cast<std::string>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(unsigned int)) {
          const auto value_a = std::any_cast<unsigned int>(a);
          const auto value_b = std::any_cast<unsigned int>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(unsigned long)) {
          const auto value_a = std::any_cast<unsigned long>(a);
          const auto value_b = std::any_cast<unsigned long>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(unsigned long long)) {
          const auto value_a = std::any_cast<unsigned long long>(a);
          const auto value_b = std::any_cast<unsigned long long>(b);
          return value_a == value_b;
        } else if (a.type() == typeid(const char*)) {
          const auto value_a = std::any_cast<const char*>(a);
          const auto value_b = std::any_cast<const char*>(b);
          return std::strcmp(value_a, value_b) == 0;
        } else {
          fmt::print("BUG: in bool compare_any(const std::any &a, const std::any &b):\n");
          fmt::print("BUG: failed to cast std::any!\n");
          fmt::print("BUG: type is {}\n", a.type().name());
          return false;
        }
      } else {
        return false;
      }
    }
  }
}

std::string variable_value_to_string(const VariableValueT& a)
{
  std::string result;
  std::visit([&result](auto&& arg){
    using T = std::decay_t<decltype (arg)>;
    if constexpr (std::is_floating_point_v<T>) {
      result = fmt::format("{:.9f}", arg);
    } else if constexpr (std::is_integral_v<T>) {
      result = std::to_string(arg);
    } else if constexpr (std::is_same_v<T, std::string>) {
      result = arg;
    }
  }, a);
  return result;
}

std::string typeformimpl_to_string(TypeFormImpl a) {
  switch (a) {
    case TypeFormImpl::ARRAY:       return std::string{"array"};
    case TypeFormImpl::ENUMERATION: return std::string{"enumeration"};
    case TypeFormImpl::RECORD:      return std::string{"record"};
    case TypeFormImpl::SCALAR:      return std::string{"scalar"};
    case TypeFormImpl::SUBRANGE:    return std::string{"subrange"};
    default:                        return std::string{"unknown"};
  }
}

std::string definitionimpl_to_string(DefinitionImpl a)
{
  switch (a) {
    case DefinitionImpl::CONSTANT:              return std::string{"constant"};
    case DefinitionImpl::ENUMERATION_CONSTANT:  return std::string{"enumeration constant"};
    case DefinitionImpl::FIELD:                 return std::string{"record field"};
    case DefinitionImpl::FUNCTION:              return std::string{"function"};
    case DefinitionImpl::PROCEDURE:             return std::string{"procedure"};
    case DefinitionImpl::PROGRAM:               return std::string{"program"};
    case DefinitionImpl::PROGRAM_PARM:          return std::string{"program praameter"};
    case DefinitionImpl::TYPE:                  return std::string{"type"};
    case DefinitionImpl::VALUE_PARM:            return std::string{"value parameter"};
    case DefinitionImpl::VARIABLE:              return std::string{"variable"};
    case DefinitionImpl::VAR_PARM:              return std::string{"VAR paramater"};
    // TODO: should I use fall-through here?
    case DefinitionImpl::UNDEFINED:
    default:                                    return std::string{"undefined"};
  }
}
