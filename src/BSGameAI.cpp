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
