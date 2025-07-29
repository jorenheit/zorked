#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <vector>

#include "player.h"
#include "location.h"
#include "item.h"
#include "condition.h"

namespace Game {

  extern std::shared_ptr<Player> g_player;
  extern std::unordered_map<std::string, std::shared_ptr<Location>> g_locations;
  extern std::unordered_map<std::string, std::shared_ptr<Item>> g_localItems;
  extern std::unordered_map<std::string, std::shared_ptr<Item>> g_commonItems;
  extern ConditionManager g_conditions;
  
  void load(std::string const &rootFilename);
  void play();
  void save(std::string const &saveFilename);
  void restore(std::string const &loadFilename);

  std::shared_ptr<ZObject> objectByID(std::string const &id);  
  std::shared_ptr<Item> commonItemByID(std::string const &id);
  std::shared_ptr<Item> localItemByID(std::string const &id);  
  std::shared_ptr<Location> locationByID(std::string const &id);


} // namespace Global

#endif //GLOBAL_H
