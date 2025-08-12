#include "action.h"

#include "game.h"
#include "player.h"
#include "location.h"
#include "condition.h"
#include "narration.h"
#include "dictionary.h"
#include "objectmanager.h"
#include "interaction.h"

namespace Impl {
  // TODO: gotta rethink this. When executing a "take all", we need all hits, so multiple different items that match the same
  // description is not an "error" anymore.

  struct MultipleItemsByThatNoun {};
  
  template <typename ... Objects>
  void findItemImpl(ObjectDescriptor const &object, std::pair<Item*, size_t> &result, ZObject *first, Objects ... rest) {
    for (Item *item: first->items()) {
      if (item->match(object)) {
	if (result.first == nullptr) result = {item, 1};
	else if (item == result.first && item->common()) ++result.second;
	else throw MultipleItemsByThatNoun{};
      }
    }
    
    if constexpr (sizeof ... (rest) > 0) {
      return findItemImpl(object, result, rest ...);
    }
  }

  template <typename ... Objects>
  std::pair<Item*, size_t> findItem(ObjectDescriptor const &object, ZObject *first, Objects&& ... rest) {
    std::pair<Item*, size_t> result = {nullptr, 0};
    if (object.str().empty()) return result;
    try {
      findItemImpl(object, result, first, std::forward<Objects>(rest)...);
      return result;
    }
    catch (MultipleItemsByThatNoun) {
      return {result.first, -1};
    }
  }


  std::pair<ZObject*, size_t> findInteractionTarget(ObjectDescriptor const &object, Player *player, Location *location) {
    if (object.str().empty()) return {player, 1};
    auto [target, n] = Impl::findItem(object, player, location);
    if (!target && location->match(object)) return {location, 1};
    return {target, n};
  }

  Interaction *findMatchingInteraction(ZObject const *target, std::string const &verb, Item const *tool) {
    for (auto const &interaction: target->interactions()) {
      if (interaction->match(verb, tool)) {
	return interaction.get();
      }
    }
    return nullptr;
  }

  std::string const &applyInteraction(Interaction *interaction) {
    Condition const *condition = interaction->condition();
    bool const result = condition->eval();
    for (auto const &effect: interaction->effects(result)) {
      effect->apply();
    }
    return condition->resultString(result);
  }
  
  std::string tryTakeItem(ObjectDescriptor const &object, Player *player, ZObject *source,
			  std::string const &you_took, std::string const &not_here, std::string const &take_fail) {
    
    auto [targetItem, n] = Impl::findItem(object, source);
    if (n == -1UL)  return Narration::which_one(object);
    if (!targetItem) {
      Item *inventoryItem = Impl::findItem(object, player).first;
      return (inventoryItem && not inventoryItem->common())
	? Narration::you_already_own(object)
	: not_here;
    }
      
    if (not targetItem->portable()) return Narration::not_portable(object);
    if (n < object.number)          return Narration::only_n_here(object, n);
    
    // TODO: add "persistent" field to condition -> determines if cleared after success
    Condition *takeCondition = targetItem->takeCondition();
    bool result = takeCondition->eval();
    std::string resultString = takeCondition->resultString(result);
    if (result) {
      takeCondition->clear();
      for (size_t count = 0; count != object.number; ++count) {
	source->removeItem(targetItem);
	player->addItem(targetItem);
      }
      return (not resultString.empty())
	? resultString
	: you_took;
    }
    else {
      return (not resultString.empty())
	? resultString
	: take_fail;
    }
  }
} // namespace Impl


std::string Nop::exec() {
  return "";
}

std::string Move::exec() {
  assert(_dir != NumDir && "dir should be valid here");

  Player *player = Global::g_objectManager.player();
  auto const &[location, condition] = player->getLocation()->connection(_dir);

  if (not location) {
    return Narration::cannot_go_there(directionToString(_dir));
  }
  if (not condition->eval())
    return condition->failString();
  
  condition->clear();
  player->setLocation(location);
  return condition->successString();
}

