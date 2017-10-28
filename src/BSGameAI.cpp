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
BSProbabilityStack probabilities;

  // go through whole map and get highest probability
  for (uint8_t y = 0; y < BS_MAP_SIZE; y++) {
    for (uint8_t x = 0; x < BS_MAP_SIZE; x++) {
      BSProbabilityCoordinate coord({x,y}, probabilityMap[y][x]);
      probabilities.add(coord);
    }
  }
  BSProbabilityCoordinate highest = probabilities.getHighest();

  return {highest.x, highest.y};
}

void BSGameAI::markCoordinatesAs(Point location, AITileValue tileValue){
  probabilityMap[location.y][location.x] = ((int8_t)tileValue);
  createProbabilityMap();
}

void BSGameAI::createProbabilityMap(){

  // go through whole map an calc probabilities for every ships left
  for (uint8_t y = 0; y < BS_MAP_SIZE; y++) {
    for (uint8_t x = 0; x < BS_MAP_SIZE; x++) {

      // for every ship left
      for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
        // if ship exsists
        if (enemyPlayer->getRemainingShipLenghtAtIndex(i) > 0) {

          // get shipLength
          uint8_t shipLength = enemyPlayer->getShipLenghtAtIndex(i);

          // Horizontal
          if (shipFitsAtPosition(x, y, shipLength, false)){
            for (uint8_t tilePos = 0; tilePos < shipLength; tilePos++) {
              probabilityMap[y][x+tilePos] += 1;
            }
          }

          // vertical
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

    // check for obstacle
    if(probabilityMap[y][x] == AITileValueMountain) return false;
    if(probabilityMap[y][x] == AITileValueShipHit) return false;
    if(probabilityMap[y][x] == AITileValueMiss) return false;
  }
  return true;
}

/////////////////////////////////////////////
// BSProbabilityCoordinate

BSProbabilityCoordinate::BSProbabilityCoordinate(Point p, int8_t prob){
  x = p.x;
  y = p.y;
  probability = prob;
}

BSProbabilityCoordinate::BSProbabilityCoordinate(){
  x = y = probability = -1;
}


/////////////////////////////////////////////
// BSProbabilityStack

void BSProbabilityStack::add(BSProbabilityCoordinate coord){

  // if we have free space in the array add in
  if (count < BS_MAX_STACK_SIZE-1) {
     count++;
     coordinates[count] = coord;
  }
  // if not replace the smallest if it's bigger
  else{
    BSProbabilityCoordinate lowestCoord = getLowest();
    if (coord.probability > lowestCoord.probability) {
      // save the lowest
      int8_t idx = getLowestIndex();
      coordinates[idx] = coord;
    }
  }
}

BSProbabilityCoordinate BSProbabilityStack::getHighest(){
  BSProbabilityCoordinate aCoord;
  aCoord = coordinates[0];

  for (uint8_t i = 1; i < BS_MAX_STACK_SIZE; i++) {
    if(aCoord.probability < coordinates[i].probability)
      aCoord = coordinates[i];
  }

  return aCoord;
}

BSProbabilityCoordinate BSProbabilityStack::getLowest(){
  BSProbabilityCoordinate aCoord;
  aCoord = coordinates[0];

  for (uint8_t i = 1; i < BS_MAX_STACK_SIZE; i++) {
    if(aCoord.probability >= coordinates[i].probability)
      aCoord = coordinates[i];
  }

  return aCoord;
}

int8_t BSProbabilityStack::getLowestIndex(){
  BSProbabilityCoordinate aCoord;
  aCoord = coordinates[0];
  int8_t lIndex = 0;

  for (uint8_t i = 1; i < BS_MAX_STACK_SIZE; i++) {
    if(aCoord.probability >= coordinates[i].probability){
      aCoord = coordinates[i];
      lIndex = i;
    }
  }

  return lIndex;
}

BSProbabilityCoordinate BSProbabilityStack::getRandom(){
  if (count <= 0) {
    BSProbabilityCoordinate aCoord;
    return aCoord;
  }
  return coordinates[random(count)];
}

void BSProbabilityStack::reset(){
  for (uint8_t i = 0; i < BS_MAX_STACK_SIZE; i++) {
    coordinates[i].x = coordinates[i].y = coordinates[i].probability = -1;
  }
  count = 0;
}
