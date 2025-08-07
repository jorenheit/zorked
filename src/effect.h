#ifndef EFFECT_H
#define EFFECT_H
#include <functional>
#include <string>
#include <memory>
#include "json_fwd.hpp"
#include "objectmanager.h"

class ZObject;
class Item;

class Effect {
  std::function<void()> _effect;
  
public:
  enum ModifierType {
    Equal,
    Add,
    Sub,
    Div,
    Mul
  };
  
  Effect(ObjectPointer target, std::string state, bool value);
  Effect(ObjectPointer target, ObjectPointer item, bool value);
  Effect(ObjectPointer target, ObjectPointer item, ModifierType type, size_t n);
  // TODO: Effect on custom properties

  inline void apply() const { _effect(); }
  static std::unique_ptr<Effect> construct(std::string const &key, nlohmann::json const &value, std::string const &path);
};

#endif
