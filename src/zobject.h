#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <unordered_map>

#include "jsonobject.h"

class Item;
class Location;
class Condition;

class ZObject {
  std::string _id;
  std::string _label;
  std::vector<std::string> _nouns;
  size_t _loreConditionIndex;
  size_t _inspectConditionIndex;
  
  std::vector<std::shared_ptr<Item>> _items;
  std::unordered_map<std::string, bool> _state;
  
public:
  ZObject(std::string const &id, std::string const &label, std::vector<std::string> const &nouns,
	  std::unordered_map<std::string, bool> const &state,
	  size_t loreConditionIndex,
	  size_t inspectConditionIndex);
  
  std::string const &id() const;
  std::string const &label() const;
  std::string const &description() const;
  std::vector<std::string> const &nouns() const;

  std::vector<std::shared_ptr<Item>> const &items() const;
  std::unordered_map<std::string, bool> const &state() const;

  std::string inspect();
  
  void addItem(std::shared_ptr<Item> item);
  bool removeItem(std::shared_ptr<Item> item);
  size_t contains(std::shared_ptr<Item> item) const; 
  bool getState(std::string const &stateStr) const;
  void setState(std::string const &stateStr, bool value);

  static std::shared_ptr<ZObject> construct(std::string const &id, JSONObject const &jsonObj);

  bool restore(json const &jsonObj);
};


#endif // ZOBJECT_H
