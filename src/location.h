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
  std::array<std::pair<std::shared_ptr<Location>, Condition>, NumDir> _connections;
  bool _visited = false;
  
public:
  Location(ZObject const &zObj);
  void connect(Direction dir, std::shared_ptr<Location> loc, Condition const &cond = {});
  std::pair<std::shared_ptr<Location>, Condition> connected(Direction dir);  
  void visit();
  void reset();  
  bool visited() const;

  static std::shared_ptr<Location> construct(std::string const &id, JSONObject const &jsonObj);
};



#endif //LOCATION_H
