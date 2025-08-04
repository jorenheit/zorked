#include <iostream>
#include "item.h"
#include "game.h"
#include "condition.h"
#include "location.h"
#include "player.h"
#include "json.hpp" // TODO: move all construct implementations to seperate TU
using json = nlohmann::json;

std::string ItemDescriptor::str() const {
  std::string result;
  for (std::string const &adj: adjectives) {
    result += (adj + " ");
  }
  result += noun;
  return result;
}

std::unique_ptr<Item> Item::construct(std::string const &id, json const &obj) {
  std::unique_ptr<ZObject> zObj = ZObject::construct(id, obj);
  auto common = obj.at("common").get<bool>();
  auto portable = obj.at("portable").get<bool>();
  auto weight = obj.at("weight").get<double>();
  std::unique_ptr<Condition> takeCondition = Condition::construct(obj.at("take-condition"));
  std::vector<std::string> adjectives = obj.at("adjectives");
  return std::make_unique<Item>(std::move(*zObj), common, portable, weight, std::move(takeCondition), adjectives);
}

// TODO: move semantics?
Item::Item(ZObject &&parent, bool common, bool portable, double weight,
	   std::shared_ptr<Condition> takeCondition,
	   std::vector<std::string> const &adjectives):
  ZObject(std::move(parent)),
  _common(common),
  _portable(portable),
  _weight(weight),
  _takeCondition(std::move(takeCondition)),
  _adjectives(adjectives)
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

std::vector<std::string> const &Item::adjectives() const {
  return _adjectives;
}

bool Item::match(ItemDescriptor const &descr) const {
  bool nounMatch = false;
  for (std::string const &noun: this->nouns()) {
    if (noun == descr.noun) {
      nounMatch = true;
      break;
    }
  }
  if (not nounMatch) return false;
  
  for (std::string const &adj: descr.adjectives) {
    bool adjMatch = false;
    for (std::string const &myAdj: this->adjectives()) {
      if (adj == myAdj) {
	adjMatch = true;
	break;
      }
    }
    if (!adjMatch) return false;
  }
  return true;
}

void Item::clearTakeCondition() {
  _takeCondition->clear();
}

std::pair<bool, std::string> Item::checkTakeCondition() const {
  return _takeCondition->eval() 
    ? std::make_pair(true, _takeCondition->successString())
    : std::make_pair(false, _takeCondition->failString());
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
