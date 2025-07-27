#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "zobject.h"
#include "location.h"

class Player: public ZObject {
  std::shared_ptr<Location> _location;
  std::string _name;
  double _health;

public:
  Player(ZObject const &zObj, std::string const &name, double health);

  void setLocation(std::shared_ptr<Location> loc);
  std::shared_ptr<Location> getLocation() const;

  inline void damage(double amount) { _health -= amount; };
  inline void heal(double amount) { _health += amount; };
  inline double getHealth() const { return _health; };

  static std::shared_ptr<Player> construct(JSONObject const &jsonObj);
};

#endif // PLAYER_H
