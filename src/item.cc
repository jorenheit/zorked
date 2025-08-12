#include <iostream>
#include "item.h"
#include "game.h"
#include "condition.h"
#include "location.h"
#include "player.h"
#include "interaction.h"
#include "json.hpp" // TODO: move all construct implementations to seperate TU
using json = nlohmann::json;

std::unique_ptr<Item> Item::construct(std::string const &id, json const &obj) {
  std::unique_ptr<ZObject> zObj = ZObject::construct(id, obj);
  auto common = obj.at("common").get<bool>();
  auto portable = obj.at("portable").get<bool>();
  auto weight = obj.at("weight").get<double>();
  std::unique_ptr<Condition> takeCondition = Condition::construct(obj.at("take-condition"));
  return std::make_unique<Item>(std::move(*zObj), common, portable, weight, std::move(takeCondition));
}

// TODO: move semantics?
Item::Item(ZObject &&parent, bool common, bool portable, double weight,
	   std::shared_ptr<Condition> takeCondition):
  ZObject(std::move(parent)),
  _common(common),
  _portable(portable),
  _weight(weight),
  _takeCondition(std::move(takeCondition))
{}

bool Item::common() const {
  return _common;
}

bool Item::portable() const {
  return _portable;
}

double Item::weight() const {
  return _weight;
}

Condition *Item::takeCondition() const {
  return _takeCondition.get();
}

void Item::to_json(json &obj) const {
  obj = json::object();
  std::cerr << "TODO: implement Item::to_json\n";
  std::exit(1);
}

bool Item::restore(json const &obj) {
  if (obj.at("take-cleared").get<bool>()) {
    _takeCondition->clear();
  }
  std::cerr << "TODO: implement Item::restore\n";
  std::exit(1);
  return ZObject::restore(obj);
}

void to_json(nlohmann::json &jsonObj, Item const &item) {
  item.to_json(jsonObj);
}

void to_json(nlohmann::json &jsonObj, std::shared_ptr<Item> const &item) {
  jsonObj = item->id();
}
