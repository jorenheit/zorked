#ifndef INTERACTION_H
#define INTERACTION_H

#include <vector>
#include <string>
#include "json_fwd.hpp"
#include "objectmanager.h"
#include "condition.h"
#include "effect.h"

class Item;

class Interaction {

  Item const *_tool;
  
  std::vector<std::string> _verbs;
  std::unique_ptr<Condition> _condition;
  std::vector<std::unique_ptr<Effect>> _successEffects;
  std::vector<std::unique_ptr<Effect>> _failEffects;
  
public:
  Interaction() = default;
  Interaction(Item *tool,
	      std::vector<std::string> const &verbs,
	      std::unique_ptr<Condition> &&condition,
	      std::vector<std::unique_ptr<Effect>> &&successEffects,
	      std::vector<std::unique_ptr<Effect>> &&failEffects);
	      

  inline std::vector<std::string> const &verbs() const { return _verbs; }
  inline Item const *tool() const { return _tool; }
  inline Condition const *condition() const { return _condition.get(); }
  inline std::vector<std::unique_ptr<Effect>> const &successEffects() const { return _successEffects; }
  inline std::vector<std::unique_ptr<Effect>> const &failEffects() const { return _failEffects; }
  inline std::vector<std::unique_ptr<Effect>> const &effects(bool result) const { return result ? _successEffects : _failEffects; }
  

  static std::unique_ptr<Interaction> construct(std::string const &key, nlohmann::json const &obj);
};

#endif // INTERACTION_H
