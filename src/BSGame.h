#ifndef GAME_H
#define GAME_H
#include <Arduboy2.h>
#include <Tinyfont.h>
#include "BSArdBitmap.h"
#include "BSPlayer.h"

// helper to calc deltatime
#define MILLIS_SINCE(MILLIS) (millis() - MILLIS)

const char* shipNameForLength(uint8_t length);

typedef enum BSGameState{
  BSGameStateMenu = 0,
  BSGameStatePlaying,
  BSGameStateEnding
}BSGameState;

class BSGame{
  public:
    BSGame();

    // ======================
    // Methods

    /// starts the game and calls all methods in neccesary order
    void run();

private:
    // ======================
    // Methods

    // Game methods
    void showMenu();
    void startNewGame();
    void resetGame();
    void showPlaceShips();

    // draws the map of a given player to the screen
    void drawMapAtPosition(int16_t posX, int16_t posY, BSPlayer *aPlayer, bool drawShips);

    /// Draws a ship with the given settings
    void drawShipAtPosition(int16_t posX, int16_t posY, uint8_t length, bool vertical);

    // DEBUG
    void printMapTileBinary(uint8_t posX, uint8_t posY, uint16_t val);

    // ======================
    // Data
    Arduboy2 arduboy;
    ArdBitmap ardbitmap = ArdBitmap(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    Tinyfont tinyfont = Tinyfont(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    BSGameState gameState;
    Point cursorPosition;

    // players
    BSPlayer player1;
    BSPlayer player2;
};
#endif
