#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <vector>

class Dictionary;
class ObjectManager;

namespace Game {

  extern Dictionary g_dict;
  extern ObjectManager g_objectManager;

  void load(std::string const &rootFilename);
  void play();
  void save(std::string const &saveFilename);
  void restore(std::string const &loadFilename);

} // namespace Global

#endif //GLOBAL_H
