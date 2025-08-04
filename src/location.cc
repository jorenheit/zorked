#include "location.h"
#include "item.h"
#include "player.h"
#include "game.h"
#include "util.h"
#include "condition.h"
#include "json.hpp"

using json = nlohmann::json;

std::unique_ptr<Location> Location::construct(std::string const &id, json const &obj) {
  std::unique_ptr<ZObject> zObj = ZObject::construct(id, obj);
  return std::make_unique<Location>(std::move(*zObj));
}

Location::Location(ZObject &&zObj):
  ZObject(std::move(zObj))
{}

void Location::connect(Direction dir, Location *loc, std::shared_ptr<Condition> cond) {
  _connections[dir] = {loc, std::move(cond)};
}

Location::Connection Location::connection(Direction dir) {
  return _connections[dir];
}

void Location::clearMoveCondition(Direction dir) {
  auto const &[loc, condition] = _connections[dir];
  if (loc) condition->clear();
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
  jsonObj = json::object();
  // {
  //   {"visited", _visited},
  //   {"items", ZObject::items()},
  //   {"state", ZObject::state()}
  // };

  // json connections = json::object();
  // for (size_t idx = 0; idx != NumDir; ++idx) {
  //   if (_connections[idx].second) {
  //     std::string dir = directionToString(static_cast<Direction>(idx));
  //     connections[dir] = _connections[idx].second->empty();
  //   }
  // }
  // jsonObj["move-cleared"] = std::move(connections);
  std::cerr << "TODO: implement Location::to_json\n";
  std::exit(1);
}


bool Location::restore(json const &jsonObj) {
  std::cerr << "TODO: implement Location::restore\n";
  std::exit(1);
  
  _visited = jsonObj.at("visited");
  return ZObject::restore(jsonObj);
  // TODO: error handling (should never error with correct file)
}


void to_json(nlohmann::json &jsonObj, Location const &loc) {
  return loc.to_json(jsonObj);
}

void to_json(nlohmann::json &jsonObj, std::shared_ptr<Location> const &ptr) {
  jsonObj = ptr->id();
}
