#include "BSGame.h"

// Main game
BSGame *game;

// ---------------------------------------------------
// Setup func
void setup() {
  // put your setup code here, to run once:
  game = new BSGame();
}

// ---------------------------------------------------
// loop func
void loop() {
  game->run();
}
