#include "location.h"
#include "util.h"

std::shared_ptr<Location> Location::construct(std::string const &id, JSONObject const &jsonObj) {
  return std::make_shared<Location>(*ZObject::construct(id, jsonObj));
}

Location::Location(ZObject const &zObj):
  ZObject(zObj)
{}

void Location::connect(Direction dir, std::shared_ptr<Location> loc, Condition const &cond) {
  _connections[dir] = {loc, cond};
}

std::pair<std::shared_ptr<Location>, Condition> Location::connected(Direction dir) {
  return _connections[dir];
}

void Location::clearMoveCondition(Direction dir) {
  auto [loc, cond] = _connections[dir];
  if (loc) cond.clear();
}

void Location::visit() {
  _visited = true;
}

void Location::reset() {
  _visited = false;
}
  
bool Location::visited() const {
  return _visited;
}


