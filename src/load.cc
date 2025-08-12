#include <filesystem>
#include <fstream>
#include <iostream>
#include "exception.h"
#include "objectmanager.h"
#include "game.h"
#include "dictionary.h"
#include "json.hpp"
using json = nlohmann::json;

namespace {
  
  //TODO: deal with int/double fields (number_unsigned/number_signed/number_float) better
  
  json parseEntireFile(std::filesystem::path const &filename) {
    std::ifstream file(filename);
    if (!file) {
      throw Exception::ErrorOpeningFile(filename);
    }
    return json::parse(file);
  }

  namespace Field {

    struct Required {
      json::value_t const type;
      std::string const key;
      Required(std::string const &k):
	type(json::value_t::null),
	key(k)
      {}
      
      Required(json::value_t t, std::string const &k):
	type(t),
	key(k)
      {}
    };

    struct Optional {
      json::value_t const type;
      std::string const key;
      json const value;

      Optional(json::value_t t, std::string const &k):
	type(t),
	key(k),
	value(json(t))
      {}
      
      Optional(json::value_t t, std::string const &k, json const &v):
	type(t),
	key(k),
	value(v)
      {
	assert(value.type() == type);
      }
    };
  };

  namespace json_t {
    using enum json::value_t;
  }

  void setPath(json &obj, std::string const &path) {
    obj["_path"] = path;
  }

  void updateDictionary(std::vector<std::string> const &wordList) {
    // We don't care if this is a list of nouns or verb. We just need to find entries
    // that contain spaces, so the tokenizer can look for n-grams in the user-input
    // that matches these compound expressions (that do not contain adjectives).

    for (std::string const &word: wordList) {
      if (word.find(' ') == std::string::npos) {
	Global::g_dict.addNoun(word);
      }
      else {
	Global::g_dict.addPhrase(word);
      }
    }
  }

  void addProxy(std::string const &id, json const &obj, ObjectType type) {
    Global::g_objectManager.addProxy(id, obj, type);
  }
  
  void normalizeField(json &obj, std::string const &path, Field::Required const &field) {
    assert(obj.type() == json_t::object);
    if (!obj.contains(field.key)) {
      throw Exception::ExpectedField(path, field.key);
    }
    json value = obj.at(field.key);
    if (field.type != json_t::null && value.type() != field.type) {
      throw Exception::InvalidFieldType(path, field.key, json(field.type).type_name(), value.type_name());
    }
  }

  void normalizeField(json &obj, std::string const &path, Field::Optional const &field) {
    assert(obj.type() == json_t::object);
    if (!obj.contains(field.key)) {
      obj[field.key] = field.value;
    }
    else if (obj.at(field.key).type() != field.type) {
      throw Exception::InvalidFieldType(path, field.key, json(field.type).type_name(), obj.at(field.key).type_name());
    }
  }

  void normalizeArray(json::value_t type, json &obj, std::string const &key, std::string const &path) {
    assert(obj.is_array());
    for (json &element: obj) {
      if (element.type() != type)
	throw Exception::InvalidArrayType(path, key, json(type).type_name(), element.type_name());
    }
  }

