#include <iostream>
#include "game.h"
#include "player.h"
#include "condition.h"
#include "location.h"
#include "exception.h"
#include "util.h"

// TODO: check includes for fwd declaration opportunities

int main() {
  try {
    Game game("../json/game.json");
    game.play();

    
  } catch (Exception::ExceptionBase &ex) {
    std::cerr << ex.ExceptionBase::what() << '\n';
  } catch (Exception::JSONFormatError &ex) {
    std::cerr << ex.JSONFormatError::what() << '\n';
  }
  
}
