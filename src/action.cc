#include "action.h"
#include "game.h"
#include "player.h"
#include "location.h"
#include "condition.h"

Move::Move(Direction dir):
  _dir(dir)
{}

std::string Move::exec() const {
  assert(_dir != NumDir && "dir should be valid here");

  auto [location, condition] = Game::g_player->getLocation()->connected(_dir);
    
  if (not condition())
    return condition.failString();
  
  if (location) {
    Game::g_player->setLocation(location);
    return condition.successString();
  }

  return "You can't go there."; // TODO: factor out default narratives
}

namespace TakeImpl {
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
	// multiple common items, just keep going
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
} // namespace TakeImpl


Take::Take(ItemDescriptor const &object, ItemDescriptor const &prepObject):
  _object(object),
  _prepObject(prepObject)
{}

std::string Take::exec() const {
  assert(not _object.noun.empty() && "_object should have a value");

  if (_prepObject.noun.empty()) {
    try {
      auto targetItem = TakeImpl::findItem(_object, Game::g_player->getLocation());
      if (!targetItem) {
	targetItem = TakeImpl::findItem(_object, Game::g_player);
	// TODO: factor out default narration
	if (targetItem) return "You already have the " + _object.str() + "!";
	else return "There is no "  + _object.str() + " here.";
      }

      auto [result, str] = targetItem->checkTakeCondition();
      if (result) {
	Game::g_player->getLocation()->removeItem(targetItem);
	Game::g_player->addItem(targetItem);
	if (str.empty()) return "You took the " + _object.str() + "!"; // TODO: factor out default narration.
	else return str;
      }
      else if (str.empty()) {
	return "You were unable to take the " + _object.str() + ".";
      }
      else return str;
    }
    catch (TakeImpl::MultipleItemsByThatNoun) {
      return "Which " + _object.str() + "?";
    }
  }
  else {
    try {
      auto enclosingItem = TakeImpl::findItem(_prepObject, Game::g_player, Game::g_player->getLocation());
      if (!enclosingItem) return "There is no "  + _prepObject.str() + " here.";
      try {
	auto targetItem = TakeImpl::findItem(_object, enclosingItem);
	if (!targetItem) return "The " + _prepObject.str() + " does not contain a " + _object.str() + ".";

	auto [result, str] = targetItem->checkTakeCondition();
	if (result) {
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
      catch (TakeImpl::MultipleItemsByThatNoun) {
	return "Which " + _object.str() + "?";
      }
    }
    catch (TakeImpl::MultipleItemsByThatNoun) {
      return "Which " + _prepObject.str() + "?";
    }
  }

  UNREACHABLE();
}






