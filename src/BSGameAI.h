#ifndef BSGAMGE_AI
#define BSGAMGE_AI
#include <Arduboy2.h>
#include "BSMapTileData.h"
#include "BSPlayer.h"

typedef enum AITileValue{
  AITileValueShipSunk   = -3,
  AITileValueShipHit    = -2,
  AITileValueMountain   = -1,
  AITileValue0 = 0
}AITileValue;

class BSGameAI{
public:
  BSGameAI(BSPlayer *enemy);
  // Returns coordinates in dimensions of BS_MAP_SIZE
  Point getNextShotPosition();

private:
  // stores the probabilities
  // values 0-100 tell the chance of hitting
  // values < 0 tell states defined by AITileValue
  int8_t probabilityMap[BS_MAP_SIZE][BS_MAP_SIZE];

  // enemy playerMap
  // needed for ships left and moutain positions, no cheating promissed ;)
  BSPlayer *enemyPlayer;
};

#endif
