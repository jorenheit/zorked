#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "json_fwd.hpp"
#include "zobject.h"

class Location;

class Player: public ZObject {
  Location *_location;
  double _health;

public:
  Player(ZObject &&parent, double health);

  void setLocation(Location *loc);
  Location *getLocation() const;

  inline std::string const &name() const { return this->label(); }
  inline void damage(double amount) { _health -= amount; };
  inline void heal(double amount) { _health += amount; };
  inline double getHealth() const { return _health; };

  static std::unique_ptr<Player> construct(nlohmann::json const &obj);
  void to_json(nlohmann::json &jsonObj) const;
  bool restore(nlohmann::json const &jsonObj);
};

inline void to_json(nlohmann::json &jsonObj, Player const &player) {
  player.to_json(jsonObj);
}

#endif // PLAYER_H
