#include "BSGame.h"



void BSGame::resetGame(){

  // clean map
  for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
    for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {
      player1Map[i][j] = 0x00;
      player2Map[i][j] = 0x00;
    }
  }

  // place random mountains
  for (uint8_t i = 0; i < 3;) {
    /* code */
    uint8_t posX = random(10);
    uint8_t posY = random(10);
    if (MAP_TILE_TYPE(player1Map[posX][posY]) != MAP_TILE_TYPE_MOUNTAIN) {
      player1Map[posX][posY] |= (MAP_TILE_TYPE_MOUNTAIN << MAP_TILE_TYPE_POS); // set to mountain
      i++;
    }
  }

  // reset cursor
  cursorPosition = {0,0};
}

void BSGame::showPlaceShips(){
  uint8_t tempShipList[BS_SHIPS_PER_PLAYER] = {2,2,2,3,3,4,5,0};

  // get number of ships
  int8_t trueShipCount = 0;
  for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
    if(tempShipList[i] > 0) trueShipCount++;
  }

  // verticality
  bool placeVertical = false;

  while (trueShipCount > 0) {
    uint8_t currentShipLength = tempShipList[trueShipCount-1];

    // Game loop
    while(true){

      // Get input
      arduboy.pollButtons();

      // Handle ship placement
      if (arduboy.justPressed(B_BUTTON)){
        if (!detectShipCollisionOnMap(cursorPosition.x, cursorPosition.y, currentShipLength, placeVertical)) {
          // place ship
          writeShipToMap(cursorPosition.x, cursorPosition.y, player1Map, currentShipLength, trueShipCount-1, placeVertical);

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
      if (arduboy.pressed(DOWN_BUTTON)){
        cursorPosition.y++;
      }
      if (arduboy.pressed(UP_BUTTON)){
        cursorPosition.y--;
      }
      if (arduboy.pressed(LEFT_BUTTON)){
        cursorPosition.x--;
      }
      if (arduboy.pressed(RIGHT_BUTTON)){
        cursorPosition.x++;
      }
      cursorPosition.x = max(cursorPosition.x, 0);
      cursorPosition.x = min(cursorPosition.x, BS_MAP_SIZE - (placeVertical?1:currentShipLength));
      cursorPosition.y = max(cursorPosition.y, 0);
      cursorPosition.y = min(cursorPosition.y, BS_MAP_SIZE - (placeVertical?currentShipLength:1));

      // wait for next frame with drawing
      if (!arduboy.nextFrame()) continue;

      // Drawing
      arduboy.clear();

      // Map
      //drawMapAtPosition(2, 2, player1Map, true);
      drawMapAtPosition(2, 2, player1Map, true);
      drawShipAtPosition(2 + cursorPosition.x * 6, 2 + cursorPosition.y * 6, currentShipLength, placeVertical);

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
