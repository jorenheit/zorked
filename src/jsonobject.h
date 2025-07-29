#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include <iostream>
#include <fstream>

#include "exception.h"
#include "json.hpp"

using json = nlohmann::json;

class JSONObject;
  
template <typename T>
struct JSONObjectType;

template <>
struct JSONObjectType<bool> {
  static constexpr json::value_t type = json::value_t::boolean;
  static constexpr char const *string = "boolean";
};

template <>
struct JSONObjectType<std::string> {
  static constexpr json::value_t type = json::value_t::string;
  static constexpr char const *string = "string";
};

template <>
struct JSONObjectType<int> {
  static constexpr json::value_t type = json::value_t::number_integer;
  static constexpr char const *string = "number";
};

template <>
struct JSONObjectType<size_t> {
  static constexpr json::value_t type = json::value_t::number_unsigned;
  static constexpr char const *string = "number";
};

template <>
struct JSONObjectType<double> {
  static constexpr json::value_t type = json::value_t::number_float;
  static constexpr char const *string = "number";
};

template <>
struct JSONObjectType<JSONObject> {
  static constexpr json::value_t type = json::value_t::object;
  static constexpr char const *string = "object";
};

template <>
struct JSONObjectType<std::vector<JSONObject>> {
  static constexpr json::value_t type = json::value_t::array;
  static constexpr char const *string = "array";
};

    
class JSONObject: public json {
  std::string _path;
  std::string _trace;
  
public:
  inline JSONObject():
    JSONObject(json::object(), "", "")
  {}
    
  inline JSONObject(json const &obj,
		    std::string const &filename = "",
		    std::string const &parent = "root"):
    json(obj),
    _path(filename),
    _trace(parent)
  {}
    
  inline std::string const &path()  const { return _path; }
  inline std::string const &trace() const { return _trace; }

  template <typename T>
  T get() const {
    assert(JSONObjectType<T>::type == json::type() && "Cannot convert JSONObject to T");
    return this->json::get<T>();
  }

  template <typename T>
  T get(size_t index) const {
    assert(this->is_object() && "get(index) can only be called on arrays");
    assert(this->size() > index && "index out of bounds");
    
    return this->get<std::vector<JSONObject>>()[index];
  }
  
  template <typename T>
  T get(std::string const &key) const {
    assert(this->is_object() && "get(key) can only be called on objects");
      
    if (!this->contains(key)) {
      throw Exception::ExpectedField(_path, _trace, key);
    }

    if (this->at(key).type() != JSONObjectType<T>::type) {
      throw Exception::InvalidFieldType(_path, _trace, key,
					JSONObjectType<T>::string,
					this->at(key).type_name());
    }

    if constexpr (std::is_same_v<T, JSONObject>) {
      return {this->at(key), _path, _trace + "::" + key};
    }
    else if constexpr (std::is_same_v<T, std::vector<JSONObject>>) {
      std::vector<JSONObject> result;
      size_t idx = 0;
      for (json const &obj: this->at(key)) {
	result.emplace_back(obj, _path, _trace + "::" + key + "::[" + std::to_string(idx++) + "]");
      }
      return result;
    }
    else {
      return this->at(key);
    }
  }

  template <typename T>
  T getOrDefault(std::string const &key, T const &def = {}) const {
    try {
      return get<T>(key);
    }
    catch (Exception::ExpectedField const &) {
      return def;
    }
    catch (...) {
      throw;
    }
  }

  class const_iterator {
    json::const_iterator _it;
    JSONObject const *_parent;
  public:
    const_iterator(json::const_iterator it, JSONObject const *parent):
      _it(it),
      _parent(parent)
    {
      assert((_parent->is_object() || _parent->is_array()) && "Can only iterate over objects or arrays");
    }

    inline std::string key() const {
      if (_parent->is_object()) return _it.key();
      // parent is an array
      size_t idx = std::distance(_parent->begin()._it, _it);
      return std::to_string(idx);
    }

    inline JSONObject value() const {
      return {
	_parent->is_object() ? _it.value() : *_it,
	_parent->path(),
	_parent->trace() + "::" + key()
      };
    }
	

    inline const_iterator &operator++() {
      ++_it;
      return *this;
    }

    inline bool operator!=(const_iterator const &other) const {
      return _it != other._it;
    }

    inline std::pair<std::string, JSONObject> operator*() const {
      return {key(), value()};
    }
      
  }; // const_iterator

  inline const_iterator begin() const {
    return {this->json::begin(), this};
  }

  inline const_iterator end() const {
    return {this->json::end(), this};
  }
    
}; // class JSONObject

inline JSONObject parseEntireFile(std::filesystem::path const &filename) {
  std::ifstream file(filename);
  if (!file) {
    throw Exception::ErrorOpeningFile(filename);
  }

  // TODO: catch parse exceptions and improve readability
  return JSONObject(json::parse(file), filename);
}

#endif //JSONOBJECT_H
