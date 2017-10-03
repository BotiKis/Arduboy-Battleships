#include "BSPlayer.h"

BSPlayer::BSPlayer(){
  resetPlayer();
}

void BSPlayer::writeShipToMap(uint8_t posX, uint8_t posY, uint8_t length, uint8_t shipIndex , bool vertical){
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
}

void BSPlayer::resetPlayer(){
  // clean map
  for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
    for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {
      playerMap[i][j] = 0x00;
    }
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
