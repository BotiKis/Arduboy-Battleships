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
  void addShip(uint8_t posX, uint8_t posY, uint8_t length, uint8_t shipIndex, bool vertical);

  // returns true if that tile contains a ship
  bool isShipTileAtPosition(uint8_t posX, uint8_t posY);

  // destroys tile at position if its a shiptile and decrements the
  // shipcount when all shiptiles of the ships are destroyed
  void destroyTileAtPosition(uint8_t posX, uint8_t posY);

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
  Point getCursorPosition();
  void setCursorPosition(Point newPosition);

private:
  // stores the players map
  uint16_t playerMap[BS_MAP_SIZE][BS_MAP_SIZE] = {{0}};
  char *playerName;
  uint8_t remainingShips;
  Point cursorPosition;
};

#endif
