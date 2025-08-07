#include "util.h"
#include "item.h"
#include "effect.h"
#include "exception.h"
#include "game.h"
#include "json.hpp"

using json = nlohmann::json;

std::unique_ptr<Effect> Effect::construct(std::string const &key, nlohmann::json const &value, std::string const &path) {
  std::string currentPath = path + "::" + key;
  auto const vec = split(key, '.');
  if (vec.size() < 3) {
    throw Exception::SpecificFormatError(currentPath, "Invalid effect specifier: ", key);
  }

  ObjectPointer target = Global::g_objectManager.get(vec[0]);
  if (!target) {
    throw Exception::UndefinedReference(currentPath, key);
  } 
    
  if (vec[1] == "is") {
    if (not value.is_boolean()) {
      throw Exception::InvalidFieldType(currentPath, key, "boolean", value.type_name());
    }
    return std::make_unique<Effect>(target, vec[2], value.get<bool>());
  }
  else if (vec[1] == "has") {
    ObjectPointer item = Global::g_objectManager.get(vec[2]);
    if (not item) {
      throw Exception::UndefinedReference(currentPath, vec[2]);
    }
      
    if (value.is_boolean()) {
      if (vec.size() != 3) {
	throw Exception::SpecificFormatError(currentPath, "A 'has'-effect that takes a boolean value should be of the form '[object].has.[item]: [value]'");
      }
      return std::make_unique<Effect>(target, item, value.get<bool>());	
    }
    else if (value.is_number_unsigned()) {
      if (vec.size() != 4 || (std::string("=+-*/").find(vec[3][0]) == std::string::npos)) {
	throw Exception::SpecificFormatError(currentPath, "A 'has'-effect that takes a number-value should be of the form '[object].has.[item].[=,+,-,*,/]: [value]'");
      }
      static std::unordered_map<char, ModifierType> const charToModifierType = {
	{'=', Equal},
	{'+', Add},
	{'-', Sub},
	{'*', Mul},
	{'/', Div}
      };

      char op = vec[3][0];
      if (not charToModifierType.contains(op)) {
	throw Exception::SpecificFormatError(currentPath, "Invalid operation: '", op, "', must be one of {=, +, -, *, /}.");
      }
      if (not item.get<Item*>()->common()) {
	throw Exception::SpecificFormatError(currentPath, "'has'-effects can only apply to common items.");
      }
      return std::make_unique<Effect>(target, item, charToModifierType.at(op), value.get<size_t>());
    }
    else {
      throw Exception::SpecificFormatError(currentPath, "Invalid value-type; must be either a boolean or unsigned integer.");
    }
  }

  UNREACHABLE("END OF FUNCTION");
}

Effect::Effect(ObjectPointer target, std::string state, bool value) {
  assert(target != nullptr);

  _effect = [=]() mutable {
    target->setState(state, value);
  };
}

// TODO: Effect on custom properties

Effect::Effect(ObjectPointer target, ObjectPointer item, bool value) {
  assert(target);
  assert(item);

  _effect = [=]() mutable {
    Item *itemPtr = item.get<Item*>();
    if (value) {
      if (not target->contains(itemPtr))
	target->addItem(itemPtr);
    }
    else {
      while (target->contains(itemPtr)) {
	target->removeItem(itemPtr);
      }
    }
  };
}
  
Effect::Effect(ObjectPointer target, ObjectPointer item, ModifierType type, size_t n) {
  assert(target);
  assert(item);

  _effect = [=]() mutable {
    Item *itemPtr = item.get<Item*>();
    int const currentAmount = target->contains(itemPtr);
    
    auto const setItems = [&](int m) {
      int diff = m - currentAmount;
      for (size_t i = 0; i != std::abs(diff); ++i) {
	if (diff > 0) target->addItem(itemPtr);
	else if (not target->removeItem(itemPtr)) break;
      }
    };

    switch (type) {
    case Equal: setItems(n); break;
    case Add:   setItems(currentAmount + n); break;
    case Sub:   setItems(currentAmount - n); break;
    case Mul:   setItems(currentAmount * n); break;
    case Div:   setItems(currentAmount / n); break;
    default: UNREACHABLE("default");
    }
  };
}

