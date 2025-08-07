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
class Interaction;


struct ObjectDescriptor {
  std::string noun;
  std::vector<std::string> adjectives;
  std::string str() const;
};

class ZObject {
  std::string _id;
  std::string _label;
  std::vector<std::string> _nouns;
  std::vector<std::string> _adjectives;
  std::unique_ptr<Condition> _loreCondition;
  std::unique_ptr<Condition> _inspectCondition;
  std::vector<std::unique_ptr<Interaction>> _interactions;
  
  std::vector<Item*> _items;
  std::unordered_map<std::string, bool> _state;
  
public:
  ZObject(ZObject&&) = default;
  ZObject(std::string const &id, std::string const &label,
	  std::vector<std::string> const &nouns,
	  std::vector<std::string> const &adjectives,
	  std::vector<Item*> const &items,
	  std::unordered_map<std::string, bool> const &state,
	  std::unique_ptr<Condition> loreCondition,
	  std::unique_ptr<Condition> inspectCondition,
	  std::vector<std::unique_ptr<Interaction>> &interactions);
  
  std::string const &id() const;
  std::string const &label() const;
  std::string const &description() const;
  std::vector<std::string> const &nouns() const;
  std::vector<std::string> const &adjectives() const;
  std::vector<std::unique_ptr<Interaction>> const &interactions() const;

  
  std::vector<Item*> const &items() const;
  std::unordered_map<std::string, bool> const &state() const;

  std::string inspect();
  bool match(ObjectDescriptor const &descr) const;
  void addItem(Item *item);
  bool removeItem(Item const *item);
  size_t contains(Item const *item) const; 
  bool getState(std::string const &stateStr) const;
  void setState(std::string const &stateStr, bool value);

  static std::unique_ptr<ZObject> construct(std::string const &id, nlohmann::json const &obj);

  bool restore(nlohmann::json const &jsonObj);
};


#endif // ZOBJECT_H
