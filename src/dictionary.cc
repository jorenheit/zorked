#include "jsonobject.h"
#include "dictionary.h"
#include "util.h"

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
  JSONObject dict = parseEntireFile(dictFilename);
  for (auto &[sectionName, wordType]: sections) {
    JSONObject sect = dict.get<JSONObject>(sectionName);
    for (auto const &[key_, value]: sect) {
      std::string key = transformString<ToLower, RemoveSpaces>(key_);
      _dict[key] = Entry{ .str = key, .type = wordType };
      for (auto const &[index, word]: value) {
	std::string synonym = transformString<ToLower, RemoveSpaces>(word.get<std::string>());
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
