#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>

enum Direction {
  North,
  East,
  South,
  West,
  Up,
  Down,
  // ... Behind, Around?
  NumDir
};

Direction directionFromString(std::string const &str);
std::string directionToString(Direction dir);
Direction oppositeDirection(Direction dir);

#endif