std::string Take::exec() {
  assert(not _object.noun.empty() && "_object should have a value");

  Player *player = Global::g_objectManager.player();
  Location *location = player->getLocation();
  
  if (_prepObject.noun.empty()) {
    return Impl::tryTakeItem(_object, player, location,
			     Narration::you_took(_object),
			     Narration::there_is_no(_object),
			     Narration::you_could_not_take(_object)
			     );
  }
  else {
    auto [enclosingItem, n]  = Impl::findItem(_prepObject, player, location);
    if (!enclosingItem) return Narration::there_is_no(_prepObject);
    if (n == -1UL)      return Narration::which_one(_prepObject);

    return Impl::tryTakeItem(_object, player, enclosingItem,
			     Narration::you_took_from(_object, _prepObject),
			     Narration::does_not_contain(_prepObject, _object),
			     Narration::you_could_not_take_from(_object, _prepObject)
			     );
  }
}


std::string Drop::exec() {
  assert(not _object.noun.empty() && "_object should have a value");
  Player *player = Global::g_objectManager.player();
  auto [targetItem, n] = Impl::findItem(_object, player);
  std::cout << n << '\n';
  if (!targetItem)        return Narration::you_do_not_own(_object);
  if (n == -1UL)          return Narration::which_one(_object);
  if (n < _object.number) return Narration::you_do_not_own_n(_object);
  else {
    if (_object.number == 0) _object.number = n;
    for (size_t count = 0; count != _object.number; ++count) {
      player->removeItem(targetItem);
      player->getLocation()->addItem(targetItem);
    }
    return Narration::you_dropped(_object);
  }
}

std::string Inspect::exec() {
  assert(not _object.noun.empty() && "_object should have a value");
  Player *player = Global::g_objectManager.player();

  auto [targetItem, n] = Impl::findItem(_object, player, player->getLocation());
  if (n == -1UL) return Narration::which_one(_object);
  if (targetItem) {
    if (_object.number == 0) _object.number = n;
    return (_object.number == 1) 
      ? targetItem->inspect()
      : Narration::can_inspect_one_at_a_time(_object);
  }
  else return Narration::there_is_no(_object);
}

std::string ShowInventory::exec() {
  Player *player = Global::g_objectManager.player();
  
  if (player->items().empty()) return Narration::empty_inventory();
  
  std::vector<std::string> itemLabels;
  for (Item *item: player->items()) {
    itemLabels.push_back(item->label());
  }
  return Narration::list_items("inventory", itemLabels);
}

std::string Interact::exec() {
  Player *player = Global::g_objectManager.player();
  Location *location = player->getLocation();

  // Find object that is interacted with
  auto [object, n1] = Impl::findInteractionTarget(_object, player, location);  
  if (object == nullptr) return Narration::there_is_no(_object);
  if (n1 == -1UL) return Narration::which_one(_object);
  if (_object.number == 0) _object.number = n1;
  if (_object.number > n1) return Narration::you_do_not_own_n(_object);
  
  // Find tool that is used to interact (if any) -> must be owned by player
  auto [tool, n2] = Impl::findItem(_tool, player);
  if (n2 == -1UL) return Narration::which_one(_tool);
  if (not _tool.str().empty() && not tool) {
    return Narration::you_do_not_own(_tool);
  }
  if (_tool.number == 0) _tool.number = n2;
  if (_tool.number > n2) return Narration::you_do_not_own_n(_tool);

  // See if there is a matching interaction for the target
  Interaction *match = Impl::findMatchingInteraction(object, _verb, tool);
  if (not match) {
    return Narration::dont_know_how_to(_verb, _object, _tool);
  }

  // Interaction is possible in principle, but only interactions with
  // single objects and using single tools are supported.
  if (object && _object.number != 1) return Narration::can_interact_with_only_one_object_at_a_time(_verb, _object);
  if (tool && _tool.number != 1)     return Narration::can_interact_with_only_one_tool_at_a_time(_verb, _object, _tool);
  
  // Apply interaction effects
  return Impl::applyInteraction(match);
}

std::string Save::exec() {
  return "SAVING";
}

std::string Load::exec() {
  return "LOADING";
}

