#ifndef BSPLAYER_H
#define BSPLAYER_H
#include <Arduboy2.h>
#include "BSMapTileData.h"

#define PLAYERNAME_LENGTH 16

class BSPlayer{
public:
  BSPlayer();
 ~BSPlayer();

  // Resets the player data
  void resetPlayer();

  // Stores a ship to the map
  void addShip(uint8_t posX, uint8_t posY, uint8_t length, bool vertical);

  // returns true if that tile contains a ship
  bool isShipTileAtPosition(uint8_t posX, uint8_t posY);

  // destroys tile at position if its a shiptile and decrements the
  // shipcount when all shiptiles of the ships are destroyed
  // returns true if a ship has been sunk
  bool destroyTileAtPosition(uint8_t posX, uint8_t posY);

  // Returns true if the given ship collides either with a mountain or other ship.
  bool detectShipCollisionOnMap(uint8_t posX, uint8_t posY, uint8_t length, bool vertical);

  // getter and setter for a tile on the map.
  // A tile is represented by 2 Bytes, see BSMapTileData.h for more info.
  uint16_t getMapTileAtPosition(uint8_t x, uint8_t y);
  void setMapTileAtPosition(uint8_t x, uint8_t y, uint16_t mapTileData);

  // Getter and setter for the player name
  void setPlayerName(const char *aName);
  char* getPlayerName();

  uint8_t getRemainingShips();

  // accessor for cursor
  // position in dimensions of BS_MAP_SIZE
  Point getCursorPosition();
  void setCursorPosition(Point newPosition);

  // accessors for number of turns
  uint8_t getNumberOfTurns();
  void setNumberOfTurns(uint8_t turns);

  // accessors for number of turns
  uint8_t getRemainingShipTiles();

  // accessors for shiplenghts
  uint8_t getShipLenghtAtIndex(uint8_t idx);
  uint8_t getRemainingShipLenghtAtIndex(uint8_t idx);

private:
  // stores the players map
  uint16_t playerMap[BS_MAP_SIZE][BS_MAP_SIZE] = {{0}};
  char *playerName;

  // stores data
  // the original ship lenghts
  uint8_t shipLenghts[BS_SHIPS_PER_PLAYER];

  // the actual ship length
  uint8_t remainingShipTiles[BS_SHIPS_PER_PLAYER];
  uint8_t remainingShips;

  // stores the cursor position
  Point cursorPosition;

  // standard 1 but may be more for future versions
  uint8_t numberOfTurns;
};

#endif
