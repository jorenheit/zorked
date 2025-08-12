#include <fstream>
#include "dictionary.h"
#include "util.h"
#include "exception.h"
#include "json.hpp"

using json = nlohmann::json;

Dictionary::Dictionary(json const &obj) {
  static std::pair<std::string, WordType> const sections[] {
    {"builtin-commands", WordType::BuiltinCommand},
    {"directions", WordType::Direction},
    {"numbers", WordType::Number},
    {"prepositions", WordType::Preposition},
    {"articles", WordType::Article}
  };

  // TODO: error handling (missing sections)
  
  // 1. Load all predefined words and their synonyms:
  //    - Builtin commands like take, move, etc.
  //    - Directions: North, East, South, West, Up and Down
  //    - Numbers written out
  //    - Prepositions
  //    - Articles

  auto add = [this](std::string const &base, std::string const &alt, WordType type) {
    _dict[normalizeString(alt)] = Entry { .str = normalizeString(base), .type = type };
  };

  for (auto &[sectionName, wordType]: sections) {
    for (auto const &[key, value]: obj.at(sectionName).items()) {
      add(key, key, wordType);
      if (value.is_array()) {
	for (json const &synonym: value) {
	  if (not synonym.is_string()) {
	    throw Exception::InvalidFieldType("Dictionary::" + sectionName, key, "string or array of strings", value.type_name());
	  }
	  add(key, synonym.get<std::string>(), wordType);
	}
      }
      else if (value.is_string()) {
	add(key, value.get<std::string>(), wordType);
      }
      else {
	throw Exception::InvalidFieldType("Dictionary::" + sectionName, key, "string or array of strings", value.type_name());
      }
    }
  }

  // 2. Keep seperate maps for irregular plurals
  std::string sectionName = "irregular-plurals";
  for (auto const &[singular, plural]: obj.at(sectionName).items()) {
    if (plural.is_array()) {
      for (json const &pl: plural) {
	if (not pl.is_string()) {
	  throw Exception::InvalidFieldType("Dictionary::" + sectionName, singular, "string or array of strings", plural.type_name());
	}
	_plurals[singular] = pl;
	_singulars[pl] = singular;
      }
    }
    else if (plural.is_string()) {
      _plurals[singular] = plural;
      _singulars[plural] = singular;
    }
    else {
      throw Exception::InvalidFieldType("Dictionary::" + sectionName, singular, "string or array of strings", plural.type_name());
    }
  }
}

std::string Dictionary::pluralize(std::string const &singular) const {
  std::string tmp = normalizeString(singular);
  
  // 1. See if the dictionary contains this as an irregular plural.
  if (_plurals.contains(tmp)) return _plurals.at(tmp);

  // 2. Not in dictionary -> apply rules:
  //    https://tutors.com/lesson/singular-and-plural-nouns
  if (tmp.size() > 1 && tmp.ends_with("y")) {
    if (std::string("aeiou").find(tmp[tmp.size() - 2]) == std::string::npos) {
      return tmp.substr(0, tmp.size() - 1) + "ies";
    }
    return tmp + "s";
  }
  if (tmp.ends_with("s")  ||
      tmp.ends_with("x")  ||
      tmp.ends_with("z")  ||
      tmp.ends_with("ch") ||
      tmp.ends_with("sh")) {

    return tmp + "es";
  }
  if (tmp.ends_with("f")) {
    return tmp.substr(0, tmp.size() - 1) + "ves";
  }
  if (tmp.ends_with("fe")) {
    return tmp.substr(0, tmp.size() - 2) + "ves";
  }
  if (tmp.ends_with("o")) {
    return tmp + "es";
  }

  return tmp + "s";
}

std::string Dictionary::singularize(std::string const &plural) const {
  std::string tmp = normalizeString(plural);
  
  // 1. See if the dictionary contains this as an irregular plural.
  if (_singulars.contains(tmp)) return _singulars.at(tmp);

  // 2. Go through list of nouns and phrases and apply the plural-algorithm to
  //    see if we can find a match there.
  for (auto const &[key, entry]: _dict) {
    if (entry.type == WordType::Noun || entry.type == WordType::Phrase) {
      if (plural == pluralize(key)) {
	return key;
      }
    }
  }

  // 3. No hits in known vocabulary -> resort to naive algorithm to singularize the word
  if (tmp.size() > 3 && tmp.ends_with("ies")) {
    return tmp.substr(0, tmp.size() - 3) + "y";
  }
  if (tmp.size() > 3 && tmp.ends_with("ves")) {
    return tmp.substr(0, tmp.size() - 3) + "f";
  }
  if (tmp.size() > 3 && (tmp.ends_with("ses")  ||
			 tmp.ends_with("xes")  ||
			 tmp.ends_with("zes")  ||
			 tmp.ends_with("ches") ||
			 tmp.ends_with("shes"))) {
    return tmp.substr(0, tmp.size() - 2);
  }
  if (tmp.size() > 1 && tmp.ends_with("s")) {
    return tmp.substr(0, tmp.size() - 1);
  }

  return tmp;
}

void Dictionary::addNoun(std::string const &noun) {
  std::string key = normalizeString(noun);
  _dict[key] = Entry { .str = key, .type = WordType::Noun };
}

void Dictionary::addPhrase(std::string const &phrase) {
  std::string key = normalizeString(phrase);
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
      Entry entry = (*this)[normalizeString(ngram)];
      if (entry || n == 1) {
	tokens.push_back(entry);
	startIdx += n;
	break;
      }
    }
  }

  return tokens;
}
