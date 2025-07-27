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
  RemovePunctuation,
  ToLower,
  ToUpper
};

// Policy specializations
#include "stringtransform.tpp" 

template <StringTransform First, StringTransform ... Rest>
char transformChar(char c) {
  char result = TransformPolicy<First>::transform(c);
  if constexpr (sizeof ... (Rest) > 0) {
    return transformChar<Rest...>(result);
  }
  return result;
}

template <StringTransform ... Transforms>
std::string transformString(std::string const &input) {
  std::string result;
  for (char c: input) {
    char sub = transformChar<Transforms ...>(c);
    if (sub) result += sub;
  }
  return result;
}

#endif // UTIL_H
