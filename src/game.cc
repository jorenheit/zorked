#include <fstream>
#include <iostream>
#include <filesystem>

#include "game.h"
#include "dictionary.h"
#include "objectmanager.h"
#include "parser.h"
#include "player.h"
#include "condition.h"
#include "location.h"
#include "narration.h"
#include "json.hpp"

using json = nlohmann::json;

// TODO: rename this stuff properly
nlohmann::json preprocess(std::string const &rootFilename);

namespace Game {

  Dictionary g_dict;
  ObjectManager g_objectManager;

  void load(std::string const &rootFilename) {
    preprocess(rootFilename);
    g_objectManager.build();
  }

  void play() {
    Parser parser(g_dict);
    Player *player = g_objectManager.player();
    
    while (true) {
      Location *current = player->getLocation();

      std::cout << current->label() << '\n';
      if (!current->visited()) {
	std::cout << current->description() << " ";
      }
      current->visit();

      for (auto const &item: current->items()) {
	if (item->common()) continue;
	std::cout << item->description() << " ";
      }
      std::cout << '\n';

      std::string input;
      std::getline(std::cin, input);
      std::unique_ptr<Action> action = parser.parseAction(input);
      if (action) {
	std::cout << action->exec() << '\n';
      }
      else {
	std::cout << Narration::dont_know() << '\n';
      }
    }
  }

  void save(std::string const &saveFilename) {
    (void)saveFilename;
    std::cerr << "TODO: implement save()\n";
    std::exit(1);
    // std::ofstream out(saveFilename);


    // json locations = json::object();
    // for (auto const &[id, ptr]: g_locations) {
    //   locations[id] = *ptr;
    // }

    // json items = json::object();
    // for (auto const &[id, ptr]: g_localItems) {
    //   items[id] = *ptr;
    // }

    // json data = {};
    // data["player"] = *g_player;
    // data["items"] = std::move(items);
    // data["locations"] = std::move(locations);
	 
    // out << data.dump(2) << '\n';
  }

  void restore(std::string const &saveFilename) {
    (void)saveFilename;
    std::cerr << "TODO: implement restore()\n";
    std::exit(1);
    
  //   std::ifstream file(saveFilename);
  //   if (!file) {
  //     throw Exception::ErrorOpeningFile(saveFilename);
  //   }

  //   try {
  //     json data = json::parse(file);
  //     g_player->restore(data.at("player"));
  //     for (auto const &[id, ptr]: g_locations) {
  // 	if (not ptr->restore(data.at("locations").at(id))) {
  // 	  throw 0;
  // 	}
  //     }
  //     for (auto const &[id, ptr]: g_localItems) {
  // 	if (not ptr->restore(data.at("items").at(id))) {
  // 	  throw 0;
  // 	}
  //     }
  //   }
  //   catch (...) {
  //     throw Exception::InvalidSaveFile();
  //   }
  }
  
} // namespace Game
