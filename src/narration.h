#ifndef NARRATION_H
#define NARRATION_H

#include <sstream>
#include "util.h"
#include "game.h"

namespace Narration {
  using enum StringTransform;
  
  inline std::string a_or_an(std::string const &noun) {
    // TODO: make better
    return ((std::string("aeiouAEIOU").find(noun[0]) == std::string::npos) ? "a " : "an ") + noun;
  };

  inline std::string is_or_are(size_t n) {
    return (n == 1) ? "is" : "are";
  }

  inline std::string to_singular(ObjectDescriptor const &object) {
    ObjectDescriptor tmp = object;
    if (object.number == 1) return object.str();
    tmp.noun = Global::g_dict.singularize(tmp.noun);
    return tmp.str();
  }

  inline std::string to_plural(ObjectDescriptor const &object) {
    ObjectDescriptor tmp = object;
    if (object.number > 1) return object.str();
    tmp.noun = Global::g_dict.pluralize(tmp.noun);
    return tmp.str();
  }

  inline std::string to_singular_or_plural(ObjectDescriptor const &object, size_t n) {
    return (n == 1) ? to_singular(object) : to_plural(object);
  }
  
  inline std::string cannot_go_there(std::string const &dir) {
    return "You can't go " + dir + " from here.";
  }

  inline std::string you_already_own(ObjectDescriptor const &object) {
    return "You already have the " + object.str() + ".";
  }

  inline std::string you_do_not_own(ObjectDescriptor const &object) {
    return "You do not have " + a_or_an(object.str()) + ".";
  }

  inline std::string you_do_not_own_n(ObjectDescriptor const &object) {
    return "You do not have that many " + to_plural(object) + ".";
  }
  
  inline std::string there_is_no(ObjectDescriptor const &object) {
    return "There " + is_or_are(object.number) + " no "  + object.str() + " here.";
  }

  inline std::string you_took(ObjectDescriptor const &object) {
    return "You took the " + object.str() + "!";
  }

  inline std::string you_took_from(ObjectDescriptor const &inner, ObjectDescriptor const &outer) {
    return "You took the " + inner.str() + " from the " + outer.str() + "!";
  }
  
  inline std::string you_could_not_take(ObjectDescriptor const &object) {
    return "You were not able to take the " + object.str() + ".";
  }

  inline std::string you_could_not_take_from(ObjectDescriptor const &inner, ObjectDescriptor const &outer) {
    return "You were not able to take the " + inner.str() + " from the " + outer.str() + ".";
  }
  
  inline std::string which_one(ObjectDescriptor const &object) {
    return "Which " + object.str() + "?";
  }

  inline std::string does_not_contain(ObjectDescriptor const &outer, ObjectDescriptor const &inner) {
    return "The " + outer.str() + " does not contain " + a_or_an(inner.str()) + ".";
  }

  inline std::string you_dropped(ObjectDescriptor const &object) {
    return "You dropped" + std::string(object.number == 1 ? " the " : " ") + object.str() + ".";
  }

  inline std::string empty_inventory() {
    return "You don't have any items on you.";
  }

  inline std::string list_items(std::string const &header, std::vector<std::string> const &labels) {
    std::ostringstream oss;
    oss << transformString<ToUpper>(header) << "\n";
    for (std::string const &l: labels)
      oss << " * " << l << '\n';
    return oss.str();
  }

  inline std::string nothing_to_see() {
    return "You do not see anything out of the ordinary.";
  }
  
  inline std::string dont_know() {
    return "I don't know what that means.";
  }

  inline std::string not_portable(ObjectDescriptor const &object) {
    return "The " + object.str() + " " + is_or_are(object.number) + " not portable.";
  }

  inline std::string dont_know_how_to(std::string const &verb, ObjectDescriptor const &object, ObjectDescriptor const &tool) {
    std::string result = "I don't know how to " + verb;
    if (not object.str().empty()) result += " the "  + object.str();
    if (not tool.str().empty())   result += " with " + a_or_an(tool.str());
    result += ".";
    return result;
  }

  inline std::string only_n_here(ObjectDescriptor const &object, size_t n) {
    return "There " + is_or_are(n) + " only " + std::to_string(n) + " " + to_singular_or_plural(object, n) + " here.";
  }

  inline std::string can_inspect_one_at_a_time(ObjectDescriptor const &object) {
    return "I can only inspect one " + to_singular(object) + " at a time.";
  }

  inline std::string can_interact_with_only_one_object_at_a_time(std::string const &verb, ObjectDescriptor const &object) {
    return "I can't " + verb + " multiple " + to_plural(object) + " at the same time.";
  }

  inline std::string can_interact_with_only_one_tool_at_a_time(std::string const &verb, ObjectDescriptor const &object, ObjectDescriptor const &tool) {
    return "I can't " + verb + " the " + object.str() + " with multiple " + to_plural(tool) + " at the same time.";
  }


};

#endif // NARRATION_H
