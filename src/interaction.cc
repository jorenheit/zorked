#include "json.hpp"
#include "interaction.h"
#include "util.h"
#include "item.h"
#include "exception.h"
#include "game.h"

using json = nlohmann::json;
using enum StringTransform;

Interaction::Interaction(Item *tool,
			 std::vector<std::string> const &verbs,
			 std::unique_ptr<Condition> &&condition,
			 std::vector<std::unique_ptr<Effect>> &&successEffects,
			 std::vector<std::unique_ptr<Effect>> &&failEffects):
  _tool(tool),
  _verbs(verbs),
  _condition(std::move(condition)),
  _successEffects(std::move(successEffects)),
  _failEffects(std::move(failEffects))
{}

std::unique_ptr<Interaction> Interaction::construct(std::string const &key, json const &obj) {
  std::vector<std::string> const vec = split(transformString<NormalizeSpaces, ToLower>(key), '.');

  std::vector<std::string> verbs;
  verbs.push_back(normalizeString(vec[0]));
  for (std::string const &verb: obj.at("verbs").get<std::vector<std::string>>()) {
    verbs.push_back(normalizeString(verb));
  }

  
  std::string toolID;
  if (vec.size() != 1) {
    if (vec.size() != 3  || vec[1] != "with") {
      throw Exception::SpecificFormatError(obj.at("_path"),
					   "Interaction must be formatted as either a single verb or in the form '[verb].with.[item-key]'.");
    }
    toolID = vec[2];
  }

  Item *tool = nullptr;
  if (not toolID.empty()) {
    auto pointer = Global::g_objectManager.get(toolID);
    if (not pointer) {
      throw Exception::UndefinedReference(obj.at("_path"), toolID);
    }
    tool = pointer.get<Item*>();
  }

  std::unique_ptr<Condition> condition = Condition::construct(obj.at(verbs[0] + "-condition"));
  std::vector<std::unique_ptr<Effect>> success, fail;
  for (auto const &[key, value]: obj.at("+").items()) {
    if (key == "_path") continue;
    success.emplace_back(Effect::construct(key, value, obj.at("_path")));
  }
  
  for (auto const &[key, value]: obj.at("-").items()) {
    if (key== "_path") continue;
    fail.emplace_back(Effect::construct(key, value, obj.at("_path")));
  }
  
  return std::make_unique<Interaction>(tool, verbs, std::move(condition), std::move(success), std::move(fail));
}
