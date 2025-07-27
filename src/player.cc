#include "player.h"

std::shared_ptr<Player> Player::construct(JSONObject const &jsonObj) {
  auto ptr = ZObject::construct("player", jsonObj);
  std::string name = jsonObj.getOrDefault<std::string>("name", "Player");
  double health = jsonObj.getOrDefault<double>("health", 100);
  
  return std::make_shared<Player>(*ptr, name, health);
}

Player::Player(ZObject const &zObj, std::string const &name, double health):
  ZObject(zObj),
  _name(name),
  _health(health)
{}

void Player::setLocation(std::shared_ptr<Location> loc) {
  _location = loc;
}

std::shared_ptr<Location> Player::getLocation() const {
  return _location;
}
