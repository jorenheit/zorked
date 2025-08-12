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
  virtual std::string exec() = 0;
};

struct Nop: public Action {
  virtual std::string exec() override;
};

struct Move: public Action {
  Direction _dir = NumDir;
  inline Move(Direction dir): _dir(dir) {}
  virtual std::string exec() override;
};


struct Take: public Action {
  ObjectDescriptor _object;
  ObjectDescriptor _prepObject;
  
  inline Take(ObjectDescriptor const &object, ObjectDescriptor const &prepObject = {}):
    _object(object), _prepObject(prepObject)
  {}
  virtual std::string exec() override;
};

struct Drop: public Action {
  ObjectDescriptor _object;
  
  inline Drop(ObjectDescriptor const &object): _object(object) {}
  virtual std::string exec() override;
};

// TODO: insert action (PUT X in Y -> opposite to TAKE X from Y)
// --> need  way to tell if objects can hold other objects
// --> container class? or simply a boolean?

struct Inspect: public Action {
  ObjectDescriptor _object;
  
  inline Inspect(ObjectDescriptor const &object): _object(object) {}
  virtual std::string exec() override;
};

struct ShowInventory: public Action {
  virtual std::string exec() override;
};

struct Interact: public Action {

  std::string _verb;
  ObjectDescriptor _object;
  ObjectDescriptor _tool;
  
  inline Interact(std::string const &verb,
		  ObjectDescriptor const &object = {},
		  ObjectDescriptor const &tool = {}):
    _verb(verb),
    _object(object),
    _tool(tool)
  {}
  
  virtual std::string exec() override;
};

struct Save: public Action {
  virtual std::string exec() override;

};

struct Load: public Action {
  virtual std::string exec() override;
};

#endif // ACTION_H
