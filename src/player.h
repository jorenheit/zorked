#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "zobject.h"
#include "location.h"

class Player: public ZObject {
  std::shared_ptr<Location> _location;
  double _health;

public:
  Player(ZObject const &zObj, double health);

  void setLocation(std::shared_ptr<Location> loc);
  std::shared_ptr<Location> getLocation() const;

  inline std::string const &name() const { return this->label(); }
  inline void damage(double amount) { _health -= amount; };
  inline void heal(double amount) { _health += amount; };
  inline double getHealth() const { return _health; };

  static std::shared_ptr<Player> construct(JSONObject const &jsonObj);
  void to_json(json &jsonObj) const;
  bool restore(json const &jsonObj);
};

inline void to_json(json &jsonObj, Player const &player) {
  player.to_json(jsonObj);
}

#endif // PLAYER_H
