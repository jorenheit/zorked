#ifndef CONDITION_H
#define CONDITION_H

#include <functional>
#include <memory>
#include <vector>

#include "jsonobject.h"

class ZObject;
class Item;

class Condition {
  std::function<bool()> _check;
  std::string _fail;
  std::string _success;
  bool _empty = false;
  
public:
  enum ComparisonType {
    Equal,
    Greater,
    Less,
    GreaterOrEqual,
    LessOrEqual,
    NotEqual,
    NumComp
  };

  static ComparisonType comparisonOperatorFromString(std::string const &str);
  static std::string comparisonOperatorToString(ComparisonType op);
  
  enum LogicType {
    And,
    Or,
    Xor,
    NumOp
  };

  static LogicType logicOperatorFromString(std::string const &str);
  static std::string logicOperatorToString(LogicType op);

  
  Condition();
  Condition(ComparisonType comp,
	    std::shared_ptr<ZObject> target,
	    std::shared_ptr<Item> item,
	    size_t value,
	    std::string const &failMsg = "",
	    std::string const &successMsg = "");
  
  Condition(std::shared_ptr<ZObject> target,
	    std::string const &state,
	    bool value,
	    std::string const &failMsg = "",
	    std::string const &successMsg = "");

  Condition(LogicType op, std::vector<std::shared_ptr<Condition>> operands,
	    std::string const &failMsg = "",
	    std::string const &successMsg = "");

  inline bool eval() const { return _check(); }
  inline std::string const &successString() const { return _success; }
  inline std::string const &failString() const { return _fail; }
  inline bool empty() const { return _empty; }
  void clear();
  
  static std::shared_ptr<Condition> construct(JSONObject const &condObj);
}; // class Condition



#endif // CONDITION_H
