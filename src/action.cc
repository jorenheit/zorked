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
  struct MultipleItemsByThatNoun {};

  template <typename ... Objects>
  Item *findItemImpl(ItemDescriptor const &object, Item *result, ZObject *first, Objects ... rest) {
    for (Item *item: first->items()) {
      if (item->match(object)) {
	if (!result) result = item;
	else if (not result->common()) throw MultipleItemsByThatNoun{};
      }
    }
    
    if constexpr (sizeof ... (rest) > 0) {
      return findItemImpl(object, result, rest ...);
    }
    else return result;
  }

  template <typename ... Objects>
  Item *findItem(ItemDescriptor const &object, ZObject *first, Objects&& ... rest) {
    return findItemImpl(object, nullptr, first, std::forward<Objects>(rest)...);
  }
} // namespace Impl


std::string Nop::exec() const {
  return "";
}

std::string Move::exec() const {
  assert(_dir != NumDir && "dir should be valid here");

  Player *player = Global::g_objectManager.player();
  auto const &[location, condition] = player->getLocation()->connection(_dir);

  if (not location) {
    return Narration::cannot_go_there(directionToString(_dir));
  }
  if (not condition->eval())
    return condition->failString();
  
  //  location->clearMoveCondition(_dir);
  condition->clear();
  player->setLocation(location);
  return condition->successString();
}

std::string Take::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");

  Player *player = Global::g_objectManager.player();
  if (_prepObject.noun.empty()) {
    try {
      auto targetItem = Impl::findItem(_object, player->getLocation());
      if (!targetItem) {
	return Impl::findItem(_object, player)
	  ? Narration::you_already_own(_object.str())
	  : Narration::there_is_no(_object.str());
      }
      
      if (not targetItem->portable()) {
	return Narration::not_portable(_object.str());
      }
      
      auto [result, str] = targetItem->checkTakeCondition();
      if (result) {
	targetItem->clearTakeCondition();
	player->getLocation()->removeItem(targetItem);
	player->addItem(targetItem);
	return (not str.empty())
	  ? str
	  : Narration::you_took(_object.str());
      }
      else {
	return (not str.empty())
	  ? str
	  : Narration::you_could_not_take(_object.str());
      }
    }
    catch (Impl::MultipleItemsByThatNoun) {
      return Narration::which_one(_object.str());
    }
  }
  else {
    try {
      Item *enclosingItem = Impl::findItem(_prepObject, player, player->getLocation());
      if (!enclosingItem) return Narration::there_is_no(_prepObject.str());

      try {
	Item *targetItem = Impl::findItem(_object, enclosingItem);
	if (!targetItem)
	  return Narration::does_not_contain(_prepObject.str(), _object.str());

	auto [result, str] = targetItem->checkTakeCondition();
	if (result) {
	  targetItem->clearTakeCondition(); // TODO: add "persistent" field to condition -> determines if cleared after success
	  enclosingItem->removeItem(targetItem);
	  player->addItem(targetItem);

	  return (not str.empty())
	    ? str
	    : Narration::you_took_from(_object.str(), _prepObject.str());
	}
	else {
	  return (not str.empty())
	    ? str
	    : Narration::you_could_not_take_from(_object.str(), _prepObject.str());
	}
      }
      catch (Impl::MultipleItemsByThatNoun) {
	return Narration::which_one(_object.str());
      }
    }
    catch (Impl::MultipleItemsByThatNoun) {
      return Narration::which_one(_object.str());
    }
  }
  UNREACHABLE();
}


std::string Drop::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");
  Player *player = Global::g_objectManager.player();

  try {
    auto targetItem = Impl::findItem(_object, player);
    if (!targetItem) {
      return Narration::you_do_not_own(_object.str());
    }
    else {
      player->removeItem(targetItem);
      player->getLocation()->addItem(targetItem);
      return Narration::you_dropped(_object.str());
    }
  }
  catch (Impl::MultipleItemsByThatNoun) {
    return Narration::which_one(_object.str());
  }
  UNREACHABLE();
}

std::string Inspect::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");
  Player *player = Global::g_objectManager.player();

  try {
    Item *targetItem = Impl::findItem(_object, player, player->getLocation());
    return targetItem
      ? targetItem->inspect()
      : Narration::there_is_no(_object.str());
  }
  catch (Impl::MultipleItemsByThatNoun) {
    return Narration::which_one(_object.str());
  }
  UNREACHABLE();
}

std::string ShowInventory::exec() const {
  Player *player = Global::g_objectManager.player();
  
  if (player->items().empty()) return Narration::empty_inventory();
  
  std::vector<std::string> itemLabels;
  for (Item *item: player->items()) {
    itemLabels.push_back(item->label());
  }
  return Narration::list_items("inventory", itemLabels);
}

std::string Interact::exec() const {
  Player *player = Global::g_objectManager.player();

  ZObject *target = nullptr;
  try {
    target = _object.str().empty()
      ? static_cast<ZObject*>(player)
      : Impl::findItem(_object, player, player->getLocation());
  }
  catch (Impl::MultipleItemsByThatNoun) {
    return Narration::which_one(_object.str());
  }
  
  if (target == nullptr) {
    // TODO: Check if the object refers to a location
    // NEED: ItemDescriptor -> ObjectDescriptor to also work for locations
    // NEED: dynamically update dictionary with multi-word phrases like "living room" and "trash bag"
    return Narration::there_is_no(_object.str());
  }

  Item const *tool = nullptr;
  if (not _tool.str().empty()) {
    try {
      tool = Impl::findItem(_tool, player);
    }
    catch (Impl::MultipleItemsByThatNoun) {
      return Narration::which_one(_tool.str());
    }
  }
  
  if (not _tool.str().empty() && not tool) {
    return Narration::you_do_not_own(_tool.str());
  }

  Interaction *match = nullptr;
  for (auto const &interactionPtr: target->interactions()) {
    for (std::string const &verb: interactionPtr->verbs()) {
      if (_verb == verb && tool == interactionPtr->tool()) {
	match = interactionPtr.get();
	break;
      }
    }
    if (match) break;
  }
  
  if (not match) {
    return Narration::dont_know_how_to(_verb, _object.str(), _tool.str());
  }

  Condition const *condition = match->condition();
  bool const result = condition->eval();
  for (auto const &effect: match->effects(result)) {
    effect->apply();
  }
  return condition->resultString(result);
}

std::string Save::exec() const {
  return "SAVING";
}

std::string Load::exec() const {
  return "LOADING";
}

