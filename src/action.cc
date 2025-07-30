#include "action.h"
#include "game.h"
#include "player.h"
#include "location.h"
#include "condition.h"
#include "narration.h"

Move::Move(Direction dir):
  _dir(dir)
{}

std::string Move::exec() const {
  assert(_dir != NumDir && "dir should be valid here");

  auto [location, conditionIndex] = Game::g_player->getLocation()->connected(_dir);

  if (not location) {
    return Narration::cannot_go_there(directionToString(_dir));
  }

  Condition const &condition = Game::g_conditions.get(conditionIndex);
  if (not condition.eval())
    return condition.failString();
  
  location->clearMoveCondition(_dir);
  Game::g_player->setLocation(location);
  return condition.successString();
}

namespace Impl {
  struct MultipleItemsByThatNoun {};

  template <typename ... Objects>
  std::shared_ptr<Item> findItemImpl(ItemDescriptor const &object, std::shared_ptr<Item> result, std::shared_ptr<ZObject> first, Objects&& ... rest) {
    for (auto const &item: first->items()) {
      if (item->match(object)) {
	if (!result) {
	  result = item;
	}
	else if (not result->common()) {
	  throw MultipleItemsByThatNoun{};
	}
      }
    }
    
    if constexpr (sizeof ... (rest) > 0) {
      return findItemImpl(object, result, std::forward<Objects>(rest)...);
    }
    else return result;
  }

  template <typename ... Objects>
  std::shared_ptr<Item> findItem(ItemDescriptor const &object, std::shared_ptr<ZObject> first, Objects&& ... rest) {
    return findItemImpl(object, nullptr, first, std::forward<Objects>(rest)...);
  }
} // namespace Impl


Take::Take(ItemDescriptor const &object, ItemDescriptor const &prepObject):
  _object(object),
  _prepObject(prepObject)
{}


std::string Take::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");

  if (_prepObject.noun.empty()) {
    try {
      auto targetItem = Impl::findItem(_object, Game::g_player->getLocation());
      if (!targetItem) {
	return Impl::findItem(_object, Game::g_player)
	  ? Narration::you_already_own(_object.str())
	  : Narration::there_is_no(_object.str());
      }

      auto [result, str] = targetItem->checkTakeCondition();
      if (result) {
	targetItem->clearTakeCondition();
	Game::g_player->getLocation()->removeItem(targetItem);
	Game::g_player->addItem(targetItem);
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
      auto enclosingItem = Impl::findItem(_prepObject, Game::g_player, Game::g_player->getLocation());
      if (!enclosingItem) return Narration::there_is_no(_prepObject.str());

      try {
	auto targetItem = Impl::findItem(_object, enclosingItem);
	if (!targetItem)
	  return Narration::does_not_contain(_prepObject.str(), _object.str());

	auto [result, str] = targetItem->checkTakeCondition();
	if (result) {
	  targetItem->clearTakeCondition(); // TODO: add "persistent" field to condition -> determines if cleared after success
	  enclosingItem->removeItem(targetItem);
	  Game::g_player->addItem(targetItem);

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


Drop::Drop(ItemDescriptor const &object):
  _object(object)
{}

std::string Drop::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");

  try {
    auto targetItem = Impl::findItem(_object, Game::g_player);
    if (!targetItem) {
      return Narration::you_do_not_own(_object.str());
    }
    else {
      Game::g_player->removeItem(targetItem);
      Game::g_player->getLocation()->addItem(targetItem);
      return Narration::you_dropped(_object.str());
    }
  }
  catch (Impl::MultipleItemsByThatNoun) {
    return Narration::which_one(_object.str());
  }
  UNREACHABLE();
}

Inspect::Inspect(ItemDescriptor const &object):
  _object(object)
{}

std::string Inspect::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");

  try {
    auto targetItem = Impl::findItem(_object, Game::g_player, Game::g_player->getLocation());
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
  auto const &items  = Game::g_player->items();
  if (items.empty()) return Narration::empty_inventory();
  
  std::vector<std::string> itemLabels;
  for (auto const &item: items) {
    itemLabels.push_back(item->label());
  }
  return Narration::list_items("inventory", itemLabels);
}

std::string Save::exec() const {
  return "SAVING";
}

std::string Load::exec() const {
  return "LOADING";
}

