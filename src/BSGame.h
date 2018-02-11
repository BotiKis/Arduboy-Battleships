#ifndef GAME_H
#define GAME_H
#include <Arduboy2.h>
#include <Tinyfont.h>
#include "BSArdBitmap.h"
#include "BSPlayer.h"
#include "BSGameAI.h"

// helper to calc deltatime
#define MILLIS_SINCE(MILLIS) (millis() - MILLIS)
#define CURSOR_ANIMATION_TIME 15 // it's in frames not millis!

static inline bool pointIsEqualToPoint(Point a, Point b){
  return (a.x == b.x && a.y == b.y);
}

const char* shipNameForLength(uint8_t length);

enum class BSGameState:uint8_t{
  Menu = 0,
  SinglePlayer,
  MultiPlayer,
  PlayingNextTurn,
  PlayingCancelAction,
  Options
};

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
    BSGameState showMenu();
    void startNewSinglePlayerGame();
    void runSinglePlayerGame();
    void startNewMultiPlayerGame();
    void runMultiPlayerGame();

    void showPlaceShipsForPlayer(BSPlayer *aPlayer);
    void showTurnOfPlayer(BSPlayer *aPlayer, BSPlayer *aOpponent);
    void showTurnOfAI();

    /// Draws the full explosion animation on screen
    BSGameState showAimMenuOnPlayersMap(Point mapOrigin, Point cursorPos, BSPlayer *aPlayer);

    // Draws the full explosion animation on screen
    void drawExplosionAnimation(Point mapOrigin, Point cursorPos, BSPlayer *aPlayer);

    // draws the map of a given player to the screen
    void drawMapAtPosition(int16_t posX, int16_t posY, BSPlayer *aPlayer, bool drawShips);

    /// Draws a ship with the given settings
    void drawShipAtPosition(int16_t posX, int16_t posY, uint8_t length, bool vertical);


    // Animates players maps
    void animateFromPlayerToPlayer(BSPlayer *startPlayer, BSPlayer *endPlayer, bool animateUp);

    // fills player 2 with random data
    void createMapForAI();

    // displays a simple dialog with the given text
    void showOKDialog(const char *dialogTitle);

    // ======================
    // Data
    Arduboy2 arduboy;
    ArdBitmap ardbitmap = ArdBitmap(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    Tinyfont tinyfont = Tinyfont(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    BSGameState gameState;

    // Shipdata for players
    uint8_t const playerShipList[BS_SHIPS_PER_PLAYER] = {2,0,0,0,0,0,0,0};

    // players
    BSPlayer player1;
    BSPlayer player2;

    // AI
    BSGameAI *gameAI = nullptr; // init and release on need

    // default position of the map
    const Point mapOrigin = {26, 0};
};
#endif
