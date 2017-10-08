#ifndef BSANIMATIONHELPER_H
#define BSANIMATIONHELPER_H

#include <Arduboy2.h>

// returns a point between the start and endpoint where the progress must be between 0 and 100
static inline Point animatePointFromToPoint(Point startPoint, Point endPoint, uint8_t progress){
  Point result;

  result.x = startPoint.x + ((endPoint.x - startPoint.x)*progress)/100;
  result.y = startPoint.y + ((endPoint.y - startPoint.y)*progress)/100;

  return result;
}

#endif
