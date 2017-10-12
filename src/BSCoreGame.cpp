#include "BSGame.h"
#include "BSIsometricBitmaps.h"
#include "BSMapTileData.h"
#include "BSAnimationHelper.h"


void BSGame::startNewSinglePlayerGame(){
  // reset players
  player1.resetPlayer();
  player2.resetPlayer();

  player1.setPlayerName("Player");
  player2.setPlayerName("Computer");

  player1.setCursorPosition({BS_MAP_SIZE/2,BS_MAP_SIZE/2});

  // place ships
  showPlaceShipsForPlayer(&player1);

  // place AI
  createMapForAI();

  // run game
  runSinglePlayerGame();
}

void BSGame::runSinglePlayerGame(){
  showOKDialog("Singleplayer");
}

void BSGame::startNewMultiPlayerGame(){
  // reset players
  player1.resetPlayer();
  player2.resetPlayer();

  player1.setPlayerName("Player 1");
  player2.setPlayerName("Player 2");

  // set start cursor
  player1.setCursorPosition({BS_MAP_SIZE/2,BS_MAP_SIZE/2});
  player2.setCursorPosition({BS_MAP_SIZE/2,BS_MAP_SIZE/2});

  static char titleBuffer[32];

  // place ships player 1
  sprintf(titleBuffer, "%s place\nyour ships!", player1.getPlayerName());
  showOKDialog(titleBuffer);
  showPlaceShipsForPlayer(&player1);

  // place ships player 2
  sprintf(titleBuffer, "%s place\nyour ships!", player2.getPlayerName());
  showOKDialog(titleBuffer);
  showPlaceShipsForPlayer(&player2);

  // start game
  runMultiPlayerGame();
}

void BSGame::runMultiPlayerGame(){
  BSPlayer *attackingPlayer, *passivePlayer;

  // randomize player
  bool randPlayer = (random()%2) == 0;
  attackingPlayer = randPlayer?&player1:&player2;
  passivePlayer = randPlayer?&player2:&player1;

  static char titleBuffer[32];

  // game loop
  while(true){
    sprintf(titleBuffer, "%s TURN!", attackingPlayer->getPlayerName());
    showOKDialog(titleBuffer);

    showTurnOfPlayer(attackingPlayer, passivePlayer);

    // check for end
    if (passivePlayer->getRemainingShips() <= 0) {
      sprintf(titleBuffer, "%s WON!\n YEAAAH", attackingPlayer->getPlayerName());
      showOKDialog(titleBuffer);
      return;
    }

    animateFromPlayerToPlayer(attackingPlayer, passivePlayer, attackingPlayer == &player1);

    // switch players
    BSPlayer *playerBuff = attackingPlayer;
    attackingPlayer = passivePlayer;
    passivePlayer = playerBuff;
  }
}

