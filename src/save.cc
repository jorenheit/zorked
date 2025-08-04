#include <iostream>
#include "game.h"

void Game::save(std::string const &saveFilename) {
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

void Game::restore(std::string const &saveFilename) {
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
  
