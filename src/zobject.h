#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <unordered_map>
#include "json_fwd.hpp"

class Condition;
class Item;

class ZObject {
  std::string _id;
  std::string _label;
  std::vector<std::string> _nouns;
  std::unique_ptr<Condition> _loreCondition;
  std::unique_ptr<Condition> _inspectCondition;
  
  std::vector<Item*> _items;
  std::unordered_map<std::string, bool> _state;
  
public:
  ZObject(ZObject&&) = default;
  ZObject(std::string const &id, std::string const &label, std::vector<std::string> const &nouns,
	  std::vector<Item*> const &items,
	  std::unordered_map<std::string, bool> const &state,
	  std::unique_ptr<Condition> loreCondition,
	  std::unique_ptr<Condition> inspectCondition);
  
  std::string const &id() const;
  std::string const &label() const;
  std::string const &description() const;
  std::vector<std::string> const &nouns() const;

  std::vector<Item*> const &items() const;
  std::unordered_map<std::string, bool> const &state() const;

  std::string inspect();
  
  void addItem(Item *item);
  bool removeItem(Item *item);
  size_t contains(Item *item) const; 
  bool getState(std::string const &stateStr) const;
  void setState(std::string const &stateStr, bool value);

  static std::unique_ptr<ZObject> construct(std::string const &id, nlohmann::json const &obj);

  bool restore(nlohmann::json const &jsonObj);
};


#endif // ZOBJECT_H
