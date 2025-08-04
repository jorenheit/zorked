#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <vector>

#include "parser.h"
#include "objectmanager.h"
#include "dictionary.h"

namespace Global {
  extern ObjectManager g_objectManager;
  extern Dictionary    g_dict;
}

class Game {
public:
  Game(std::string const &rootFilename) {
    load(rootFilename);
  }

  void play();
  void save(std::string const &saveFilename);
  void restore(std::string const &loadFilename);

private:
  void load(std::string const &rootFilename);
  
};

#endif //GLOBAL_H
