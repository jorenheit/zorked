#include <filesystem>
#include <fstream>
#include <iostream>

#include "exception.h"
#include "objectmanager.h"
#include "preprocessor.h"
#include "game.h"
#include "dictionary.h"
#include "json.hpp"
using json = nlohmann::json;

namespace {

  //TODO: deal with int/double better
  
  json parseEntireFile2(std::filesystem::path const &filename) {
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
  
  void normalizeField(json &obj, std::string const &path, Field::Required const &field) {
    assert(obj.type() == json_t::object);
    if (!obj.contains(field.key)) {
      std::cerr << path << ": expected field: " << field.key << '\n';
      std::exit(1); // TODO: Exception
    }
    if (field.type != json_t::null && obj.at(field.key).type() != field.type) {
      std::cerr << path << "::" << field.key << ": expected field type: " << json(field.type).type_name() << '\n';
      std::exit(1); // TODO: Exception
    }
  }

  void normalizeField(json &obj, std::string const &path, Field::Optional const &field) {
    assert(obj.type() == json_t::object);
    if (!obj.contains(field.key)) {
      obj[field.key] = field.value;
    }
    else if (obj.at(field.key).type() != field.type) {
      std::cerr << path << "::" << field.key << ": expected field type: " << json(field.type).type_name() << '\n';
      std::exit(1); // TODO: Exception
    }
  }

  void normalizeArray(json::value_t type, json &obj, std::string const &path) {
    assert(obj.is_array());
    for (json &element: obj) {
      if (element.type() != type)
	std::cerr << path << ": Expected array of type " << json(type).type_name() << '\n';
    }
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
		    Field::Optional(json_t::string, "success"),
		    Field::Optional(json_t::string, "fail")
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

  void normalizeDescription(json &obj, std::string const &path) {
    std::string currentPath = path + "::description";
    if (obj.is_string()) {
      std::string descr = obj;
      obj = json {{"lore-condition", json {{"success", descr}, {"fail", ""}}}};
    }
    else if (!obj.is_object()) {
      std::cerr << path << ", " << obj.type_name() << '\n';
      std::exit(0);
      // TODO: exception
    }

    normalizeObject(obj, currentPath, Field::Required(json_t::object, "lore-condition"));
    normalizeCondition(obj["lore-condition"], "lore", currentPath);
    setPath(obj, currentPath);
  }
  
  void normalizeState(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object);
    for (auto &[key, value]: obj.items()) {
      if (value.type() != json_t::boolean) {
	std::cerr << path << ", " << key << ": Expected boolean.\n"; // TODO: Exception
	std::exit(0);
      }
    }

    setPath(obj, path);
  }

  void normalizeInspect(json &obj, std::string const &path) {
    std::string currentPath = path + "::inspect";
    if (obj.is_string() || obj.is_null()) {
      std::string descr = obj.is_null() ? "" : obj.get<std::string>();
      obj = json {{"inspect-condition", json {{"success", descr}, {"fail", ""}}}};
    }
    else if (!obj.is_object()) {
      std::cerr << path << ", " << obj.type_name() << '\n';
      std::exit(0);
      // TODO: exception
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
	std::cerr << currentPath << "::" << key << ": expected number, got " << value.type_name() << '\n';
	std::exit(0);
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
		      Field::Optional(json_t::object, "common-items")
		      );

      normalizeDescription(value["description"], nestedPath);
      normalizeInspect(value["inspect"], nestedPath);
      
      normalizeArray(json_t::string, value["nouns"], nestedPath);
      normalizeArray(json_t::string, value["adjectives"], nestedPath);
      normalizeCondition(value["take-condition"], "take", nestedPath);
      normalizeItems(value["items"], nestedPath);
      normalizeCommonItemReferences(value["common-items"], nestedPath);
      normalizeState(value["state"], nestedPath);
      setPath(value, nestedPath);
      Game::g_objectManager.addProxy(key, value, ObjectType::LocalItem);
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
      Game::g_objectManager.setConnections(obj);
    }
  }

  void processPlayer(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object); // TODO: exception
    normalizeObject(obj, path,
		    Field::Required("description"),		 
		    Field::Required(json_t::string, "label"),
		    Field::Optional(json_t::number_float, "health", 100.0),
		    Field::Optional(json_t::object, "state"),
		    Field::Optional(json_t::object, "items"),
		    Field::Optional(json_t::array, "nouns")
		    );

