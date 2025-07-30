#include "util.h"
#include "condition.h"
#include "game.h"

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
Condition::Condition(ComparisonType comp, std::shared_ptr<ZObject> target, std::shared_ptr<Item> item, size_t value,
		     std::string const &failMsg, std::string const &successMsg):
  _fail(failMsg),
  _success(successMsg)
{
  _check = [=]() -> bool {
    size_t count = target->contains(item);
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
Condition::Condition(std::shared_ptr<ZObject> target, std::string const &state, bool value,
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
Condition::Condition(LogicType op, std::vector<std::shared_ptr<Condition>> const &operands,
		     std::string const &failMsg, std::string const &successMsg):
  _fail(failMsg),
  _success(successMsg),
  _empty(false)
{
  _check = [=]() -> bool {
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
  
std::shared_ptr<Condition> Condition::construct(JSONObject const &condObj) {
  // The conditional node is either:
  // 1. A key/value pair 
  // 2. A logical expression composed of an operator and multiple other nodes
  auto fail = condObj.getOrDefault<std::string>("fail");
  auto success = condObj.getOrDefault<std::string>("success");
  
  std::vector<std::shared_ptr<Condition>> result;
  for (auto const &[key, value]: condObj) {
    if (key == "fail") continue;
    if (key == "success") continue;
    
    LogicType op = logicOperatorFromString(key);
    if (op != NumOp) {
      std::vector<std::shared_ptr<Condition>> array;
      for (auto const &[idx, jsonObj]: value) {
	array.push_back(Condition::construct(jsonObj));
      }
      result.emplace_back(std::make_shared<Condition>(op, array));
      continue;
    }

    // We need to parse the key-string to find the condition
    std::vector<std::string> vec = split(key, '.');
    if (vec.size() != 3) {
      throw Exception::SpecificFormatError(value.path(), value.trace(),
					   "Conditional expression must be of the form '[object].[is/has].[state/object]'.");
    }

    std::shared_ptr<ZObject> zObj = Game::objectByID(vec[0]);
    if (not zObj) {
      throw Exception::UndefinedReference(value.path(), value.trace(), vec[0]);
    }
      
    if (vec[1] == "is") {
      result.emplace_back(std::make_shared<Condition>(zObj, vec[2], value.get<bool>()));
      continue;
    }

    if (vec[1] == "has") {
      std::shared_ptr<Item> zObj2 = std::static_pointer_cast<Item>(Game::objectByID(vec[2]));
      if (!zObj2) {
	throw Exception::UndefinedReference(value.path(), value.trace(), vec[2]);
      }

      if (value.is_boolean()) {
	result.emplace_back(std::make_shared<Condition>(value.get<bool>() ? Greater : Equal, zObj, zObj2, 0));
	continue;
      }

      if (value.is_number()) {
	result.emplace_back(std::make_shared<Condition>(GreaterOrEqual, zObj, zObj2, value.get<size_t>()));
	continue;
      }
      
      if (value.is_object()) {
	auto count = value.get<size_t>("value");
	auto compStr = value.get<std::string>("comparison");
	ComparisonType comp = comparisonOperatorFromString(compStr);
	  
	if (comp == NumComp) {
	  throw Exception::SpecificFormatError(value.path(), value.trace(),
					       "Invalid comparison operator '", compStr, "'; must be one of {=,<,>,<=,>=,!=}.");
	}
	result.emplace_back(std::make_shared<Condition>(comp, zObj, zObj2, count));
	continue;
      }

      throw Exception::SpecificFormatError(value.path(), value.trace(),
					   "Value of condition '", key, "' must be either boolean, integer or "
					   "an object containing a 'value' and 'comparison' field.");
    }

    throw Exception::SpecificFormatError(value.path(), value.trace(),
					 "Condition may only contain 'is' or 'has', got '", vec[1], "'.");
  }

  return result.empty()
    ? std::make_shared<Condition>(success)
    : std::make_shared<Condition>(And, result, fail, success);
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


size_t ConditionManager::add(JSONObject const &obj) {
  assert(not _initialized && "Calling add after process");
  _conditionProxies.push_back(obj);
  return _conditionProxies.size() - 1;
}

Condition &ConditionManager::get(size_t index) {
  assert(_initialized && "Calling get before process.");
  assert(index < _conditions.size() && "index out of bounds");
  return *_conditions[index];
}

void ConditionManager::process() {
  for (JSONObject const &jsonObj: _conditionProxies) {
    auto ptr = Condition::construct(jsonObj);
    _conditions.emplace_back(ptr);
  }
  _initialized = true;
}

