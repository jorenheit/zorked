#include "exception.h"
#include "zobject.h"
#include "item.h"
#include "util.h"
#include "game.h"
#include "narration.h"
#include "condition.h"
#include "objectmanager.h"
#include "json.hpp"

using json = nlohmann::json;

std::unique_ptr<ZObject> ZObject::construct(std::string const &id, json const &obj) {
  auto label = obj.at("label").get<std::string>();
  auto nouns = obj.at("nouns").get<std::vector<std::string>>();
  if (nouns.empty())
    nouns.push_back(id);

  std::unique_ptr<Condition> loreCondition = Condition::construct(obj.at("description").at("lore-condition"));
  std::unique_ptr<Condition> inspectCondition = Condition::construct(obj.at("inspect").at("inspect-condition"));

  std::unordered_map<std::string, bool> state;
  for (auto const &[key, value]: obj.at("state").items()) {
    if (key == "_path") continue;
    state[key] = value;
  }
  auto itemIDs = obj.at("items").get<std::vector<std::string>>();

  std::vector<Item*> items;
  for (std::string const &id: itemIDs) {
    ObjectPointer ptr = Global::g_objectManager.get(id, ObjectType::LocalItem, ObjectType::CommonItem);
    if (!ptr) throw Exception::UndefinedReference(obj.at("_path"), id);
    items.push_back(ptr.get<Item*>()); // Will construct the object from its proxy if necessary
  }

  return std::make_unique<ZObject>(id, label, nouns, items, state, std::move(loreCondition), std::move(inspectCondition));
}

// Todo: move semantics
ZObject::ZObject(std::string const &id, std::string const &label, std::vector<std::string> const &nouns,
		 std::vector<Item*> const &items,
		 std::unordered_map<std::string, bool> const &state,
		 std::unique_ptr<Condition> loreCondition,
		 std::unique_ptr<Condition> inspectCondition):
  _id(id),
  _label(label),
  _nouns(nouns),
  _loreCondition(std::move(loreCondition)),
  _inspectCondition(std::move(inspectCondition)),
  _items(items),
  _state(state)
{}

std::string const &ZObject::id() const {
  return _id;
}

std::string const &ZObject::label() const {
  return _label;
}

std::string const &ZObject::description() const {
  return _loreCondition->eval()
    ? _loreCondition->successString()
    : _loreCondition->failString();
}


std::vector<std::string> const &ZObject::nouns() const {
  return _nouns;
}

std::vector<Item*> const &ZObject::items() const {
  return _items;
}

std::unordered_map<std::string, bool> const &ZObject::state() const {
  return _state;
}

void ZObject::addItem(Item *item) {
  _items.push_back(item);
}

bool ZObject::removeItem(Item *item) {
  auto it = std::find(_items.begin(), _items.end(), item);
  if (it == _items.end()) return false;
  _items.erase(it);
  return true;
}

size_t ZObject::contains(Item *item) const {
  size_t count = 0;
  for (auto const &itemPtr: _items) {
    count += (itemPtr == item);
  }
  return count;
}

bool ZObject::getState(std::string const &stateStr) const {
  if (not _state.contains(stateStr))
    return false;

  return _state.at(stateStr);
}

void ZObject::setState(std::string const &stateStr, bool value) {
  _state[stateStr] = value;
}

std::string ZObject::inspect() {
  if (_inspectCondition->eval()) {
    std::string const &success = _inspectCondition->successString();
    if (not success.empty()) return success;
    return Narration::nothing_to_see();
  }

  std::string const &fail = _inspectCondition->failString();
  if (not fail.empty()) return fail;
  return Narration::nothing_to_see();
}

bool ZObject::restore(json const &obj) {
  // _items.clear();

  // for (std::string const &id: obj.at("items").get<std::vector<std::string>>()) {
  //   ObjectPointer ptr = Global::g_objectManager.get(id, ObjectType::CommonItem, ObjectType::LocalItem);
  //   if (ptr) addItem(ptr.get<Item*>());
  //   else return false;
  // }
  // _state = obj.at("state");
  return true;
}
