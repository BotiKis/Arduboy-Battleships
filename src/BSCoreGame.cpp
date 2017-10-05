#include "BSGame.h"
#include "BSIsometricBitmaps.h"
#include "BSMapTileData.h"

void BSGame::showPlaceShipsForPlayer(BSPlayer *aPlayer){

  // get number of ships
  int8_t shipCount = 0;
  for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
    if(playerShipList[i] > 0) shipCount++;
  }

  // point where the map will be drawn
  Point cameraPosition = {0,0};
  static const Point mapOrigin = {26, 0};

  // verticality
  bool placeVertical = false;

  // animatiors
  bool animatorCursor = false;

  while (shipCount > 0) {
    uint8_t currentShipLength = playerShipList[shipCount-1];

    // Game loop
    while(true){

      // Get input
      arduboy.pollButtons();

      // Handle ship placement
      if (arduboy.justPressed(B_BUTTON)){
        if (!aPlayer->detectShipCollisionOnMap(cursorPosition.x, cursorPosition.y, currentShipLength, placeVertical)) {
          // place ship
          aPlayer->addShip(cursorPosition.x, cursorPosition.y, currentShipLength, shipCount-1, placeVertical);

          shipCount--;
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
      drawMapAtPosition(cameraPosition.x, cameraPosition.y, aPlayer, true);

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


void BSGame::createMapForAI(){
  player2.resetPlayer();

  // get number of ships
  int8_t shipCount = 0;
  for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
    if(playerShipList[i] > 0) shipCount++;
  }

  // verticality
  bool placeVertical = false;
  Point shipPos = {0,0};

  while (shipCount > 0) {
    // get ship length
    uint8_t currentShipLength = playerShipList[shipCount-1];

    // randomize positions
    shipPos.x = random(BS_MAP_SIZE);
    shipPos.y = random(BS_MAP_SIZE);
    placeVertical = (random()%2) == 0;

    // place ship
    if (!player2.detectShipCollisionOnMap(shipPos.x, shipPos.y, currentShipLength, placeVertical)) {
      // place ship
      player2.addShip(shipPos.x, shipPos.y, currentShipLength, shipCount-1, placeVertical);

      shipCount--;
    }
  }

}

void BSGame::startNewSinglePlayerGame(){
  BSPlayer *attackingPlayer, *passivePlayer;

  // randomize player
  bool randPlayer = (random()%2) == 0;
  attackingPlayer = randPlayer?&player1:&player2;
  passivePlayer = randPlayer?&player2:&player1;

  // frame for the dialog
  static Rect dialogFrame;
  dialogFrame.width = 80;
  dialogFrame.height = 32;
  dialogFrame.x = (WIDTH - dialogFrame.width)/2;
  dialogFrame.y = (HEIGHT - dialogFrame.height)/2;

  static char titleBuffer[32];

  // game loop
  while(true){
    sprintf(titleBuffer, "%s turn!", attackingPlayer->getPlayerName());
    showOKDialog(dialogFrame, titleBuffer);

    showTurnOfPlayer(attackingPlayer, passivePlayer);

    // check for end
    if (passivePlayer->getRemainingShips() == 0) {

    }

    // switch players
    BSPlayer *playerBuff = attackingPlayer;
    attackingPlayer = passivePlayer;
    passivePlayer = playerBuff;
  }
}


void BSGame::showTurnOfPlayer(BSPlayer *aPlayer, BSPlayer *aOpponent){
  // point where the map will be drawn
  Point cameraPosition = {0,0};
  static const Point mapOrigin = {26, 0};

  // animatiors
  bool animatorCursor = false;

  // Game loop
  while(true){

    // wait for next frame with drawing
    if (!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    if (arduboy.justPressed(B_BUTTON)){
      return;
    }
    if (arduboy.justPressed(A_BUTTON)){
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

    // handle animators
    // cursor
    if (arduboy.everyXFrames(3)) animatorCursor = !animatorCursor;

    // Drawing
    arduboy.clear();

    // Map
    // tile height is 32 and width 16, all sprites are 32x32 for simplicity and overdrawing
    cameraPosition.x = mapOrigin.x - (cursorPosition.x - cursorPosition.y)*16*gameMapZoom;
    cameraPosition.y = mapOrigin.y - (cursorPosition.x + cursorPosition.y)*8*gameMapZoom;
    drawMapAtPosition(cameraPosition.x, cameraPosition.y, aOpponent, false);

    // draw cursor
    if(animatorCursor) ardbitmap.drawCompressedResized(mapOrigin.x, mapOrigin.y, BitmapCursorFull, WHITE, ALIGN_H_LEFT, MIRROR_NONE, gameMapZoom);

    // Infobox
    arduboy.fillRect(73,0,55,19,WHITE);
    arduboy.fillRect(74,0,54,18,BLACK);
    tinyfont.setCursor(76,3);
    char sBuff[32] = {'\0'};
    sprintf(sBuff, "%s\nturn", aPlayer->getPlayerName());
    tinyfont.print(sBuff);

    arduboy.display();
  }
}
