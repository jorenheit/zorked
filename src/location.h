#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <array>
#include <utility>
#include <memory>

#include "direction.h"
#include "condition.h"
#include "zobject.h"
#include "jsonobject.h"

class Location: public ZObject {
  using Connection =  std::pair<std::shared_ptr<Location>, size_t>;
  std::array<Connection, NumDir> _connections;
  bool _visited = false;
  
public:
  Location(ZObject const &zObj);
  void connect(Direction dir, std::shared_ptr<Location> loc, size_t conditionIndex);
  Connection connected(Direction dir);  
  void visit();
  void reset();  
  bool visited() const;
  void clearMoveCondition(Direction dir);
  
  static std::shared_ptr<Location> construct(std::string const &id, JSONObject const &jsonObj);
  void to_json(json &jsonObj) const;
  bool restore(json const &jsonObj);
};

inline void to_json(json &jsonObj, Location const &loc) {
  loc.to_json(jsonObj);
}

inline void to_json(json &jsonObj, std::shared_ptr<Location> ptr) {
  jsonObj = ptr->id();
}




#endif //LOCATION_H
