#include "zobject.h"
#include "util.h"

std::shared_ptr<ZObject> ZObject::construct(std::string const &id, JSONObject const &jsonObj) {
  auto label = jsonObj.get<std::string>("label");
  auto description = jsonObj.getOrDefault<std::string>("description");

  std::vector<std::string> nouns;
  for (auto const &noun: jsonObj.getOrDefault<std::vector<JSONObject>>("nouns")) {
    nouns.push_back(noun.get<std::string>());
  }
  if (nouns.empty()) nouns.push_back(id);

  std::unordered_map<std::string, bool> state;
  for (auto const &[key, value]: jsonObj.getOrDefault<JSONObject>("state")) {
    state[key] = value;
  }

  return std::make_shared<ZObject>(id, label, description, nouns, state);
}


ZObject::ZObject(std::string const &id, std::string const &label, std::string const &description,
		 std::vector<std::string> const &nouns, std::unordered_map<std::string, bool> const &state): // TODO: move semantics for state map
  _id(id),
  _label(label),
  _description(description),
  _nouns(nouns),
  _state(state)
{}

std::string const &ZObject::id() const {
  return _id;
}

std::string const &ZObject::label() const {
  return _label;
}

std::string const &ZObject::description() const {
  return _description;
}

std::vector<std::string> const &ZObject::nouns() const {
  return _nouns;
}

  
std::vector<std::shared_ptr<Item>> const &ZObject::items() const {
  return _items;
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
  for (auto const &[str, val]: _state) {
    if (str == stateStr) return val;
  }
  return false;
}

void ZObject::setState(std::string const &stateStr, bool value) {
  _state[stateStr] = value;
}