    normalizeDescription(obj["description"], path);
    normalizeInspect(obj["inspect"], path);
    
    normalizeArray(json_t::string, obj["nouns"], path);
    normalizeItems(obj["items"], path);
    setPath(obj, path);
    Game::g_objectManager.setPlayer(obj);
  }

  void processCommonItems(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object); // TODO: exception

    for (auto &[key, value]: obj.items()) {
      if (value.contains("state")) {
	std::cerr << "Common item may not contain state.\n";
	std::exit(0); // TODO: exception
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
		      Field::Optional(json_t::object, "take-condition")
		      );

      normalizeDescription(value["description"], nestedPath);
      normalizeInspect(value["inspect"], nestedPath);

      normalizeArray(json_t::string, value["nouns"], nestedPath);
      normalizeArray(json_t::string, value["adjectives"], nestedPath);
      normalizeCondition(value["take-condition"], "take", nestedPath);
      normalizeItems(value["items"], nestedPath);
      normalizeCommonItemReferences(value["common-items"], nestedPath);
      setPath(value, nestedPath);
      Game::g_objectManager.addProxy(key, value, ObjectType::CommonItem);
    }
    setPath(obj, path);
  }

  void processLocations(json &obj, std::string const &path) {
    assert(obj.type() == json_t::object); // TODO: exception

    for (auto &[key, value]: obj.items()) {
      std::string nestedPath = path + "::" + key;
      normalizeObject(value, nestedPath,
		      Field::Required("description"),
		      Field::Required(json_t::string, "label"),
		      Field::Optional(json_t::object, "state"),
		      Field::Optional(json_t::object, "items"),
		      Field::Optional(json_t::array,  "nouns"),
		      Field::Optional(json_t::object, "common-items")
		      );

      normalizeDescription(value["description"], nestedPath);
      normalizeInspect(value["inspect"], nestedPath);
      normalizeItems(value["items"], nestedPath);
      normalizeArray(json_t::string, value["nouns"], nestedPath);
      normalizeCommonItemReferences(value["common-items"], nestedPath);
      setPath(value, nestedPath);
      Game::g_objectManager.addProxy(key, value, ObjectType::Location);
    }
    setPath(obj, path);
  }

  void processWorld(json &obj, std::string const &path) {
    assert(obj.is_object()); // TODO: exception
    normalizeObject(obj, path,
		    Field::Required(json_t::string, "start"),
		    Field::Required(json_t::array, "connections"));

    normalizeConnections(obj["connections"], path);
    
    Game::g_objectManager.setConnections(obj["connections"]);
    Game::g_objectManager.setStart(obj["start"]);
  }
  
};


json preprocess(std::string const &rootFilename) {
  std::filesystem::path const rootPath(rootFilename);
  json root = parseEntireFile2(rootPath);

  // TODO: normalize root
  json files = root["files"];
  std::string playerFilename = files["player"];
  std::string itemFilename = files["items"];
  std::string locationsFilename = files["locations"];
  std::string worldFilename = files["world"];
  std::string dictionaryFilename = files["dictionary"];

  // Initialize dictionary
  std::filesystem::path dictionaryPath = rootPath;
  dictionaryPath.replace_filename(dictionaryFilename);
  Game::g_dict = Dictionary(dictionaryPath);

  // Load player data
  std::filesystem::path playerPath = rootPath;
  playerPath.replace_filename(playerFilename);
  json playerData = parseEntireFile2(playerPath)["player"];
  processPlayer(playerData, playerFilename);

  // Load common (stateless) items
  std::filesystem::path itemPath = rootPath;
  itemPath.replace_filename(itemFilename); 
  json itemData = parseEntireFile2(itemPath);
  processCommonItems(itemData, itemFilename);

  // Load locations and their items
  std::filesystem::path locPath = rootPath;
  locPath.replace_filename(locationsFilename); 
  json locData = parseEntireFile2(locPath);
  processLocations(locData, locationsFilename);

  // Load world
  std::filesystem::path worldPath = rootPath;
  worldPath.replace_filename(worldFilename);
  json worldData = parseEntireFile2(worldPath);
  processWorld(worldData, worldFilename);


  // Construct objects
  
  json result;
  result["player"] = playerData;
  result["common"] = itemData;
  result["locations"] = locData;
  result["world"] = worldData;


  return result;
}
