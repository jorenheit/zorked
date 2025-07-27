#include "item.h"

std::string ItemDescriptor::str() const {
  std::string result;
  for (std::string const &adj: adjectives) {
    result += (adj + " ");
  }
  result += noun;
  return result;
}


std::shared_ptr<Item> Item::construct(std::string const &id, bool common, JSONObject const &jsonObj) {
  auto ptr = ZObject::construct(id, jsonObj);
  bool portable = jsonObj.getOrDefault<bool>("portable", false);
  double weight = jsonObj.getOrDefault<double>("weight", 1.0);
  Condition cond = Condition::construct(jsonObj.getOrDefault<JSONObject>("take-condition"));

  std::vector<std::string> adjectives;
  for (auto const &adj: jsonObj.getOrDefault<std::vector<JSONObject>>("adjectives")) {
    adjectives.push_back(adj.get<std::string>());
  }
  
  return std::make_shared<Item>(*ptr, common, portable, weight, cond, adjectives);
}

Item::Item(ZObject const &zObj, bool common, bool portable, double weight,
	   Condition const &cond, std::vector<std::string> const &adjectives):
  ZObject(zObj),
  _common(common),
  _portable(portable),
  _weight(weight),
  _takeCondition(cond),
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

void Item::clearCondition() {
  _takeCondition = Condition{};
}

std::pair<bool, std::string> Item::checkTakeCondition() const {
  return _takeCondition() 
    ? std::make_pair(true, _takeCondition.successString())
    : std::make_pair(false, _takeCondition.failString());
}
