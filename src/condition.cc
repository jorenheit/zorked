#include "util.h"
#include "zobject.h"
#include "item.h"
#include "condition.h"
#include "game.h"
#include "exception.h"

#define OBJECTMANAGER_IMPLEMENTATION
#include "objectmanager.h"

#include "json.hpp"
using json = nlohmann::json;

// Default condition: always returns true
Condition::Condition(std::string const &msg):
  _success(msg),
  _empty(true)
{
  _check = [&]() -> bool {
    return true;
  };
}

// Condition to check if a target object contains some item
Condition::Condition(ComparisonType comp, ObjectPointer target, ObjectPointer item, size_t value,
		     std::string const &failMsg, std::string const &successMsg):
  _fail(failMsg),
  _success(successMsg)
{
  _check = [=]() -> bool {
    size_t count = target->contains(item.get<Item*>());
    switch (comp) {
    case Equal: return (count == value);
    case Greater: return (count > value);
    case Less: return (count < value);
    case GreaterOrEqual: return (count >= value);
    case LessOrEqual: return (count <= value);
    case NotEqual: return (count != value);
    default: UNREACHABLE("Comparison enum incomplete");
    } 
  };
}

// Condition to check if a target has a state-string set to some value
Condition::Condition(ObjectPointer target, std::string const &state, bool value,
		     std::string const &failMsg, std::string const &successMsg):
  _fail(failMsg),
  _success(successMsg),
  _empty(false)
{
  _check = [=]() -> bool {
    return target->getState(state) == value;
  };
}

// Compound conditions, strung together by AND or OR.
Condition::Condition(LogicType op, std::vector<std::unique_ptr<Condition>> &&operands_,
		     std::string const &failMsg, std::string const &successMsg):
  _fail(failMsg),
  _success(successMsg),
  _empty(false)
{
  std::vector<std::shared_ptr<Condition>> operands;
  for (auto &ptr: operands_)
    operands.push_back(std::move(ptr));
  
  _check = [op, operands]() -> bool {
    static std::function<bool(bool,bool)> logicFunc[] = {
      /* And */ [](bool a, bool b) { return a && b; },
      /* Or  */ [](bool a, bool b) { return a || b; },
      /* Xor */ [](bool a, bool b) { return a ^ b;  }
    };

    bool value = operands[0]->eval();
    for (size_t idx = 1; idx != operands.size(); ++idx) {
      value = logicFunc[op](value, operands[idx]->eval());
    }
    return value;
  };
}

void Condition::clear() {
  _check = [](){ return true; };
  _empty = true;
}
  
std::unique_ptr<Condition> Condition::construct(json const &obj, std::string path) {
  // The conditional node is either:
  // 1. A key/value pair 
  // 2. A logical expression composed of an operator and multiple other nodes

  auto fail    = path.empty() ? obj.at("fail").get<std::string>() : "";
  auto success = path.empty() ? obj.at("success").get<std::string>() : "";

  if (path.empty())
    path = obj.at("_path").get<std::string>();
  
  std::vector<std::unique_ptr<Condition>> result;
  for (auto const &[key, value]: obj.items()) {
    if (key == "_path") continue;
    if (key == "fail") continue;
    if (key == "success") continue;


    LogicType op = logicOperatorFromString(key);
    if (op != NumOp) {
      std::vector<std::unique_ptr<Condition>> array;
      for (json const &childNode: value) {
	array.emplace_back(Condition::construct(childNode, path + "::" + key));
      }
      result.emplace_back(std::make_unique<Condition>(op, std::move(array)));
      continue;
    }

    // We need to parse the key-string to find the condition
    std::vector<std::string> vec = split(key, '.');
    if (vec.size() != 3) {
      throw Exception::SpecificFormatError(path,
					   "Conditional expression must be of the form '[object].[is/has].[state/object]'.");
    }

    ObjectPointer zObj = Game::g_objectManager.get(vec[0]);
    if (not zObj) {
      throw Exception::UndefinedReference(path, vec[0]);
    }
      
    if (vec[1] == "is") {
      if (!value.is_boolean()) {
	throw Exception::InvalidFieldType(path, key, "boolean", value.type_name());
      }
      result.emplace_back(std::make_unique<Condition>(zObj, vec[2], value.get<bool>()));
      continue;
    }

    if (vec[1] == "has") {
      ObjectPointer zObj2 = Game::g_objectManager.get(vec[2], ObjectType::LocalItem, ObjectType::CommonItem);
      if (!zObj2) {
	throw Exception::UndefinedReference(path, vec[2]);
      }

      if (value.is_boolean()) {
	result.emplace_back(std::make_unique<Condition>(value.get<bool>() ? Greater : Equal, zObj, zObj2, 0));
	continue;
      }

      if (value.is_number()) {
	result.emplace_back(std::make_unique<Condition>(GreaterOrEqual, zObj, zObj2, value.get<size_t>()));
	continue;
      }
      
      if (value.is_object()) {
	auto count = value.at("value").get<size_t>();
	auto compStr = value.at("comparison").get<std::string>();
	ComparisonType comp = comparisonOperatorFromString(compStr);
	  
	if (comp == NumComp) {
	  throw Exception::SpecificFormatError(path, 
					       "Invalid comparison operator '", compStr, "'; must be one of {=,<,>,<=,>=,!=}.");
	}
	result.emplace_back(std::make_unique<Condition>(comp, zObj, zObj2, count));
	continue;
      }

      throw Exception::SpecificFormatError(path,
					   "Value of condition '", key, "' must be either boolean, integer or "
					   "an object containing a 'value' and 'comparison' field.");
    }

    throw Exception::SpecificFormatError(value.at("_path"),
					 "Condition may only contain 'is' or 'has', got '", vec[1], "'.");
  }

  return result.empty()
    ? std::make_unique<Condition>(success)
    : std::make_unique<Condition>(And, std::move(result), fail, success);
}

static constexpr std::string const comparisonOperatorStrings[Condition::NumComp] = {
  "=",
  ">",
  "<",
  ">=",
  "<=",
  "!="
};

static constexpr std::string const logicOperatorStrings[Condition::NumOp] = {
  "and",
  "or",
  "xor"
};

Condition::ComparisonType Condition::comparisonOperatorFromString(std::string const &str) {
  for (size_t idx = 0; idx != NumComp; ++idx) {
    if (comparisonOperatorStrings[idx] == str)
      return static_cast<ComparisonType>(idx);
  }
  
  return NumComp;
}

std::string Condition::comparisonOperatorToString(ComparisonType op) {
  return comparisonOperatorStrings[op];
}

Condition::LogicType Condition::logicOperatorFromString(std::string const &str) {
  for (size_t idx = 0; idx != NumOp; ++idx) {
    if (str == logicOperatorStrings[idx])
      return static_cast<LogicType>(idx);
  }
  return NumOp;
}			   

std::string Condition::logicOperatorToString(LogicType op) {
  return logicOperatorStrings[op];
}

