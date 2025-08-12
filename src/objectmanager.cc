#include "objectmanager.h"
#include "util.h"
#include "zobject.h"
#include "direction.h"
#include "exception.h"
#include "location.h"
#include "condition.h"
#include "interaction.h"
#include "player.h"
#include "item.h"
#include "game.h"

namespace Impl {
  std::vector<std::string> getItemIDs(nlohmann::json const &obj) {
    std::vector<std::string> result;
    for (auto const &[key, _]: obj.at("items").items()) {
      if (key == "_path") continue; // TODO: factor out _path
      result.push_back(key);
    }

    for (auto const &[key, value]: obj.at("common-items").items()) {
      if (key == "_path") continue; // TODO: factor out _path
      for (size_t idx = 0; idx != value.get<size_t>(); ++idx) {
	result.push_back(key);
      }
    }
    return result;
  }
}
// TODO: better assertions

ObjectPointer::ObjectPointer(std::nullptr_t):
  _obj(nullptr),
  _manager(nullptr),
  _index(0)
{}

ObjectPointer::ObjectPointer(ZObject *obj):
  _obj(obj),
  _manager(nullptr),
  _index(0)
{}
  
ObjectPointer::ObjectPointer(ObjectManagerBase *manager, size_t index):
  _obj(nullptr),
  _manager(manager),
  _index(index)
{}

ObjectPointer::operator bool() const {
  return _obj || _manager;
}

// TODO: rewrite in terms of getPointer instead of getReference
ZObject &ObjectPointer::operator*() {
  return *(this->operator->());
}
  
ZObject const &ObjectPointer::operator*() const {
  return const_cast<ObjectPointer*>(this)->operator*();
}

ZObject *ObjectPointer::operator->() {
  assert((_obj || _manager) && "Dereferencing null-pointer");
  if (_obj) return _obj;
  _obj = _manager->getRawPointer(_index);
  return _obj;
}

ZObject const *ObjectPointer::operator->() const {
  return const_cast<ObjectPointer*>(this)->operator->();
}

bool ObjectPointer::operator==(ObjectPointer const &other) const {
  if (*this && other) return this->get() == other.get();
  if (!*this && !other) return true;
  return false;
}

bool ObjectPointer::operator!=(ObjectPointer const &other) const {
  return !(*this == other);
}

void ObjectManager::setPlayer(nlohmann::json const &obj) {
  assert(!_initialized && "called setPlayer() after init()");
  assert(obj.is_object());

  std::vector<std::string> itemIDs = Impl::getItemIDs(obj);
  _playerData = obj;
  _playerData["items"] = itemIDs;
}
void ObjectManager::setStart(std::string const &startLocation, std::string const &path){
  assert(!_initialized && "called setStart() after init()");
  _startLocation = {startLocation, path};
}

Player *ObjectManager::player() {
  assert(_initialized && "called player() before init()");
  return _player.get();
}

ObjectPointer ObjectManager::addProxy(std::string const &id, nlohmann::json const &obj, ObjectType type) {
  assert(!_initialized && "called addProxy() after init()");
  switch (type) {
  case ObjectType::Location:   return _locationManager.addProxy(id, obj);
  case ObjectType::LocalItem:  return _localItemManager.addProxy(id, obj);
  case ObjectType::CommonItem: return _commonItemManager.addProxy(id, obj);
  case ObjectType::Player: assert(false);
  default: UNREACHABLE("default");
  }
}


void ObjectManager::setConnections(nlohmann::json const &obj) {
  assert(!_initialized && "called setConnections() after init()");
  _connections = obj;
}

ObjectPointer ObjectManager::get(std::string const &id) {
  static constexpr ObjectType const _types[] = {
    ObjectType::Location,
    ObjectType::LocalItem,
    ObjectType::CommonItem
  };

  if (id == _player->id()) return _player.get();
  for (ObjectType type: _types) {
    ObjectPointer ptr = get(id, type);
    if (ptr) return ptr;
  }
  return nullptr;
}

ObjectPointer ObjectManager::get(std::string const &id, ObjectType type) {
  switch (type) {
  case ObjectType::Location:   return _locationManager.getPointer(id);
  case ObjectType::LocalItem:  return _localItemManager.getPointer(id);
  case ObjectType::CommonItem: return _commonItemManager.getPointer(id);
  case ObjectType::Player:     assert(false);
  default: UNREACHABLE("default");
  }
  return nullptr;
}

namespace {
  using json = nlohmann::json;
  // Helper function for ObjectManager::build
  void connectLocations(ObjectManager *self, json const &connections) {
    for (json const &conn: connections) {
      std::string const &fromID = conn.at("from");
      std::string const &toID   = conn.at("to");
      std::string const &dirStr = conn.at("direction");

      auto from = self->get(fromID, ObjectType::Location).get<Location*>();
      auto to   = self->get(toID,   ObjectType::Location).get<Location*>();
      Direction dir = directionFromString(dirStr);

      if (not from) throw Exception::UndefinedReference(conn.at("_path"), fromID);
      if (not to) throw Exception::UndefinedReference(conn.at("_path"), toID);
      if (dir == NumDir) throw Exception::SpecificFormatError(conn.at("_path"), "Invalid direction '", dirStr, "'.");

      if (from->connection(dir).first != nullptr) {
	throw Exception::DoubleConnectedLocation(conn.at("_path"), fromID, from->connection(dir).first->id());
      }

      std::shared_ptr<Condition> cond = Condition::construct(conn.at("move-condition"));
      from->connect(dir, to, cond);
      if (conn.at("symmetric").get<bool>()) {
	if (to->connection(oppositeDirection(dir)).first != nullptr) {
	  throw Exception::DoubleConnectedLocation(conn.at("_path"), toID, to->connection(oppositeDirection(dir)).first->id());
	}
      
	to->connect(oppositeDirection(dir), from, cond);
      }
    }
  }
}

void ObjectManager::init() {
  assert(not _playerData.is_null());
  assert(not _connections.is_null());
  assert(not _startLocation.first.empty());
  assert(not _initialized);

  _player = Player::construct(_playerData);
  ObjectPointer start = this->get(_startLocation.first, ObjectType::Location);
  if (not start) {
    throw Exception::UndefinedReference(_startLocation.second, _startLocation.first);
  }
  _player->setLocation(start.get<Location*>());
  connectLocations(this, _connections);

  _locationManager.constructAll();
  _localItemManager.constructAll();
  _commonItemManager.constructAll();
  
  _initialized = true;
}

  
