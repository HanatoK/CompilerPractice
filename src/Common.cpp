#include "Common.h"

#include <fmt/format.h>

void clear_line_ending(std::string& line) {
  const auto ending = line.find_last_of("\r\n");
  if (ending != std::string::npos) {
    line.erase(line.begin() + static_cast<int>(ending));
  }
}

std::string variable_value_to_string(const VariableValueT& a)
{
  std::string result;
  if (a.index() == 0) {
    result = "empty";
  }
  std::visit([&result](auto&& arg){
    using T = std::decay_t<decltype (arg)>;
    if constexpr (std::is_same_v<T, bool>) {
      result = arg ? "true" : "false";
    } else if constexpr (std::is_same_v<T, PascalFloat>) {
      result = fmt::format("{:.9f}", arg);
    } else if constexpr (std::is_same_v<T, PascalInteger>) {
      result = std::to_string(arg);
    } else if constexpr (std::is_same_v<T, std::string>) {
      result = "\'" + arg + "\'";
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
