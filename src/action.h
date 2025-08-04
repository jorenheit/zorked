#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <tuple>

#include "util.h"
#include "direction.h"
#include "item.h"

class Player;

struct Action {
  virtual ~Action() = default;
  virtual std::string exec() const = 0;
};

struct Nop: public Action {
  virtual std::string exec() const override;
};

struct Move: public Action {
  Direction _dir = NumDir;
  inline Move(Direction dir): _dir(dir) {}
  virtual std::string exec() const override;
};


struct Take: public Action {
  ItemDescriptor _object;
  ItemDescriptor _prepObject;
  
  inline Take(ItemDescriptor const &object, ItemDescriptor const &prepObject = {}):
    _object(object), _prepObject(prepObject)
  {}
  virtual std::string exec() const override;
};

struct Drop: public Action {
  ItemDescriptor _object;
  
  inline Drop(ItemDescriptor const &object): _object(object) {}
  virtual std::string exec() const override;
};

struct Inspect: public Action {
  ItemDescriptor _object;
  
  inline Inspect(ItemDescriptor const &object): _object(object) {}
  virtual std::string exec() const override;
};


struct ShowInventory: public Action {
  virtual std::string exec() const override;
};

struct Save: public Action {
  virtual std::string exec() const override;

};

struct Load: public Action {
  virtual std::string exec() const override;
};

template <typename ActionType>
inline std::string ActionVerb;

#define SET_VERB(ActionType, Verb) template <> inline std::string ActionVerb<ActionType> = Verb;
SET_VERB(Move, "move");
SET_VERB(Take, "take");

#endif // ACTION_H
