#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <vector>

#include "player.h"
#include "location.h"
#include "item.h"
#include "condition.h"

namespace Game {

  class ConditionManager {
    std::vector<std::shared_ptr<Condition>> _conditions;
    std::vector<JSONObject> _conditionProxies;
    bool _initialized = false;
    
  public:
    size_t add(JSONObject const &obj) {
      _conditionProxies.push_back(obj);
      return _conditionProxies.size() - 1;
    }

    Condition &get(size_t index) {
      assert(_initialized && "Calling get before process.");
      assert(index < _conditions.size() && "index out of bounds");
      return *_conditions[index];
    }

    void process() {
      for (JSONObject const &jsonObj: _conditionProxies) {
	_conditions.emplace_back(Condition::construct(jsonObj));
      }
      _initialized = true;
    }
  };
  
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
