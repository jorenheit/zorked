#include <iostream>

template <typename T>
ObjectPointer ObjectManager_<T>::addProxy(std::string const &id, nlohmann::json const &obj){
  assert(not _initialized && "Calling 'add' after 'constructAll'");

  if (id == "player") std::cout << "ADD PLAYER PROXY\n";
  // Replace items by their keys, then add to list of proxies
  std::vector<std::string> itemIDs = Impl::getItemIDs(obj);
  nlohmann::json proxy = obj;
  proxy["items"] = itemIDs;
  _proxies.emplace_back(std::make_pair(id, proxy));
  _objects.push_back(nullptr);
  return ObjectPointer{this, _proxies.size() - 1};
}

template <typename T>
ObjectPointer ObjectManager_<T>::getPointer(std::string const &id) {
  for (size_t idx = 0; idx != _proxies.size(); ++idx) {
    if (_proxies[idx].first == id) {
      return ObjectPointer{this, idx};
    }
  }
  return nullptr;
}

template <typename T>
void ObjectManager_<T>::construct(size_t index) {
  if (!_objects[index]) {
    _objects[index] = T::construct(_proxies[index].first, _proxies[index].second);
  }
}

template <typename T>
void ObjectManager_<T>::constructAll() {
  for (size_t idx = 0; idx != _objects.size(); ++idx) {
    construct(idx);
  }
}

template <typename T>
ZObject *ObjectManager_<T>::getRawPointer(size_t index) {
  construct(index);
  return _objects[index].get();
}

template <typename T>
ZObject const *ObjectManager_<T>::getRawPointer(size_t index) const {
  return const_cast<ObjectManager_<T>*>(this)->getRawPointer(index);
}
