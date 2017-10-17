#ifndef BSGAMGE_AI
#define BSGAMGE_AI
#include <Arduboy2.h>
#include "BSMapTileData.h"

typedef enum AITileValue{
  AITileValueShipHit = -1,
  AITileValueShipSunk = -2,
  AITileValue0 = 0
}AITileValue;

class BSGameAI{
public:
  BSGameAI();
  // Returns coordinates in dimensions of BS_MAP_SIZE
  Point getNextShotPosition();

private:
  // stores the possibility map
  // values 0-100 tell the chance of hitting
  // values < 0 tell states defined by AITileValue
  AITileValue playerMap[BS_MAP_SIZE][BS_MAP_SIZE];
};

#endif
