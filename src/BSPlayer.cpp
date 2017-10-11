#include "BSPlayer.h"

BSPlayer::BSPlayer(){
  resetPlayer();
  playerName = new char[PLAYERNAME_LENGTH+1];
}

BSPlayer::~BSPlayer(){
  delete[] playerName;
}

void BSPlayer::resetPlayer(){
  cursorPosition = {0,0};
  
  // clean map
  for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
    for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {
      playerMap[i][j] = 0x00;
    }
  }

  // reset shipCount
  remainingShips = 0;

  // place random mountains
  for (uint8_t i = 0; i < 3;) {
    /* code */
    uint8_t posX = random(BS_MAP_SIZE);
    uint8_t posY = random(BS_MAP_SIZE);
    if (MAP_TILE_TYPE(getMapTileAtPosition(posX, posY)) != MAP_TILE_TYPE_MOUNTAIN) {
      setMapTileAtPosition(posX, posY, (MAP_TILE_TYPE_MOUNTAIN << MAP_TILE_TYPE_POS)); // set to mountain
      i++;
    }
  }
}

void BSPlayer::addShip(uint8_t posX, uint8_t posY, uint8_t length, uint8_t shipIndex , bool vertical){
  uint8_t x, y;
  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (vertical?0:i);
    y = posY + (vertical?i:0);

    // Create tile data
    uint16_t tileData = 0;

    // tile type
    tileData |= (MAP_TILE_TYPE_SHIP << MAP_TILE_TYPE_POS);

    // Ships index
    tileData |= ( (shipIndex & 0b111) << MAP_SHIP_INDEX_BIT_POS);

    // ship length
    tileData |= ( (length & 0b111) << MAP_SHIPLENGTH_BIT_POS);

    // tile index
    tileData |= ( (i & 0b111) << MAP_SHIPTILE_INDEX_BIT_POS);

    // vertical
    if (vertical) tileData |= MAP_FLAG_IS_VERTICAL;

    // write to map
    playerMap[y][x] = tileData;
  }

  remainingShips++;
}

bool BSPlayer::isShipTileAtPosition(uint8_t posX, uint8_t posY){

  uint16_t tileData = playerMap[posY][posX];

  return MAP_TILE_TYPE(tileData) == MAP_TILE_TYPE_SHIP;
}

void BSPlayer::destroyTileAtPosition(uint8_t posX, uint8_t posY){
  // check if it's a shiptile which can be destroyed
  if (isShipTileAtPosition(posX, posY)) {
      // mark shiptile as destroyed
      playerMap[posY][posX] |= MAP_FLAG_IS_DESTROYED;

      // check if ship have remaining tiles
      uint8_t shipIndex = MAP_SHIP_INDEX(playerMap[posY][posX]);
      uint16_t currentTile = 0;

      for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
        for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {
          if (isShipTileAtPosition(j,i)) {
            currentTile = playerMap[i][j];
            if (MAP_SHIP_INDEX(currentTile) == shipIndex && !(currentTile & MAP_FLAG_IS_DESTROYED)) {
              return;
            }
          }
        }
      }

      // if we are here, there are no remaining shipTiles with the same index
      // decrease shipcount
      remainingShips = max(0, remainingShips-1);
  }
}

bool BSPlayer::detectShipCollisionOnMap(uint8_t posX, uint8_t posY, uint8_t length, bool vertical){

  // check for out of bounds
  if (posX < 0 ||
      posY < 0 ||
      (posX + (vertical?0:length-1)) >= BS_MAP_SIZE ||
      (posY + (vertical?length-1:0)) >= BS_MAP_SIZE) return true;

  // check for mountains or other ships on the map
  uint8_t x, y;
  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (vertical?0:i);
    y = posY + (vertical?i:0);

    // check for mountain
    if(MAP_TILE_TYPE(playerMap[y][x]) == MAP_TILE_TYPE_MOUNTAIN) return true;
    if(MAP_TILE_TYPE(playerMap[y][x]) == MAP_TILE_TYPE_SHIP) return true;
  }
  return false;
}

uint16_t BSPlayer::getMapTileAtPosition(uint8_t x, uint8_t y){
  return playerMap[x][y];
}

void BSPlayer::setMapTileAtPosition(uint8_t x, uint8_t y, uint16_t mapTileData){
  playerMap[x][y] = mapTileData;
}

void BSPlayer::setPlayerName(const char *aName){
  strcpy(playerName, aName);
}

char* BSPlayer::getPlayerName(){
  return playerName;
}

uint8_t BSPlayer::getRemainingShips(){
  return remainingShips;
}

Point BSPlayer::getCursorPosition(){
  return cursorPosition;
}

void BSPlayer::setCursorPosition(Point newPosition){
  cursorPosition.x = newPosition.x;
  cursorPosition.y = newPosition.y;
}