  void normalizeState(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object);
    for (auto &[key, value]: obj.items()) {
      if (value.type() != json_t::boolean) {
	throw Exception::InvalidFieldType(path, key, "boolean", value.type_name());
      }
    }
    setPath(obj, path);
  }

  template <typename ... Fields>
  void normalizeObject(json &obj, std::string const &path, Fields ... fields) {
    assert(obj.is_object());
    (normalizeField(obj, path, fields), ...);
  }

  void normalizeCondition(json &obj, std::string const &kind, std::string const &path) {
    assert(obj.is_object());
    std::string currentPath = path + "::" + kind + "-condition";
    normalizeObject(obj, currentPath,
		    Field::Optional(json_t::string, "+"),
		    Field::Optional(json_t::string, "-")
		    );

    // Set paths recursively but do not check if the structure is sound, that will
    // be done while constructing the Condition object.
    
    auto setPathRecursively = [](json &obj, std::string const &path) -> void {
      auto setPathRecursively_ = [&](auto const &self, json &obj, std::string const &path) -> void {
	setPath(obj, path);
	for (auto &[key, value] : obj.items()) {
	  if (!value.is_object()) continue;
	  self(self, value, path + "::" + key);
	}
      };
      setPathRecursively_(setPathRecursively_, obj, path);
    };

    setPathRecursively(obj, currentPath);
  }

  void normalizeEffects(json &obj, std::string const &path) {
    assert(obj.is_object());
    // Parsing each of the effects will be done in the Effect constructor
    setPath(obj, path);
  }
  

  void normalizeInteractions(json &obj, std::string const &path) {
    assert(obj.is_object());

    for (auto const &[key, value]: obj.items()) {
      std::string currentPath = path + "::" + key;

      std::string action = split(key, '.')[0];
      normalizeObject(value, currentPath,
		      Field::Optional(json_t::array, "verbs"),
		      Field::Optional(json_t::object, action + "-condition"),
		      Field::Optional(json_t::object, "+"),
		      Field::Optional(json_t::object, "-")
		      
		      );

      normalizeArray(json_t::string, value["verbs"], "verbs", currentPath);
      normalizeCondition(value[action + "-condition"], action, currentPath);
      normalizeEffects(value["+"], currentPath);
      normalizeEffects(value["-"], currentPath);
      setPath(value, currentPath);

      updateDictionary(value["verbs"].get<std::vector<std::string>>());
    }
  }

  void normalizeDescription(json &obj, std::string const &path) {
    std::string currentPath = path + "::description";
    if (obj.is_string()) {
      std::string descr = obj;
      obj = json {{"lore-condition", json {{"+", descr}, {"-", ""}}}};
    }
    else if (!obj.is_object()) {
      throw Exception::SpecificFormatError(path, "Description should be either a string or a 'lore-condition' object.");
    }

    normalizeObject(obj, currentPath, Field::Required(json_t::object, "lore-condition"));
    normalizeCondition(obj["lore-condition"], "lore", currentPath);
    setPath(obj, currentPath);
  }
  
  void normalizeInspect(json &obj, std::string const &path) {
    std::string currentPath = path + "::inspect";
    if (obj.is_string() || obj.is_null()) {
      std::string descr = obj.is_null() ? "" : obj.get<std::string>();
      obj = json {{"inspect-condition", json {{"+", descr}, {"-", ""}}}};
    }
    else if (!obj.is_object()) {
      throw Exception::SpecificFormatError(path, "Inspect field should be either a string or an 'inspect-condition' object.");
    }

    normalizeObject(obj, currentPath, Field::Required(json_t::object, "inspect-condition"));
    normalizeCondition(obj["inspect-condition"], "inspect", currentPath);
    setPath(obj, currentPath);
  }

  void normalizeCommonItemReferences(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object);

    std::string currentPath = path + "::common-items";
    for (auto const &[key, value]: obj.items()) {
      if (not value.is_number_unsigned()) {
	throw Exception::InvalidFieldType(currentPath, key, "number", value.type_name());
      }
    }
    setPath(obj, currentPath);
  }

  void normalizeItems(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object);
    std::string currentPath = path + "::items";
    
    for (auto &[key, value]: obj.items()) {
      value["common"] = false;
      std::string nestedPath = currentPath + "::" + key;
      normalizeObject(value, nestedPath,
		      Field::Required("description"),
		      Field::Required(json_t::string, "label"),
		      Field::Optional(json_t::array, "nouns"),
		      Field::Optional(json_t::array, "adjectives"),
		      Field::Optional(json_t::boolean, "portable"),
		      Field::Optional(json_t::number_float, "weight"),
		      Field::Optional(json_t::object, "take-condition"),
		      Field::Optional(json_t::object, "state"),
		      Field::Optional(json_t::object, "items"),
		      Field::Optional(json_t::object, "common-items"),
		      Field::Optional(json_t::object, "interactions")
		      );


      normalizeDescription(value["description"], nestedPath);
      normalizeInspect(value["inspect"], nestedPath);
      normalizeInteractions(value["interactions"], nestedPath);
      normalizeArray(json_t::string, value["nouns"], "nouns", nestedPath);
      normalizeArray(json_t::string, value["adjectives"], "adjectives", nestedPath);
      normalizeCondition(value["take-condition"], "take", nestedPath);
      normalizeItems(value["items"], nestedPath);
      normalizeCommonItemReferences(value["common-items"], nestedPath);
      normalizeState(value["state"], nestedPath);
      setPath(value, nestedPath);

      updateDictionary(value["nouns"].get<std::vector<std::string>>());
      addProxy(key, value, ObjectType::LocalItem);
    }
    setPath(obj, currentPath);
  }

  void normalizeConnections(json &obj, std::string const &path) {
    assert(obj.type() == json_t::array);

    std::string currentPath = path + "::connections";
    for (size_t idx = 0; idx != obj.size(); ++idx) {
      std::string nestedPath = path + "[" + std::to_string(idx) + "]";
      normalizeObject(obj[idx], nestedPath,
		      Field::Required(json_t::string,  "from"),
		      Field::Required(json_t::string,  "to"),
		      Field::Required(json_t::string,  "direction"),
		      Field::Optional(json_t::boolean, "symmetric"),
		      Field::Optional(json_t::object,  "move-condition")
		      );

      normalizeCondition(obj[idx]["move-condition"], "move", nestedPath);
      setPath(obj[idx], nestedPath);
      Global::g_objectManager.setConnections(obj);
    }
  }

  void processPlayerFile(json &obj, std::string const &path) {
    if (not obj.is_object())
      throw Exception::InvalidFieldType(path, "player", "object", obj.type_name());
    
    normalizeObject(obj, path,
		    Field::Required("description"),		 
		    Field::Required(json_t::string, "label"),
		    Field::Optional(json_t::number_float, "health", 100.0),
		    Field::Optional(json_t::object, "state"),
		    Field::Optional(json_t::object, "items"),
		    Field::Optional(json_t::object, "common-items"),
		    Field::Optional(json_t::array, "nouns"),
		    Field::Optional(json_t::array, "adjectives"),
		    Field::Optional(json_t::object, "interactions")
		    );

    normalizeDescription(obj["description"], path);
    normalizeInspect(obj["inspect"], path);
    normalizeInteractions(obj["interactions"], path);
    normalizeArray(json_t::string, obj["nouns"], "nouns", path);
    normalizeArray(json_t::string, obj["adjectives"], "adjectives", path);
    normalizeItems(obj["items"], path);
    normalizeCommonItemReferences(obj["common-items"], path);
    setPath(obj, path);

    updateDictionary(obj["nouns"].get<std::vector<std::string>>());
    Global::g_objectManager.setPlayer(obj);
  }

  void processCommonItemsFile(json &obj, std::string const &path) {
    if (not obj.is_object())
      throw Exception::InvalidFieldType(path, "common", "object", obj.type_name());

    for (auto &[key, value]: obj.items()) {
      if (value.contains("state")) {
	throw Exception::SpecificFormatError(path, "Common items may not contain a state-field.");
      }
      
      std::string nestedPath = path + "::" + key;
      value["state"] = json::object();
      value["common"] = true;
      normalizeObject(value, nestedPath,
		      Field::Required("description"),
		      Field::Required(json_t::string, "label"),
		      Field::Optional(json_t::array, "nouns"),
		      Field::Optional(json_t::array, "adjectives"),
		      Field::Optional(json_t::boolean, "portable"),
		      Field::Optional(json_t::number_float, "weight"),
		      Field::Optional(json_t::object, "items"),
		      Field::Optional(json_t::object, "common-items"),
		      Field::Optional(json_t::object, "take-condition"),
		      Field::Optional(json_t::object, "interactions")
		      );

      normalizeDescription(value["description"], nestedPath);
      normalizeInspect(value["inspect"], nestedPath);
      normalizeInteractions(value["interactions"], nestedPath);
      normalizeArray(json_t::string, value["nouns"], "nouns", nestedPath);
      normalizeArray(json_t::string, value["adjectives"], "adjectives", nestedPath);
      normalizeCondition(value["take-condition"], "take", nestedPath);
      normalizeItems(value["items"], nestedPath);
      normalizeCommonItemReferences(value["common-items"], nestedPath);
      setPath(value, nestedPath);
      
      updateDictionary(value["nouns"].get<std::vector<std::string>>());      
      addProxy(key, value, ObjectType::CommonItem);
    }
    setPath(obj, path);
  }

  void processLocationsFile(json &obj, std::string const &path) {
    if (not obj.is_object())
      throw Exception::InvalidFieldType(path, "locations", "object", obj.type_name());

    for (auto &[key, value]: obj.items()) {
      std::string nestedPath = path + "::" + key;
      normalizeObject(value, nestedPath,
		      Field::Required("description"),
		      Field::Required(json_t::string, "label"),
		      Field::Optional(json_t::object, "state"),
		      Field::Optional(json_t::object, "items"),
		      Field::Optional(json_t::array,  "nouns"),	
		      Field::Optional(json_t::array,  "adjectives"),
		      Field::Optional(json_t::object, "common-items"),
		      Field::Optional(json_t::object, "interactions")
		      );

      normalizeDescription(value["description"], nestedPath);
      normalizeInspect(value["inspect"], nestedPath);
      normalizeItems(value["items"], nestedPath);
      normalizeArray(json_t::string, value["nouns"], "nouns", nestedPath);
      normalizeArray(json_t::string, value["adjectives"], "adjectives", nestedPath);
      normalizeCommonItemReferences(value["common-items"], nestedPath);
      normalizeInteractions(value["interactions"], nestedPath);
      setPath(value, nestedPath);

      updateDictionary(value["nouns"].get<std::vector<std::string>>());      
      addProxy(key, value, ObjectType::Location);
    }
    setPath(obj, path);
  }

  void processWorldFile(json &obj, std::string const &path) {
    if (not obj.is_object())
      throw Exception::InvalidFieldType(path, "world", "object", obj.type_name());

    normalizeObject(obj, path,
		    Field::Required(json_t::string, "start"),
		    Field::Required(json_t::array, "connections"));

    normalizeConnections(obj["connections"], path);
    
    Global::g_objectManager.setConnections(obj["connections"]);
    Global::g_objectManager.setStart(obj["start"], path);
  }

  void initDictionary(json &obj, std::string const &) {
    Global::g_dict = Dictionary(obj);
  }
  
  void process(void (*func)(json&, std::string const &), json &root, std::string rootPath, std::string const &part) {
    std::filesystem::path path = rootPath;
    if (not root.contains("files")) {
      throw Exception::ExpectedField(rootPath, "files");
    }

    if (not root["files"].contains(part)) {
      throw Exception::ExpectedField(rootPath + "::files", part);
    }
    
    std::string file = root["files"][part];
    path.replace_filename(file);
    json data = parseEntireFile(path);
    func(data, path);

    //    std::cout << data.dump(2) << '\n';
  }
} // unnamed namespace 


void Game::load(std::string const &rootFilename) {
  std::filesystem::path const rootPath(rootFilename);
  json root = parseEntireFile(rootPath);
  process(initDictionary, root, rootPath, "dictionary");
  process(processPlayerFile, root, rootPath, "player");
  process(processCommonItemsFile, root, rootPath, "common");
  process(processLocationsFile, root, rootPath, "locations");
  process(processWorldFile, root, rootPath, "world");
  Global::g_objectManager.init();
}
