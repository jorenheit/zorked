#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H
#include <iostream> // debug
#include <cassert>
#include <vector>
#include <string>
#include <utility>
#include <type_traits>
#include <memory>
#include "json.hpp"

class ZObject;
class Player;
class Location;
class Item;

enum class ObjectType {
  Player = 0,
  Location = 1,
  LocalItem = 2,
  CommonItem = 3
};

class ObjectPointer;

class ObjectManagerBase {
public:
  virtual ~ObjectManagerBase() = default;
  virtual ZObject &getReference(size_t index) = 0;
  virtual ZObject const &getReference(size_t index) const = 0;
};


class ObjectPointer {
  ZObject *_obj;
  ObjectManagerBase *_manager;
  size_t _index;

public:
  ObjectPointer(std::nullptr_t = nullptr);
  ObjectPointer(ZObject *obj);
  ObjectPointer(ObjectManagerBase *manager, size_t index);

  operator bool() const;
  ZObject &operator*();
  ZObject const &operator*() const;
  ZObject *operator->();
  ZObject const *operator->() const;

  bool operator==(ObjectPointer const &other) const;
  bool operator!=(ObjectPointer const &other) const;

  template <typename Derived>
  Derived get() {
    static_assert(std::is_pointer_v<Derived>, "Must be a pointer type derived from ZObject");
    return static_cast<Derived>(this->operator->());
  }

  template <typename Derived>
  Derived get() const {
    return const_cast<ObjectPointer*>(this)->get<Derived>();
  }
    
};

template <typename T>
class ObjectManager_: public ObjectManagerBase {
  std::vector<std::unique_ptr<T>> _objects;
  std::vector<std::pair<std::string, nlohmann::json>> _proxies;
  bool _initialized = false;
    
public:
  ObjectPointer addProxy(std::string const &id, nlohmann::json const &obj);
  ObjectPointer getPointer(std::string const &id);
  void construct(size_t index);
  void constructAll();
  
  // TODO: remove this
  std::vector<std::pair<std::string, nlohmann::json>> const &proxies() const {
    return _proxies;
  }
  
private:
  virtual ZObject &getReference(size_t index) override;
  virtual ZObject const &getReference(size_t index) const override;
};

class ObjectManager {

  std::unique_ptr<Player>  _player;
  ObjectManager_<Location> _locationManager;
  ObjectManager_<Item>     _localItemManager;
  ObjectManager_<Item>     _commonItemManager;

  nlohmann::json _playerData;
  nlohmann::json _connections;
  std::string _startLocation;
  bool _initialized = false;
  
public:
  ObjectPointer addProxy(std::string const &id, nlohmann::json const &obj, ObjectType type);  
  void setPlayer(nlohmann::json const &obj);
  void setConnections(nlohmann::json const &obj);
  void setStart(std::string const &startID);

  void build();
    
  // TODO: make these const
  ObjectPointer get(std::string const &id);
  ObjectPointer get(std::string const &id, ObjectType type);
  Player *player();

  template <typename ... Rest>
  ObjectPointer get(std::string const &id, ObjectType first, Rest ... rest) {
    ObjectPointer result = get(id, first);
    return result ? result : get(id, rest ...);
  }

};

#include "objectmanager.tpp"


#endif // OBJECTMANAGER_H
