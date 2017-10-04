#include "BSGame.h"
#include "BSIsometricBitmaps.h"
#include "BSMapTileData.h"

void BSGame::showPlaceShips(){
  // Shipdata for players
  uint8_t tempShipList[BS_SHIPS_PER_PLAYER] = {2,2,2,3,3,4,5,0};

  // get number of ships
  int8_t trueShipCount = 0;
  for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
    if(tempShipList[i] > 0) trueShipCount++;
  }

  // point where the map will be drawn
  Point cameraPosition = {0,0};
  static const Point mapOrigin = {26, 0};

  // verticality
  bool placeVertical = false;

  // animatiors
  bool animatorCursor = false;

  while (trueShipCount > 0) {
    uint8_t currentShipLength = tempShipList[trueShipCount-1];

    // Game loop
    while(true){

      // Get input
      arduboy.pollButtons();

      // Handle ship placement
      if (arduboy.justPressed(B_BUTTON)){
        if (!player1.detectShipCollisionOnMap(cursorPosition.x, cursorPosition.y, currentShipLength, placeVertical)) {
          // place ship
          player1.writeShipToMap(cursorPosition.x, cursorPosition.y, currentShipLength, trueShipCount-1, placeVertical);

          trueShipCount--;
          break;
        }
        else{
          tinyfont.setCursor(76,22);
          tinyfont.print("NOPE");
        }
        arduboy.display();
        delay(1000);
      }

      // flip orientation
      if (arduboy.justPressed(A_BUTTON)){
        placeVertical = !placeVertical;
      }

      // Move cursor
      if (arduboy.justPressed(DOWN_BUTTON)){
        cursorPosition.y++;
      }
      if (arduboy.justPressed(UP_BUTTON)){
        cursorPosition.y--;
      }
      if (arduboy.justPressed(LEFT_BUTTON)){
        cursorPosition.x--;
      }
      if (arduboy.justPressed(RIGHT_BUTTON)){
        cursorPosition.x++;
      }
      cursorPosition.x = max(cursorPosition.x, 0);
      cursorPosition.x = min(cursorPosition.x, BS_MAP_SIZE-1);
      cursorPosition.y = max(cursorPosition.y, 0);
      cursorPosition.y = min(cursorPosition.y, BS_MAP_SIZE-1);

      // wait for next frame with drawing
      if (!arduboy.nextFrame()) continue;

      // handle animators
      // cursor
      if (arduboy.everyXFrames(3)) animatorCursor = !animatorCursor;

      // Drawing
      arduboy.clear();

      // Map
      // tile height is 32 and width 16, all sprites are 32x32 for simplicity and overdrawing
      cameraPosition.x = mapOrigin.x - (cursorPosition.x - cursorPosition.y)*16*gameMapZoom;
      cameraPosition.y = mapOrigin.y - (cursorPosition.x + cursorPosition.y)*8*gameMapZoom;
      drawMapAtPosition(cameraPosition.x, cameraPosition.y, &player1, true);

      // draw cursor
      if(animatorCursor) ardbitmap.drawCompressedResized(mapOrigin.x, mapOrigin.y, BitmapCursorFull, WHITE, ALIGN_H_LEFT, MIRROR_NONE, gameMapZoom);

      drawShipAtPosition(mapOrigin.x, mapOrigin.y, currentShipLength, placeVertical);

      // Infobox
      arduboy.fillRect(73,0,55,19,WHITE);
      arduboy.fillRect(74,0,54,18,BLACK);
      tinyfont.setCursor(76,3);
      tinyfont.print(F("PLACE SHIP"));

      tinyfont.setCursor(76,10);
      char shipNameBuf[16] = "\0";
      strcpy(shipNameBuf, shipNameForLength(currentShipLength));
      tinyfont.print(shipNameBuf);

      arduboy.display();
    }
  }

}
