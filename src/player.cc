#include "player.h"
#include "item.h"

std::shared_ptr<Player> Player::construct(JSONObject const &jsonObj) {
  auto ptr = ZObject::construct("player", jsonObj);
  double health = jsonObj.getOrDefault<double>("health", 100);
  
  return std::make_shared<Player>(*ptr, health);
}

Player::Player(ZObject const &zObj, double health):
  ZObject(zObj),
  _health(health)
{}

void Player::setLocation(std::shared_ptr<Location> loc) {
  _location = loc;
}

std::shared_ptr<Location> Player::getLocation() const {
  return _location;
}

void Player::to_json(json &jsonObj) const {
  jsonObj = json {
    {"health", _health},
    {"items", ZObject::items()},
    {"state", ZObject::state()}
  };
}

bool Player::restore(json const &jsonObj) {
  _health = jsonObj.at("health");
  return ZObject::restore(jsonObj);
}
