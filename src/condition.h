#ifndef CONDITION_H
#define CONDITION_H

#include <functional>
#include <memory>
#include <vector>
#include "json_fwd.hpp"

class Item;
class ObjectPointer;

class Condition {
  std::function<bool()> _check;
  std::string const _fail;
  std::string const _success;
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

  
  Condition(std::string const &msg = "");
  Condition(Condition const &) = default;
  Condition(Condition&&) = default;
  Condition(ComparisonType comp,
	    ObjectPointer target,
	    ObjectPointer item, // should this be more typesafe?
	    size_t value,
	    std::string const &failMsg = "",
	    std::string const &successMsg = "");
  
  Condition(ObjectPointer target,
	    std::string const &state,
	    bool value,
	    std::string const &failMsg = "",
	    std::string const &successMsg = "");

  Condition(LogicType op, std::vector<std::unique_ptr<Condition>> &&operands,
	    std::string const &failMsg = "",
	    std::string const &successMsg = "");

  inline bool eval() const { return _check(); }
  inline std::string const &successString() const { return _success; }
  inline std::string const &failString() const { return _fail; }
  inline std::string const &resultString(bool result) const { return result ? _success : _fail; }
  inline bool empty() const { return _empty; }
  void clear();

  static std::unique_ptr<Condition> construct(nlohmann::json const &obj, std::string path = "");
}; // class Condition


#endif // CONDITION_H
