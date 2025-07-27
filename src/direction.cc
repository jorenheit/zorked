#include "direction.h"

static constexpr std::string const directionStrings[NumDir] = {
  "north",
  "east",
  "south",
  "west",
  "up",
  "down"
};


Direction directionFromString(std::string const &str) {
  for (size_t i = 0; i != NumDir; ++i) {
    if (str == directionStrings[i])
      return static_cast<Direction>(i);
  }
  return NumDir;
}

std::string directionToString(Direction dir) {
  return directionStrings[dir];
}

Direction oppositeDirection(Direction dir) {
  switch (dir) {
  case North: return South;
  case East: return West;
  case South: return North;
  case West: return East;
  case Up: return Down;
  case Down: return Up;
  default: return NumDir;
  }
}
