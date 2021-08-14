#ifndef COMMON_H
#define COMMON_H

#include <map>

// assume no key shares a same value
template <typename T1, typename T2>
std::map<T2, T1> reverse_map(const std::map<T1, T2>& map_in) {
  std::map<T2, T1> map_out;
  for (auto it = map_in.begin(); it != map_in.end(); ++it) {
    map_out[it->second] = it->first;
  }
  return map_out;
}

#endif // COMMON_H
