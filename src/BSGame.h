#ifndef GAME_H
#define GAME_H
#include <Arduboy2.h>
#include <Tinyfont.h>
#include "BSArdBitmap.h"

// -------------------------------------------------------
// MAP Flags
// The full game is stored in a 2d 16bit array.
// These flags define the tile at a certain coordinate.
// The coordinate can hold a TILE like water or a mountain, or a shippart.


#define MAP_TILE_TYPE_POS           0
#define MAP_SHIP_INDEX_BIT_POS      4
#define MAP_SHIPLENGTH_BIT_POS      7
#define MAP_SHIPTILE_INDEX_BIT_POS  10

#define MAP_FLAG_IS_DESTROYED    _BV(13)  // Set if the shippart is destroyed
#define MAP_FLAG_IS_VERTICAL     _BV(14)  // Set if the shipTile (thus the full ship) is vertically aligned

// 3 bits tell the index/number of the ship
#define MAP_SHIP_INDEX(V) ((V >> MAP_SHIP_INDEX_BIT_POS) & 0b111)

// 3 bits tell the index/number of the shippart inside the ship
#define MAP_SHIPTILE_INDEX(V) ((V >> MAP_SHIPTILE_INDEX_BIT_POS) & 0b111)

// 3 bits tell the length/part count of the ship
#define MAP_SHIPLENGTH(V) ((V >> MAP_SHIPLENGTH_BIT_POS) & 0b111)

// 3 bits tell the type of TILE
#define MAP_TILE_TYPE(V) ((V >> MAP_TILE_TYPE_POS) & 0b1111)

#define MAP_TILE_TYPE_WATER     0
#define MAP_TILE_TYPE_SHIP      1
#define MAP_TILE_TYPE_MOUNTAIN  2
#define MAP_TILE_TYPE_WAVE1     3
#define MAP_TILE_TYPE_WAVE2     4
#define MAP_TILE_TYPE_WAVE3     5


// helper to calc deltatime
#define MILLIS_SINCE(MILLIS) (millis() - MILLIS)

// Size of map
#define BS_MAP_SIZE 10
#define BS_SHIPS_PER_PLAYER 8

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

    //
    void drawMapAtPosition(uint16_t posX, uint16_t posY, uint16_t playerMap[BS_MAP_SIZE][BS_MAP_SIZE], bool drawShips);

    /// Draws a ship with the given settings
    void drawShipAtPosition(uint16_t posX, uint16_t posY, uint8_t length, bool vertical);

    // Returns true if the given ship collides either with a mountain or other ship.
    bool detectShipCollisionOnMap(uint8_t posX, uint8_t posY, uint8_t length, bool vertical);

    //
    void writeShipToMap(uint8_t posX, uint8_t posY, uint16_t playerMap[BS_MAP_SIZE][BS_MAP_SIZE], uint8_t length, uint8_t shipIndex, bool vertical);


    // DEBUG
    void printMapTileBinary(uint8_t posX, uint8_t posY, uint16_t val);

    // ======================
    // Data
    Arduboy2 arduboy;
    ArdBitmap ardbitmap = ArdBitmap(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    Tinyfont tinyfont = Tinyfont(arduboy.getBuffer(), Arduboy2::width(), Arduboy2::height());
    BSGameState gameState;
    Point cursorPosition;

    // Game Maps
    uint16_t player1Map[BS_MAP_SIZE][BS_MAP_SIZE] = {{0}};
    uint16_t player2Map[BS_MAP_SIZE][BS_MAP_SIZE] = {{0}};
};
#endif
