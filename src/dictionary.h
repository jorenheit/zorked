#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "json_fwd.hpp"

enum class WordType {
  Unknown,
  BuiltinCommand,
  Phrase,
  Direction,
  Number,
  Preposition,
  Article
};

inline WordType operator|(WordType lhs, WordType rhs) {
  return static_cast<WordType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

class Dictionary {
public:
  struct Entry {
    std::string str = "unknown";
    WordType type = WordType::Unknown;
    operator bool () const {
      return type != WordType::Unknown;
    }
  };

private:
  std::map<std::string, Entry> _dict;
  
public:
  Dictionary() = default;
  Dictionary(nlohmann::json const &obj);  
  Entry operator[](std::string const &word) const;
  std::vector<Entry> tokenize(std::string input, size_t const ngramSize) const;
  void addPhrase(std::string const &phrase);
  
};

inline std::ostream &operator<<(std::ostream &out, Dictionary::Entry const &entry) {
  return (out << entry.str);
}

#endif // DICTIONARY_H
