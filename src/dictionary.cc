#include <fstream>
#include "dictionary.h"
#include "util.h"
#include "exception.h"
#include "json.hpp"

using json = nlohmann::json;

namespace {
  json parseEntireFile(std::filesystem::path const &filename) {
    std::ifstream file(filename);
    if (!file) {
      throw Exception::ErrorOpeningFile(filename);
    }
    return json::parse(file);
  }
}

Dictionary::Dictionary(std::string const &dictFilename) {
  static std::pair<std::string, WordType> const sections[] {
    {"builtin-commands", WordType::BuiltinCommand},
    {"directions", WordType::Direction},
    {"numbers", WordType::Number},
    {"verbs", WordType::Verb},
    {"nouns", WordType::Noun},
    {"prepositions", WordType::Preposition},
    {"adjectives", WordType::Adjective},
    {"articles", WordType::Article}
  };

  using enum StringTransform;
  json dict = parseEntireFile(dictFilename);
  for (auto &[sectionName, wordType]: sections) {
    json sect = dict.at(sectionName);
    for (auto const &[key_, value]: sect.items()) {
      std::string key = transformString<ToLower, RemoveSpaces>(key_);
      _dict[key] = Entry{ .str = key, .type = wordType };
      for (std::string synonym: value) {
	synonym = transformString<ToLower, RemoveSpaces>(synonym);
	_dict[synonym] = Entry { .str = key, .type = wordType };
      }
    }
  }
}
  
Dictionary::Entry Dictionary::operator[](std::string const &word) const {
  return _dict.contains(word) ? _dict.at(word) : Entry { word, WordType::Unknown };
}

Dictionary::Entry Dictionary::findEntry(std::vector<std::string> const &vec, size_t startIdx, size_t n) const {
  assert(startIdx + n - 1 < vec.size() && "ngram index/size out of bounds");
  std::string ngram = vec[startIdx];
  for (size_t idx = startIdx + 1; idx <= startIdx + n - 1; ++idx) {
    ngram += vec[idx];
  }
  return (*this)[ngram];
} 


std::vector<Dictionary::Entry> Dictionary::tokenize(std::string input, size_t const ngramSize) const {
  using enum StringTransform;
  input = transformString<ToLower, RemovePunctuation>(input);
  auto vec = split(input, ' ');

  std::vector<Dictionary::Entry> tokens;
  size_t i = 0;
  while (i < vec.size()) {
    for (size_t n = ngramSize; n != 0; --n) {
      if (i + n - 1 >= vec.size()) continue;
      Entry entry = findEntry(vec, i, n);
      if (entry || n == 1) {
	tokens.push_back(entry);
	i += n;
	break;
      }
    }
  }

  return tokens;
}
