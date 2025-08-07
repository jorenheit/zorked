#ifndef NARRATION_H
#define NARRATION_H

#include <sstream>
#include "util.h"

namespace Narration {
  using enum StringTransform;
  
  inline std::string a_or_an(std::string const &noun) {
    assert(not noun.empty());
    return ((std::string("aeiouAEIOU").find(noun[0]) == std::string::npos) ? "a " : "an ") + noun;
  };

  inline std::string cannot_go_there(std::string const &dir) {
    return "You can't go " + dir + " from here.";
  }

  inline std::string you_already_own(std::string const &object) {
    return "You already have the " + object + ".";
  }

  inline std::string you_do_not_own(std::string const &object) {
    return "You do not have " + a_or_an(object) + ".";
  }

  inline std::string there_is_no(std::string const &object) {
    return "There is no "  + object + " here.";
  }

  inline std::string you_took(std::string const &object) {
    return "You took the " + object + "!";
  }

  inline std::string you_took_from(std::string const &inner, std::string const &outer) {
    return "You took the " + inner + " from the " + outer + "!";
  }
  
  inline std::string you_could_not_take(std::string const &object) {
    return "You were not able to take the " + object + ".";
  }

  inline std::string you_could_not_take_from(std::string const &inner, std::string const &outer) {
    return "You were not able to take the " + inner + " from the " + outer + ".";
  }
  
  inline std::string which_one(std::string const &object) {
    return "Which " + object + "?";
  }

  inline std::string does_not_contain(std::string const &outer, std::string const &inner) {
    return "The " + outer + " does not contain " + a_or_an(inner) + ".";
  }

  inline std::string you_dropped(std::string const &object) {
    return "You dropped the " + object + ".";
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

  inline std::string not_portable(std::string const &object) {
    return "The " + object + " is not portable.";
  }

  inline std::string dont_know_how_to(std::string const &verb, std::string const &object, std::string const &tool) {
    std::string result = "I don't know how to " + verb;
    if (not object.empty()) result += " the " + object;
    if (not tool.empty()) result += " with " + a_or_an(tool);
    result += ".";
    return result;
  }
};

#endif // NARRATION_H
