#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cassert>
#include <vector>
#include <string>

#define UNREACHABLE(str) assert(!("Unreachable: " str));

std::string trim(std::string str);
std::vector<std::string> split(std::string const &str, char const c, bool allowEmpty = false);
std::vector<std::string> split(std::string const &str, std::string const &token, bool allowEmpty = false);


enum class StringTransform {
  RemoveSpaces,
  NormalizeSpaces,
  RemovePunctuation,
  ToLower,
  ToUpper
};

// Policy specializations
#include "stringtransform.tpp" 

template <StringTransform First, StringTransform ... Rest>
char transformChar(char c, char prev) {
  char result = TransformPolicy<First>::transform(c, prev);
  if constexpr (sizeof ... (Rest) > 0) {
    return transformChar<Rest...>(result, prev);
  }
  return result;
}

template <StringTransform ... Transforms>
std::string transformString(std::string const &input) {
  std::string result;
  char prev = 0;
  for (char c: input) {
    char sub = transformChar<Transforms ...>(c, prev);
    if (sub) result += sub;
    prev = sub;
  }
  return result;
}

inline std::string normalizeString(std::string str) {
  // TODO: replace dash with space
  using enum StringTransform;
  str = trim(str);
  return transformString<ToLower, NormalizeSpaces, RemovePunctuation>(str);
}

struct ObjectDescriptor {
  std::string noun;
  std::vector<std::string> adjectives;
  size_t number = 1;
  std::string str(bool includeNumber = false) const;
};


#endif // UTIL_H
