#include "BSGameAI.h"

BSGameAI::BSGameAI(BSPlayer *enemy){
  enemyPlayer = enemy;

  // init map
  int16_t currentTile = 0;
  for (uint8_t y = 0; y < BS_MAP_SIZE; y++) {
    for (uint8_t x = 0; x < BS_MAP_SIZE; x++) {
      // check for mountains
      currentTile = enemyPlayer->getMapTileAtPosition(x, y);
      if (MAP_TILE_TYPE(currentTile) == MAP_TILE_TYPE_MOUNTAIN){
        probabilityMap[y][x] = AITileValueMountain;
      }
    }
  }
}

Point BSGameAI::getNextShotPosition(){
  return {0,0};
}

void BSGameAI::createProbabilityMap(){

  // go through whole map an calc probabilities for every ships left
  for (uint8_t y = 0; y < BS_MAP_SIZE; y++) {
    for (uint8_t x = 0; x < BS_MAP_SIZE; x++) {

      // for every ship left
      for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
        /* code */
        if (enemyPlayer->getRemainingShipLenghtAtIndex(i) > 0) {
          /* code */
          uint8_t shipLength = enemyPlayer->getShipLenghtAtIndex(i);
          if (shipFitsAtPosition(x, y, shipLength, false)){
            for (uint8_t tilePos = 0; tilePos < shipLength; tilePos++) {
              probabilityMap[y][x+tilePos] += 1;
            }
          }

          if (shipFitsAtPosition(x, y, shipLength, true)){
            for (uint8_t tilePos = 0; tilePos < shipLength; tilePos++) {
              probabilityMap[y+tilePos  ][x] += 1;
            }
          }

        }
      }

    }
  }

}

void BSGameAI::udpateProbabilityMap(){
}

bool BSGameAI::shipFitsAtPosition(uint8_t posX, uint8_t posY, uint8_t length, bool vertical){

  // check for out of bounds
  if (posX < 0 ||
      posY < 0 ||
      (posX + (vertical?0:length-1)) >= BS_MAP_SIZE ||
      (posY + (vertical?length-1:0)) >= BS_MAP_SIZE) return false;

  // check for mountains or other ships on the map
  uint8_t x, y;
  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (vertical?0:i);
    y = posY + (vertical?i:0);

    // check for mountain
    if(probabilityMap[y][x] == AITileValueMountain) return false;
  }
  return true;
}
