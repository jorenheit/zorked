#include "util.h"

std::string trim(std::string str) {
  size_t idx = 0;
  while (idx < str.length() && std::isspace(str[idx])) { ++idx; }
  if (idx == str.length()) {
    str.clear();
    return str;
  }
        
  size_t const start = idx;
  idx = str.length() - 1;
  while (idx >= 0 && std::isspace(str[idx])) { --idx; }
  size_t const stop = idx;
        
  str = str.substr(start, stop - start + 1);
  return str;
}

std::vector<std::string> split(std::string const &str, char const c, bool allowEmpty) {
  return split(str, std::string{c}, allowEmpty);
}

std::vector<std::string> split(std::string const &str, std::string const &token, bool allowEmpty) {
  std::vector<std::string> result;
  size_t prev = 0;
  size_t current = 0;
  while ((current = str.find(token, prev)) != std::string::npos) {
    std::string part = str.substr(prev, current - prev);
    trim(part);
    if (allowEmpty || !part.empty()) result.push_back(part);
    prev = current + token.length();
  }
  std::string last = str.substr(prev);
  trim(last);
  if (allowEmpty || !last.empty()) result.push_back(last);

  return result;
}

std::string ObjectDescriptor::str(bool includeNumber) const {
  std::string result = (includeNumber && number > 1) ? (std::to_string(number) + " ") : "";
  for (std::string const &adj: adjectives) {
    result += (adj + " ");
  }
  result += noun;
  return result;
}

