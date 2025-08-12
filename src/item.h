#ifndef ITEM_H
#define ITEM_H

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "json_fwd.hpp"

#include "zobject.h"
class Condition;

class Item: public ZObject {
  bool const _common;
  bool _portable;
  double _weight;
  std::shared_ptr<Condition> _takeCondition;
  bool _dropped = false;

public:
  Item(ZObject &&parent, bool common, bool portable, double weight, std::shared_ptr<Condition> cond);

  bool common() const;
  bool portable() const;
  double weight() const;
  bool dropped() const;
  
  Condition *takeCondition() const;

  static std::unique_ptr<Item> construct(std::string const &id, nlohmann::json const &obj);
  
  void to_json(nlohmann::json &jsonObj) const;
  bool restore(nlohmann::json const &jsonObj);
};

void to_json(nlohmann::json &jsonObj, Item const &item);
void to_json(nlohmann::json &jsonObj, std::shared_ptr<Item> const &item);




#endif //ITEM_H
