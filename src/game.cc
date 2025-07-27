#include <fstream>
#include <iostream>
#include <filesystem>

#include "exception.h"
#include "game.h"
#include "direction.h"
#include "jsonobject.h"
#include "action.h"
#include "parser.h"

using namespace Game;

void addCommonItemsRecursively(std::string const &id, JSONObject const &jsonObj) {
  if (jsonObj.contains("state")) {
    throw Exception::SpecificFormatError(jsonObj.path(), jsonObj.trace(), "Common items may not have a \"state\" field.");
  }
    
  auto itemPtr = Item::construct(id, true, jsonObj);
  g_commonItems.insert({itemPtr->id(), itemPtr});
  if (not jsonObj.contains("items")) return;
  for (auto const &[key, value]: jsonObj.get<JSONObject>("items")) {
    addCommonItemsRecursively(key, value);
  }
}
 
void addLocalItemsRecursively(std::shared_ptr<ZObject> parent, JSONObject const &jsonObj) {
  if (not jsonObj.contains("items")) return;
  for (auto const &[key, value]: jsonObj.get<JSONObject>("items")) {
    if (value.contains("id")) {
	std::shared_ptr<Item> itemPtr = commonItemByID(value.get<std::string>("id"));
	size_t amount = value.getOrDefault<size_t>("amount", 1);
	for (size_t i = 0; i != amount; ++i) {
	  parent->addItem(itemPtr);
	  addLocalItemsRecursively(itemPtr, value);
	}
    }
    else {
      if (value.contains("amount")) {
	throw Exception::SpecificFormatError(value.path(), value.trace(), "Locally defined object may not have an \"amount\" field.");
      }

      std::shared_ptr<Item> itemPtr = Item::construct(key, false, value);
      g_localItems.insert({itemPtr->id(), itemPtr});
      parent->addItem(itemPtr);
      addLocalItemsRecursively(itemPtr, value);
    }
  }
}

namespace Game {

  Dictionary g_dict;
  std::shared_ptr<Player> g_player;
  
  std::unordered_map<std::string, std::shared_ptr<Location>> g_locations;
  std::unordered_map<std::string, std::shared_ptr<Item>> g_localItems;
  std::unordered_map<std::string, std::shared_ptr<Item>> g_commonItems;

  void load(std::string const &rootFilename) {
    std::filesystem::path const rootPath(rootFilename);
    JSONObject root = parseEntireFile(rootPath);
    auto files = root.get<JSONObject>("files");
    auto dictionaryFilename = files.get<std::string>("dictionary");
    auto playerFilename = files.get<std::string>("player");
    auto itemFilename = files.get<std::string>("items");
    auto locationsFilename = files.get<std::string>("locations");
    auto worldFilename = files.get<std::string>("world");

    // Load dictionary
    std::filesystem::path dictionaryPath = rootPath;
    dictionaryPath.replace_filename(dictionaryFilename);
    g_dict = Dictionary(dictionaryPath);
    
    // Load player data
    std::filesystem::path playerPath = rootPath;
    playerPath.replace_filename(playerFilename);
    JSONObject playerData = parseEntireFile(playerPath).get<JSONObject>("player");

    g_player = Player::construct(playerData);
    addLocalItemsRecursively(g_player, playerData);
    
    // Load common (stateless) items
    std::filesystem::path itemPath = rootPath;
    itemPath.replace_filename(itemFilename); 
    JSONObject itemData = parseEntireFile(itemPath);
    
    for (auto const &[key, value]: itemData) {
      addCommonItemsRecursively(key, value);
    }
    
    // Load locations and their items
    std::filesystem::path locPath = rootPath;
    locPath.replace_filename(locationsFilename); 
    JSONObject locData = parseEntireFile(locPath);
    
    for (auto const &[key, value]: locData) {
      std::shared_ptr<Location> loc = Location::construct(key, value);
      g_locations.insert({loc->id(), loc});
      addLocalItemsRecursively(loc, value);
    }

    // Load world
    std::filesystem::path worldPath = rootPath;
    worldPath.replace_filename(worldFilename);
    JSONObject worldData = parseEntireFile(worldPath);
    
    // Start location
    auto startLocationID = worldData.get<std::string>("start");
    std::shared_ptr<Location> startLocation = locationByID(startLocationID);
    if (not startLocation)
      throw Exception::UndefinedReference(worldData.path(), worldData.trace(), startLocationID);
    g_player->setLocation(startLocation);

    // Connections
    for (auto &conn: worldData.get<std::vector<JSONObject>>("connections")) {

      auto fromID = conn.get<std::string>("from");
      auto toID = conn.get<std::string>("to");
      auto dirStr = conn.get<std::string>("direction");
      Direction dir = directionFromString(dirStr);

      std::shared_ptr<Location> from = locationByID(fromID);
      std::shared_ptr<Location> to = locationByID(toID);
      
      if (not from) throw Exception::UndefinedReference(conn.path(), conn.trace(), fromID);
      if (not to) throw Exception::UndefinedReference(conn.path(), conn.trace(), toID);
      if (dir == NumDir) throw Exception::SpecificFormatError(conn.path(), conn.trace(), "Invalid direction \"", dirStr, "\".");

      if (from->connected(dir).first) {
	throw Exception::DoubleConnectedLocation(conn.path(), conn.trace(),
						 fromID, from->connected(dir).first->id());
      }
      
      Condition cond = Condition::construct(conn.getOrDefault<JSONObject>("move-condition"));

      from->connect(dir, to, cond);
      if (conn.getOrDefault<bool>("symmetric", true)) {
	if (to->connected(oppositeDirection(dir)).first) {
	  throw Exception::DoubleConnectedLocation(conn.path(), conn.trace(),
						   toID, to->connected(oppositeDirection(dir)).first->id());
	}
	to->connect(oppositeDirection(dir), from, cond);
      }
    }
  }

  std::shared_ptr<ZObject> objectByID(std::string const &id) {
    if (id == g_player->id()) return g_player;
    
    std::shared_ptr<ZObject> result = localItemByID(id);
    if (result) return result;

    result = commonItemByID(id);
    if (result) return result;

    result = locationByID(id);
    if (result) return result;

    return nullptr;
  }
  
  std::shared_ptr<Item> localItemByID(std::string const &id) {
    if (g_localItems.contains(id)) return g_localItems.at(id);
    return nullptr;
  }
  
  std::shared_ptr<Item> commonItemByID(std::string const &id) {
    if (g_commonItems.contains(id)) return g_commonItems.at(id);
    return nullptr;
  }
  
  std::shared_ptr<Location> locationByID(std::string const &id) {
    if (g_locations.contains(id)) return g_locations.at(id);
    return nullptr;
  }

  void play() {
    Parser parser(g_dict);
    
    while (true) {
      std::shared_ptr<Location> current = g_player->getLocation();
      std::cout << current->label() << '\n';
      std::cout << current->description() << '\n';
      for (auto const &itemPtr: current->items()) {
	std::cout << itemPtr->description() << '\n';
      }

      std::string input;
      std::getline(std::cin, input);
      std::unique_ptr<Action> action = parser.parseAction(input);
      if (action)
	std::cout << action->exec() << '\n';
      else {
	std::cout << "Wut?\n";
      }
    }
  }
  
} // namespace Game
