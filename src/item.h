#ifndef ITEM_H
#define ITEM_H

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "json_fwd.hpp"

#include "zobject.h"
class Condition;

struct ItemDescriptor {
  std::string noun;
  std::vector<std::string> adjectives;
  std::string str() const;
};

class Item: public ZObject {
  bool const _common;
  bool _portable;
  double _weight;
  std::shared_ptr<Condition> _takeCondition;
  std::vector<std::string> _adjectives;

public:
  Item(ZObject &&parent, bool common, bool portable, double weight, std::shared_ptr<Condition> cond,
       std::vector<std::string> const &adjectives);

  bool common() const;
  bool portable() const;
  double weight() const;
  std::vector<std::string> const &adjectives() const;
  bool match(ItemDescriptor const &descr) const;

  std::pair<bool, std::string> checkTakeCondition() const;
  void clearTakeCondition();

  static std::unique_ptr<Item> construct(std::string const &id, nlohmann::json const &obj);
  
  void to_json(nlohmann::json &jsonObj) const;
  bool restore(nlohmann::json const &jsonObj);
};

void to_json(nlohmann::json &jsonObj, Item const &item);
void to_json(nlohmann::json &jsonObj, std::shared_ptr<Item> const &item);




#endif //ITEM_H
