#include "game.h"
#include "location.h"
#include "item.h"
#include "util.h"

std::shared_ptr<Location> Location::construct(std::string const &id, JSONObject const &jsonObj) {
  return std::make_shared<Location>(*ZObject::construct(id, jsonObj));
}

Location::Location(ZObject const &zObj):
  ZObject(zObj)
{}

void Location::connect(Direction dir, std::shared_ptr<Location> loc, size_t conditionIndex) {
  _connections[dir] = {loc, conditionIndex};
}

Location::Connection Location::connected(Direction dir) {
  return _connections[dir];
}

void Location::clearMoveCondition(Direction dir) {
  auto [loc, conditionIndex] = _connections[dir];
  if (loc) Game::g_conditions.get(conditionIndex).clear();
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

void Location::to_json(json &jsonObj) const {
  jsonObj = json {
    {"visited", _visited},
    {"items", ZObject::items()},
    {"state", ZObject::state()}
  };

  json connections = json::object();
  for (size_t idx = 0; idx != NumDir; ++idx) {
    if (_connections[idx].second) {
      std::string dir = directionToString(static_cast<Direction>(idx));
      connections[dir] = Game::g_conditions.get(_connections[idx].second).empty();
    }
  }
  jsonObj["move-cleared"] = std::move(connections);
}


bool Location::restore(json const &jsonObj) {
  _visited = jsonObj.at("visited");
  return ZObject::restore(jsonObj);
  // TODO: error handling (should never error with correct file)
}