void BSGame::showPlaceShipsForPlayer(BSPlayer *aPlayer){
  // get number of ships
  int8_t shipCount = 0;
  for (uint8_t i = 0; i < BS_SHIPS_PER_PLAYER; i++) {
    if(playerShipList[i] > 0) shipCount++;
  }

  // point where the map will be drawn
  Point cameraPosition = {0,0};
  Point newCursorPos = aPlayer->getCursorPosition();

  // verticality
  bool placeVertical = false;

  // animatiors
  bool animatorCursor = false;

  while (shipCount > 0) {
    uint8_t currentShipLength = playerShipList[shipCount-1];

    // Game loop
    while(true){

    // wait for next frame with drawing
    if (!arduboy.nextFrame()) continue;

      // Get input
      arduboy.pollButtons();

      // Handle ship placement
      if (arduboy.justPressed(B_BUTTON)){
        if (!aPlayer->detectShipCollisionOnMap(aPlayer->getCursorPosition().x, aPlayer->getCursorPosition().y, currentShipLength, placeVertical)) {
          // place ship
          aPlayer->addShip(aPlayer->getCursorPosition().x, aPlayer->getCursorPosition().y, currentShipLength, shipCount-1, placeVertical);

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
        newCursorPos.y = aPlayer->getCursorPosition().y+1;
        newCursorPos.y = min(newCursorPos.y, BS_MAP_SIZE-1);
      }
      if (arduboy.justPressed(UP_BUTTON)){
        newCursorPos.y = aPlayer->getCursorPosition().y-1;
        newCursorPos.y = max(newCursorPos.y, 0);
      }
      if (arduboy.justPressed(LEFT_BUTTON)){
        newCursorPos.x = aPlayer->getCursorPosition().x-1;
        newCursorPos.x = max(newCursorPos.x, 0);
      }
      if (arduboy.justPressed(RIGHT_BUTTON)){
        newCursorPos.x = aPlayer->getCursorPosition().x+1;
        newCursorPos.x = min(newCursorPos.x, BS_MAP_SIZE-1);
      }

      // update cursor position
      aPlayer->setCursorPosition(newCursorPos);

      // handle animators
      // cursor
      if (arduboy.everyXFrames(CURSOR_ANIMATION_TIME)) animatorCursor = !animatorCursor;

      // Drawing
      arduboy.clear();

      // Map
      // tile height is 32 and width 16, all sprites are 32x32 for simplicity and overdrawing
      cameraPosition.x = mapOrigin.x - (aPlayer->getCursorPosition().x - aPlayer->getCursorPosition().y)*16;
      cameraPosition.y = mapOrigin.y - (aPlayer->getCursorPosition().x + aPlayer->getCursorPosition().y)*8;
      drawMapAtPosition(cameraPosition.x, cameraPosition.y, aPlayer, true);

      // draw cursor
      if(animatorCursor) arduboy.drawBitmap(mapOrigin.x, mapOrigin.y+16, BitmapCursorDotted32x16, 32, 16, WHITE);

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

void BSGame::showTurnOfPlayer(BSPlayer *aPlayer, BSPlayer *aOpponent){
  // point where the map will be drawn
  Point cameraPosition = {0,0};
  Point newCursorPos = aPlayer->getCursorPosition();

  // animatiors
  bool animatorCursor = false;

  // Game loop
  while(true){

    // wait for next frame with drawing
    if (!arduboy.nextFrame()) continue;

    // Get input
    arduboy.pollButtons();

    // show dialog to fire
    if (arduboy.justPressed(B_BUTTON)){
      if(showAimMenuOnPlayersMap(mapOrigin, aPlayer->getCursorPosition(), aOpponent) == BSGameStatePlayingNextTurn) return;
    }

    // Move cursor
    if (arduboy.justPressed(DOWN_BUTTON)){
      newCursorPos.y = aPlayer->getCursorPosition().y+1;
      newCursorPos.y = min(newCursorPos.y, BS_MAP_SIZE-1);
    }
    if (arduboy.justPressed(UP_BUTTON)){
      newCursorPos.y = aPlayer->getCursorPosition().y-1;
      newCursorPos.y = max(newCursorPos.y, 0);
    }
    if (arduboy.justPressed(LEFT_BUTTON)){
      newCursorPos.x = aPlayer->getCursorPosition().x-1;
      newCursorPos.x = max(newCursorPos.x, 0);
    }
    if (arduboy.justPressed(RIGHT_BUTTON)){
      newCursorPos.x = aPlayer->getCursorPosition().x+1;
      newCursorPos.x = min(newCursorPos.x, BS_MAP_SIZE-1);
    }

    // update cursor position
    aPlayer->setCursorPosition(newCursorPos);

    // handle animators
    // cursor
    if (arduboy.everyXFrames(CURSOR_ANIMATION_TIME)) animatorCursor = !animatorCursor;

    // Drawing
    arduboy.clear();

    // Map
    // tile height is 32 and width 16
    cameraPosition.x = mapOrigin.x - (aPlayer->getCursorPosition().x - aPlayer->getCursorPosition().y)*16;
    cameraPosition.y = mapOrigin.y - (aPlayer->getCursorPosition().x + aPlayer->getCursorPosition().y)*8;


    drawMapAtPosition(cameraPosition.x, cameraPosition.y, aOpponent, false);

    if(animatorCursor) arduboy.drawBitmap(mapOrigin.x, mapOrigin.y+16, BitmapCursorDotted32x16, 32, 16, WHITE);

    // Infobox
    arduboy.fillRect(73,0,55,19,WHITE);
    arduboy.fillRect(74,0,54,18,BLACK);

    // Print palyername
    tinyfont.setCursor(76,3);
    tinyfont.print(aPlayer->getPlayerName());

    tinyfont.setCursor(76,10);
    tinyfont.print(F("AIM!"));

    arduboy.display();
  }
}


BSGameState BSGame::showAimMenuOnPlayersMap(Point mapOrigin, Point cursorPos, BSPlayer *aPlayer){
  //handle submenu
  uint8_t menuCursorIdx = 0;

  // calc pos
  Point subMenuPosition;
  subMenuPosition.x = mapOrigin.x+32;
  subMenuPosition.y = mapOrigin.y+17;

  Point cameraPosition;
  cameraPosition.x = mapOrigin.x - (cursorPos.x - cursorPos.y)*16;
  cameraPosition.y = mapOrigin.y - (cursorPos.x + cursorPos.y)*8;

  // animators
  bool animatorCursor = false;

  // menu loop
  while(true){

    // wait for next frame with drawing
    if (!arduboy.nextFrame()) continue;

    // handle animators
    // cursor
    if (arduboy.everyXFrames(CURSOR_ANIMATION_TIME)) animatorCursor = !animatorCursor;

    // Get input
    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      menuCursorIdx++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      menuCursorIdx--;
    }
    menuCursorIdx = menuCursorIdx%2;

    if (arduboy.justPressed(B_BUTTON)){

      // Handle Fire
      if (menuCursorIdx == 0){

        // draw rocket and explosion
        drawExplosionAnimation(mapOrigin, cursorPos, aPlayer);
        char dialogText[16] = {'\0'};

        // check for shipSprite
        if (aPlayer->isShipTileAtPosition(cursorPos.x, cursorPos.y)) {
          aPlayer->destroyTileAtPosition(cursorPos.x, cursorPos.y);
          strcpy(dialogText, "HIT!");
        }
        else{
          strcpy(dialogText, "HAHA MISS!");
        }

        arduboy.clear();
        drawMapAtPosition(cameraPosition.x, cameraPosition.y, aPlayer, false);
        arduboy.display();

        delay(600);
        showOKDialog(dialogText);

        return BSGameStatePlayingNextTurn;
      }
      // cancel sub menu
      if (menuCursorIdx == 1)
        return BSGameStatePlayingCancelAction;
    }
    // cancel on A
    if (arduboy.justPressed(A_BUTTON))
      return BSGameStatePlayingCancelAction;

    arduboy.clear();

    // draw map
    drawMapAtPosition(cameraPosition.x, cameraPosition.y, aPlayer, false);

    // draw cursor
    if(animatorCursor) arduboy.drawBitmap(mapOrigin.x, mapOrigin.y+16, BitmapCursorDotted32x16, 32, 16, WHITE);

    // show context menu
    arduboy.fillRect(subMenuPosition.x, subMenuPosition.y, 38, 15, BLACK);
    arduboy.drawRect(subMenuPosition.x+1, subMenuPosition.y+1, 36, 13, WHITE);

    // draw text
    tinyfont.setCursor(subMenuPosition.x+3, subMenuPosition.y+3);
    tinyfont.print(F("FIRE!"));
    tinyfont.setCursor(subMenuPosition.x+3, subMenuPosition.y+8);
    tinyfont.print(F("cancel"));

    // draw cursor
    tinyfont.setCursor(subMenuPosition.x+32, subMenuPosition.y + 3 + 5*menuCursorIdx);
    tinyfont.print("<");

    arduboy.display();
  }
}


void BSGame::animateFromPlayerToPlayer(BSPlayer *startPlayer, BSPlayer *endPlayer, bool animateUp){

  Point startPositionActivePlayer;
  Point endPositionActivePlayer;
  startPositionActivePlayer.x = mapOrigin.x - (startPlayer->getCursorPosition().x - startPlayer->getCursorPosition().y)*16;
  startPositionActivePlayer.y = mapOrigin.y - (startPlayer->getCursorPosition().x + startPlayer->getCursorPosition().y)*8;

  Point endPositionPassivePlayer;
  Point startPositionPassivePlayer;
  endPositionPassivePlayer.x = mapOrigin.x - (endPlayer->getCursorPosition().x - endPlayer->getCursorPosition().y)*16;
  endPositionPassivePlayer.y = mapOrigin.y - (endPlayer->getCursorPosition().x + endPlayer->getCursorPosition().y)*8;

  startPositionPassivePlayer.x = startPositionActivePlayer.x + BS_MAP_SIZE*16*(animateUp?1:-1);
  startPositionPassivePlayer.y = startPositionActivePlayer.y + BS_MAP_SIZE*8*(animateUp?-1:1);

  endPositionActivePlayer.x = endPositionPassivePlayer.x + BS_MAP_SIZE*16*(animateUp?-1:1);
  endPositionActivePlayer.y = endPositionPassivePlayer.y + BS_MAP_SIZE*8*(animateUp?1:-1);

  Point mapPositionStartPlayer;
  Point mapPositionEndPlayer;

  Point deltaVector;
  deltaVector.x = startPositionActivePlayer.x - startPositionPassivePlayer.x;
  deltaVector.y = startPositionActivePlayer.y - startPositionPassivePlayer.y;
  uint16_t deltaLen = sqrt(deltaVector.x*deltaVector.x + deltaVector.y*deltaVector.y);

  // set up animation
  uint64_t deltaTime = 0;
  const uint16_t animationDuration = 666 + ((deltaLen*200)/32);
  uint64_t animationStart = millis();
  uint8_t progress = 0;

  while (true) {

      // wait for next frame with drawing
      if (!arduboy.nextFrame()) continue;

      // get deltatime
      deltaTime = MILLIS_SINCE(animationStart);
      progress = min(deltaTime*100/animationDuration, 100);

      // get new position
      mapPositionStartPlayer = animatePointFromToPoint(startPositionActivePlayer, endPositionActivePlayer, progress);
      mapPositionEndPlayer = animatePointFromToPoint(startPositionPassivePlayer, endPositionPassivePlayer, progress);

      arduboy.clear();

      drawMapAtPosition(mapPositionStartPlayer.x, mapPositionStartPlayer.y, endPlayer, false);
      drawMapAtPosition(mapPositionEndPlayer.x, mapPositionEndPlayer.y, startPlayer, false);

      arduboy.display();

      // exit if animation has ended
      if (deltaTime >= animationDuration) break;
  }

  arduboy.clear();
  drawMapAtPosition(endPositionPassivePlayer.x, endPositionPassivePlayer.y, startPlayer, false);
  arduboy.display();
}
