#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <array>
#include <utility>
#include <memory>
#include "json_fwd.hpp"

#include "zobject.h"
#include "direction.h"

class Condition;

class Location: public ZObject {
  using Connection =  std::pair<Location*, std::shared_ptr<Condition>>;
  std::array<Connection, NumDir> _connections;
  bool _visited = false;
  
public:
  Location(ZObject &&zObj);
  void connect(Direction dir, Location *loc, std::shared_ptr<Condition> cond);
  Connection connection(Direction dir);  
  void visit();
  void reset();  
  bool visited() const;
  void clearMoveCondition(Direction dir);

  static std::unique_ptr<Location> construct(std::string const &id, nlohmann::json const &obj);
  void to_json(nlohmann::json &jsonObj) const;
  bool restore(nlohmann::json const &jsonObj);
};

void to_json(nlohmann::json &jsonObj, Location const &loc);
void to_json(nlohmann::json &jsonObj, std::shared_ptr<Location> ptr);



#endif //LOCATION_H
