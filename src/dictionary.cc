#include <fstream>
#include "dictionary.h"
#include "util.h"
#include "exception.h"
#include "json.hpp"

using json = nlohmann::json;
using enum StringTransform;


Dictionary::Dictionary(json const &obj) {
  static std::pair<std::string, WordType> const sections[] {
    {"builtin-commands", WordType::BuiltinCommand},
    {"directions", WordType::Direction},
    {"numbers", WordType::Number},
    {"prepositions", WordType::Preposition},
    {"articles", WordType::Article}
  };

  // TODO: error handling (missing sections)
  for (auto &[sectionName, wordType]: sections) {
    for (auto const &[key_, value]: obj.at(sectionName).items()) {
      std::string key = transformString<ToLower, NormalizeSpaces, RemovePunctuation>(key_);
      _dict[key] = Entry{ .str = key, .type = wordType };
      for (std::string synonym: value) {
	synonym = transformString<ToLower, NormalizeSpaces, RemovePunctuation>(synonym);
	_dict[synonym] = Entry { .str = key, .type = wordType };
      }
    }
  }
}

void Dictionary::addPhrase(std::string const &phrase) {
  std::string key = transformString<ToLower, NormalizeSpaces, RemovePunctuation>(phrase);
  _dict[key] = Entry { .str = key, .type = WordType::Phrase };
}

Dictionary::Entry Dictionary::operator[](std::string const &word) const {
  return _dict.contains(word) ? _dict.at(word) : Entry { word, WordType::Unknown };
}

std::vector<Dictionary::Entry> Dictionary::tokenize(std::string input, size_t const ngramSize) const {
  auto wordVec = split(input, ' ');
  std::vector<Dictionary::Entry> tokens;
  size_t startIdx = 0;
  while (startIdx < wordVec.size()) {
    for (size_t n = ngramSize; n != 0; --n) {
      if (startIdx + n - 1 >= wordVec.size()) continue;
      std::string ngram = wordVec[startIdx];
      for (size_t idx = startIdx + 1; idx <= startIdx + n - 1; ++idx) {
	ngram += (" " + wordVec[idx]);
      }
      Entry entry = (*this)[transformString<ToLower, NormalizeSpaces, RemovePunctuation>(ngram)];
      if (entry || n == 1) {
	tokens.push_back(entry);
	startIdx += n;
	break;
      }
    }
  }

  return tokens;
}
