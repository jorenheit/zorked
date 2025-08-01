#include "zobject.h"
#include "item.h"
#include "util.h"
#include "game.h"
#include "narration.h"

std::shared_ptr<ZObject> ZObject::construct(std::string const &id, JSONObject const &jsonObj) {
  auto label = jsonObj.get<std::string>("label");

  std::vector<std::string> nouns;
  for (auto const &noun: jsonObj.getOrDefault<std::vector<JSONObject>>("nouns")) {
    nouns.push_back(noun.get<std::string>());
  }
  if (nouns.empty()) nouns.push_back(id);

  std::unordered_map<std::string, bool> state;
  for (auto const &[key, value]: jsonObj.getOrDefault<JSONObject>("state")) {
    state[key] = value;
  }

  auto loreConditionObject = [&jsonObj]() -> JSONObject {
    if (not jsonObj.contains("description")) {
      return json {{"success", ""}};
    }
    else if (jsonObj.at("description").is_string()) {
      return json {{"success", jsonObj.get<std::string>("description")}};
    }
    else {
      return jsonObj.get<JSONObject>("description").get<JSONObject>("lore-condition");
    }
  }();
  size_t loreConditionIndex = Game::g_conditions.add(loreConditionObject);
  
  auto inspectConditionObject = [&jsonObj]() -> JSONObject {
    if (not jsonObj.contains("inspect")) {
      return json {{"success", Narration::nothing_to_see()}};
    }
    else if (jsonObj.at("inspect").is_string()) {
      return json {{"success", jsonObj.get<std::string>("inspect")}};
    }
    else {
      return jsonObj.get<JSONObject>("inspect").get<JSONObject>("inspect-condition");
    }
  }();
  
  if (not inspectConditionObject.contains("success")) {
    throw Exception::SpecificFormatError(jsonObj.path(), jsonObj.trace(),
					 "'inspect-condition' field must contain a 'success'-field.");
  }
  
  size_t inspectConditionIndex = Game::g_conditions.add(inspectConditionObject);
  return std::make_shared<ZObject>(id, label, nouns, state, loreConditionIndex, inspectConditionIndex);
}


ZObject::ZObject(std::string const &id, std::string const &label, std::vector<std::string> const &nouns,
		 std::unordered_map<std::string, bool> const &state,
		 size_t loreConditionIndex,
		 size_t inspectConditionIndex):
  _id(id),
  _label(label),
  _nouns(nouns),
  _loreConditionIndex(loreConditionIndex),
  _inspectConditionIndex(inspectConditionIndex),
  _state(state)
{}

std::string const &ZObject::id() const {
  return _id;
}

std::string const &ZObject::label() const {
  return _label;
}

std::string const &ZObject::description() const {
  Condition const &loreCondition = Game::g_conditions.get(_loreConditionIndex);
  return loreCondition.eval()
    ? loreCondition.successString()
    : loreCondition.failString();
}


std::vector<std::string> const &ZObject::nouns() const {
  return _nouns;
}

std::vector<std::shared_ptr<Item>> const &ZObject::items() const {
  return _items;
}

std::unordered_map<std::string, bool> const &ZObject::state() const {
  return _state;
}

void ZObject::addItem(std::shared_ptr<Item> item) {
  _items.push_back(item);
}

bool ZObject::removeItem(std::shared_ptr<Item> item) {
  auto it = std::find(_items.begin(), _items.end(), item);
  if (it == _items.end()) return false;
  _items.erase(it);
  return true;
}

size_t ZObject::contains(std::shared_ptr<Item> item) const {
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
  Condition const &inspectCondition = Game::g_conditions.get(_inspectConditionIndex);
  if (inspectCondition.eval())
    return inspectCondition.successString();

  std::string const &fail = inspectCondition.failString();
  if (not fail.empty()) return fail;
  return Narration::nothing_to_see();
}

bool ZObject::restore(json const &jsonObj) {
  _items.clear();

  for (std::string const &id: jsonObj.at("items").get<std::vector<std::string>>()) {
    std::shared_ptr<Item> ptr = Game::commonItemByID(id);
    if (ptr) {
      addItem(ptr);
      continue;
    }
    ptr = Game::localItemByID(id);
    if (ptr) {
      addItem(ptr);
      continue;
    }
    return false;
  }

  _state = jsonObj.at("state");
  return true;
}
