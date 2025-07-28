#include "action.h"
#include "game.h"
#include "player.h"
#include "location.h"
#include "condition.h"

Move::Move(Direction dir):
  _dir(dir)
{}

// TODO: factor out default narratives
std::string Move::exec() const {
  assert(_dir != NumDir && "dir should be valid here");

  auto [location, condition] = Game::g_player->getLocation()->connected(_dir);

  if (not location)
    return "You can't go there.";
  
  if (not condition->eval())
    return condition->failString();
  
  location->clearMoveCondition(_dir);
  Game::g_player->setLocation(location);
  return condition->successString();
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


// TODO: factor out default narration
std::string Take::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");

  if (_prepObject.noun.empty()) {
    try {
      auto targetItem = Impl::findItem(_object, Game::g_player->getLocation());
      if (!targetItem) {
	targetItem = Impl::findItem(_object, Game::g_player);
	if (targetItem) return "You already have the " + _object.str() + "!";
	else return "There is no "  + _object.str() + " here.";
      }

      auto [result, str] = targetItem->checkTakeCondition();
      if (result) {
	targetItem->clearTakeCondition();
	Game::g_player->getLocation()->removeItem(targetItem);
	Game::g_player->addItem(targetItem);
	if (str.empty()) return "You took the " + _object.str() + "!"; 
	else return str;
      }
      else if (str.empty()) {
	return "You were unable to take the " + _object.str() + ".";
      }
      else return str;
    }
    catch (Impl::MultipleItemsByThatNoun) {
      return "Which " + _object.str() + "?";
    }
  }
  else {
    try {
      auto enclosingItem = Impl::findItem(_prepObject, Game::g_player, Game::g_player->getLocation());
      if (!enclosingItem) return "There is no "  + _prepObject.str() + " here.";
      try {
	auto targetItem = Impl::findItem(_object, enclosingItem);
	if (!targetItem) return "The " + _prepObject.str() + " does not contain a " + _object.str() + ".";

	auto [result, str] = targetItem->checkTakeCondition();
	if (result) {
	  targetItem->clearTakeCondition(); // TODO: add "persistent" field to condition -> determines if cleared after success
	  enclosingItem->removeItem(targetItem);
	  Game::g_player->addItem(targetItem);
	  if (str.empty())
	    return "You took the " + _object.str() + " from the " + _prepObject.str() + "!";
	  else return str;
	}
	else if (str.empty()) {
	  return "You were unable to take the " + _object.str() + ".";
	}
	else return str;
      }
      catch (Impl::MultipleItemsByThatNoun) {
	return "Which " + _object.str() + "?";
      }
    }
    catch (Impl::MultipleItemsByThatNoun) {
      return "Which " + _prepObject.str() + "?";
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
      return "You don't have a "  + _object.str() + ".";
    }
    else {
      Game::g_player->removeItem(targetItem);
      Game::g_player->getLocation()->addItem(targetItem);
      return "You dropped the " + _object.str() + ".";
    }
  }
  catch (Impl::MultipleItemsByThatNoun) {
    return "Which " + _object.str() + "?";
  }
  UNREACHABLE();
}


// TODO: factor out inventory showing (should be the same as showing "inventory" of a location maybe).
std::string ShowInventory::exec() const {
  auto const &items  = Game::g_player->items();
  if (items.empty()) {
    return "You don't have any items on you.";
  }
  
  std::string result;
  for (auto const &item: items) {
    result += item->description() + "\n";
  }
  return result;
}

std::string Save::exec() const {
  return "SAVING";
}

std::string Load::exec() const {
  return "LOADING";
}

