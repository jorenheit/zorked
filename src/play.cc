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

void Game::play() {
  Parser parser(Global::g_dict);
  Player *player = Global::g_objectManager.player();
    
  while (true) {
    Location *current = player->getLocation();

    std::cout << current->label() << '\n';
    if (!current->visited()) {
      std::cout << current->description() << " ";
    }
    current->visit();

    for (Item const *item: current->items()) {
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
