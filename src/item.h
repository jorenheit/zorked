#ifndef ITEM_H
#define ITEM_H
#include "zobject.h"
#include "condition.h"

struct ItemDescriptor {
  std::string noun;
  std::vector<std::string> adjectives;

  std::string str() const;

};

class Item: public ZObject {
  bool _common;
  bool _portable;
  double _weight;
  std::shared_ptr<Condition> _takeCondition;
  std::vector<std::string> _adjectives;
  
public:
  Item(ZObject const &zObj, bool common, bool portable, double weight,
       std::shared_ptr<Condition> cond, std::vector<std::string> const &adjectives);

  bool common() const;
  bool portable() const;
  double weight() const;
  std::vector<std::string> const &adjectives() const;
  bool match(ItemDescriptor const &descr) const;

  std::pair<bool, std::string> checkTakeCondition() const;
  void clearTakeCondition();

  static std::shared_ptr<Item> construct(std::string const &id, bool common, JSONObject const &obj);
  void to_json(json &jsonObj) const;
  bool restore(json const &jsonObj);
};

inline void to_json(json &jsonObj, Item const &item) {
  item.to_json(jsonObj);
}

inline void to_json(json &jsonObj, std::shared_ptr<Item> item) {
  jsonObj = item->id();
}



#endif //ITEM_H
