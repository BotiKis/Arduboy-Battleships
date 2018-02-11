#ifndef BSGAMGE_AI
#define BSGAMGE_AI
#include <Arduboy2.h>
#include "BSMapTileData.h"
#include "BSPlayer.h"

enum class AITileValue: int8_t{
  ShipHit    = -3,
  Mountain   = -2,
  Miss       = -1,
  Zero       =  0
};

class BSGameAI{
public:
  BSGameAI(BSPlayer *enemy);
  // Returns coordinates in dimensions of BS_MAP_SIZE
  Point getNextShotPosition();

  void createProbabilityMap();
  void udpateProbabilityMap();
  void markCoordinatesAs(Point location, AITileValue tileValue);

private:
  // stores the probabilities
  // values 0-100 tell the chance of hitting
  // values < 0 tell states defined by AITileValue
  AITileValue probabilityMap[BS_MAP_SIZE][BS_MAP_SIZE];

  // enemy playerMap
  // needed for ships left and moutain positions, no cheating promissed ;)
  BSPlayer *enemyPlayer;
  bool shipFitsAtPosition(uint8_t posX, uint8_t posY, uint8_t length, bool vertical);
};

class BSProbabilityCoordinate{
public:
  BSProbabilityCoordinate();
  BSProbabilityCoordinate(Point p, int8_t prob);
  int16_t x,y;
  int8_t probability;
};

#define BS_MAX_STACK_SIZE 8
class BSProbabilityStack{
public:
  void add(BSProbabilityCoordinate coord);
  BSProbabilityCoordinate getHighest();
  BSProbabilityCoordinate getLowest();
  int8_t getLowestIndex();
  BSProbabilityCoordinate getRandom();
  void reset();

private:
  uint8_t count;
  BSProbabilityCoordinate coordinates[BS_MAX_STACK_SIZE];
};

#endif
