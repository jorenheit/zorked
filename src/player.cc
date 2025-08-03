#include <iostream>
#include "player.h"
#include "item.h"
#include "condition.h"
#include "json.hpp"

using json = nlohmann::json;


std::unique_ptr<Player> Player::construct(json const &obj) {
  std::unique_ptr<ZObject> zObj = ZObject::construct("player", obj);
  auto health = obj.at("health").get<double>();
  return std::make_unique<Player>(std::move(*zObj), health);
}

Player::Player(ZObject &&parent, double health):
  ZObject(std::move(parent)),
  _health(health)
{}

void Player::setLocation(Location *loc) {
  assert(loc);
  _location = loc;
}

Location *Player::getLocation() const {
  return _location;
}

void Player::to_json(json &jsonObj) const {
  std::cerr << "TODO: implement Player::to_json\n";
  std::exit(1);
  // jsonObj = json {
  //   {"health", _health},
  //   {"items", ZObject::items()},
  //   {"state", ZObject::state()}
  // };
}

bool Player::restore(json const &jsonObj) {
  std::cerr << "TODO: implement Player::restore()\n";
  std::exit(1);
  
  // _health = jsonObj.at("health");
  // return ZObject::restore(jsonObj);
  return false;
}
