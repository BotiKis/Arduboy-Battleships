#include "BSGame.h"
#include "BSIsometricBitmaps.h"
#include "BSMapTileData.h"
#include "BSAnimationHelper.h"
#include "BSMenuAssets.h"

// Shipnames
const char* shipNameForLength(uint8_t length){
  switch (length) {
    case 5: return "Carrier";
    case 4: return "Battleship";
    case 3: return "Cruiser";
    case 2: return "Destroyer";
    default: return NULL;
  }
}

// -------------------------------------------------------
// Constructor sets up basic stuff for the game

BSGame::BSGame(){
  arduboy.boot();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  arduboy.audio.on();
  arduboy.invert(true);
  gameState = BSGameState::Menu;
}

// -------------------------------------------------------
// Main game method.
// Handles gamestates and switches to the desired one

void BSGame::run(){

  // Game loop
  while(true){

      switch (gameState) {
        case BSGameState::SinglePlayer:{
          // start game
          startNewSinglePlayerGame();
          this->gameState = BSGameState::Menu;
          break;
        }
        case BSGameState::MultiPlayer:{
          startNewMultiPlayerGame();
          this->gameState = BSGameState::Menu;
          break;
        }
        case BSGameState::Options:{
          showOKDialog("Options here");
          this->gameState = BSGameState::Menu;
          break;
        }
        case BSGameState::Menu:
        default:
          this->gameState = showMenu();
      }
  }

}

// ======================


BSGameState BSGame::showMenu(){

  uint8_t cursorIdx = 0;

  // Game loop
  while(true){

    if (!arduboy.nextFrame()) continue;

    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      cursorIdx++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      if (cursorIdx == 0)
        cursorIdx = 2;
      else
        cursorIdx--;
    }
    if (arduboy.justPressed(B_BUTTON)){
      arduboy.initRandomSeed();

      // Single player
      if (cursorIdx == 0)
        return BSGameState::SinglePlayer;
      if (cursorIdx == 1)
        return BSGameState::MultiPlayer;
      if (cursorIdx == 2)
        return BSGameState::Options;

    }
    cursorIdx = cursorIdx%3;

    arduboy.clear();
    float shipAnimTime = millis()/12000.0;
    int shipAnimValue = sin( shipAnimTime * 2.0 * PI )*4;

    float wavesBackAnimTime = millis()/24000.0;
    int wavesBackAnimValue = sin( wavesBackAnimTime * 2.0 * PI )*8+8;

    float wavesFrontAnimTime = millis()/14000.0;
    int wavesFrontAnimValue = sin( wavesFrontAnimTime * 2.0 * PI + PI/2)*8+8;

    // draw background
    ardbitmap.drawCompressed(0+wavesBackAnimValue, 42, menuWavesBack128x16, WHITE);
    ardbitmap.drawCompressed(1, 3+shipAnimValue, menuShip76x64, BLACK);
    ardbitmap.drawCompressed(0, 2+shipAnimValue, menuShip76x64, WHITE);
    ardbitmap.drawCompressed(0+wavesFrontAnimValue-64, 47, menuWavesFront128x16, BLACK);
    ardbitmap.drawCompressed(0+wavesFrontAnimValue+64, 47, menuWavesFront128x16, BLACK);
    ardbitmap.drawCompressed(0+wavesFrontAnimValue-64, 48, menuWavesFront128x16, WHITE);
    ardbitmap.drawCompressed(0+wavesFrontAnimValue+64, 48, menuWavesFront128x16, WHITE);

    // draw title
    ardbitmap.drawCompressed(54, 4, menuTitle72x8, WHITE);

    // draw menu style
    int8_t menuY = 16;
    ardbitmap.drawCompressed(72, menuY, menuBoxMask56x8, BLACK);
    ardbitmap.drawCompressed(72, menuY+10, menuBoxMask56x8, BLACK);
    ardbitmap.drawCompressed(72, menuY+20, menuBoxMask56x8, BLACK);
    ardbitmap.drawCompressed(72, menuY, menuBoxBorder56x8, WHITE);
    ardbitmap.drawCompressed(72, menuY+10, menuBoxBorder56x8, WHITE);
    ardbitmap.drawCompressed(72, menuY+20, menuBoxBorder56x8, WHITE);

    // draw menu
    tinyfont.setCursor(75, menuY+2);
    tinyfont.print(F("ONE PLAYER"));
    tinyfont.setCursor(75, menuY+12);
    tinyfont.print(F("TWO PLAYER"));
    tinyfont.setCursor(75, menuY+22);
    tinyfont.print(F("OPTIONS"));

    // draw cursor
    if (millis()/1000%2)
      arduboy.drawBitmap(126, menuY + cursorIdx*10, menuCursor2x8, 2, 8, WHITE);

    arduboy.display();
  }
}
//
void BSGame::drawMapAtPosition(int16_t posX, int16_t posY, BSPlayer *aPlayer, bool drawShips){

  // stores the cu brrent maptile
  uint16_t mapTile = 0;
  uint16_t mapTileType = 0;
  Point drawPosition = {0,0};

  uint64_t currentTime = millis();
  uint8_t shipDebriesAnimationFrame = 0;

  // draw map
  for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
    for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {
      mapTile = aPlayer->getMapTileAtPosition(j,i);
      mapTileType = MAP_TILE_TYPE(mapTile);

      drawPosition.x = posX - j*16 + i*16;
      drawPosition.y = posY + j*8 + i*8;

      // check off screen
      if(drawPosition.x <= -32 || drawPosition.x > WIDTH || drawPosition.y <= -32 || drawPosition.y > HEIGHT) continue;

      // draw wireframe
      arduboy.drawBitmap(drawPosition.x, drawPosition.y+16, BitmapWireframeTop32x16, 32, 16, WHITE);

      // Check for mountain
      if (mapTileType == MAP_TILE_TYPE_MOUNTAIN){
        ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, BitmapMountainMask32x32, BLACK);
        arduboy.drawBitmap(drawPosition.x, drawPosition.y, BitmapMountain32x32, 32, 32, WHITE);
      }

      // Check for water
      else if (mapTileType == MAP_TILE_TYPE_WATER){

        uint8_t animationStep = ((currentTime/600)+i+j)%4;

        switch (animationStep) {
          case 0: {
            ardbitmap.drawCompressed(drawPosition.x, drawPosition.y+16, BitmapWater132x16, WHITE);
            break;
          }
          case 1: {
            ardbitmap.drawCompressed(drawPosition.x, drawPosition.y+16, BitmapWater232x16, WHITE);
            break;
          }
          case 2: {
            ardbitmap.drawCompressed(drawPosition.x, drawPosition.y+16, BitmapWater332x16, WHITE);
            break;
          }
          case 3:
          default: {
            // reset maptile
            aPlayer->setMapTileAtPosition(j, i, 0x00);
            break;
          }
        }

      }

      // check if empty and maybe make water
      else if (mapTileType == MAP_TILE_TYPE_EMPTY){
          uint8_t waterProbability = random(100);
          if (waterProbability < 1) {
            aPlayer->setMapTileAtPosition(j, i, MAP_TILE_TYPE_WATER);
          }
      }

      // draw missed water
      else if (mapTileType == MAP_TILE_TYPE_MISS) {
        arduboy.drawBitmap(drawPosition.x, drawPosition.y+16, BitmapCursorDotted32x16, 32, 16, WHITE);
      }

      // Check for Ship
      else if (mapTileType == MAP_TILE_TYPE_SHIP){

        // check if it's destroyed
        if ((mapTile & MAP_FLAG_IS_DESTROYED) != 0) {
          shipDebriesAnimationFrame = ((currentTime/1000)+i+j)%2;
          ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, (shipDebriesAnimationFrame == 0)?BitmapShipDebries132x32:BitmapShipDebries232x32, WHITE);
          continue;
        }

        // don't draw
        if (!drawShips) continue;

        bool isVertical = (mapTile & MAP_FLAG_IS_VERTICAL) != 0;
        uint8_t spriteOffset = 0;

        uint8_t shipLength = MAP_SHIPLENGTH(mapTile);
        uint8_t tileIdx = MAP_SHIPTILE_INDEX(mapTile);

        unsigned const char *shipSprite = NULL;
        unsigned const char *shipMaskSprite = NULL;
        if (tileIdx == 0) {
          // Ship rear
          shipSprite = BitmapShipEnd16x32;
          shipMaskSprite = BitmapShipEndMask16x32;
          if (isVertical) spriteOffset = 16;
        }
        else if (tileIdx == shipLength-1) {
          // ship front
          shipSprite = BitmapShipFront32x32;
          shipMaskSprite = BitmapShipFrontMask32x32;
        }
        else{
          // Middle
          shipSprite = BitmapShipMiddle16x32;
          shipMaskSprite = BitmapShipMiddleMask16x32;
          if (isVertical) spriteOffset = 16;
        }

        ardbitmap.drawCompressed(drawPosition.x+spriteOffset, drawPosition.y, shipMaskSprite, BLACK, ALIGN_H_LEFT, isVertical?MIRROR_NONE:MIRROR_HORIZONTAL);
        ardbitmap.drawCompressed(drawPosition.x+spriteOffset, drawPosition.y, shipSprite, WHITE, ALIGN_H_LEFT, isVertical?MIRROR_NONE:MIRROR_HORIZONTAL);

      }
    }
  }
}

void BSGame::drawExplosionAnimation(Point mapOrigin, Point cursorPos, BSPlayer *aPlayer){

  // Calc draw position
  Point cameraPosition;
  cameraPosition.x = mapOrigin.x - (cursorPos.x - cursorPos.y)*16;
  cameraPosition.y = mapOrigin.y - (cursorPos.x + cursorPos.y)*8;

  Point rocketStartPosition = {mapOrigin.x+24, -20};
  Point rocketEndPosition = {mapOrigin.x+15, mapOrigin.y+8};

  Point explosionPosition = {mapOrigin.x-8, mapOrigin.y-18};

  // set up animation
  uint64_t deltaTime = 0;
  const uint16_t rocketFlyTime = 300;
  const uint16_t explosionPhase1 = 200;
  const uint16_t explosionPhase2 = 200;
  const uint16_t explosionPhase3 = 300;
  const uint16_t explosionPhase4 = 400;
  const uint16_t animationDurationInMillis = rocketFlyTime + explosionPhase1 + explosionPhase2 + explosionPhase3 + explosionPhase4;
  uint64_t animationStart = millis();

  while(true){
    // wait for next frame with drawing
    if (!arduboy.nextFrame()) continue;

    // get deltatime
    deltaTime = MILLIS_SINCE(animationStart);

    // exit if animation has ended
    if (deltaTime > animationDurationInMillis) return;

    arduboy.clear();
    drawMapAtPosition(cameraPosition.x, cameraPosition.y, aPlayer, false);

    // calc rocket
    // only visible during the anim time
    if (deltaTime <= rocketFlyTime) {
      Point currentPos = animatePointFromToPoint(rocketStartPosition, rocketEndPosition, deltaTime*100/rocketFlyTime);
      arduboy.drawBitmap(currentPos.x, currentPos.y, BitmapBombMask7x16, 7, 16, BLACK);
      arduboy.drawBitmap(currentPos.x, currentPos.y, BitmapBomb7x16, 7, 16, WHITE);
    }

    // draw explosions
    if (deltaTime > rocketFlyTime && deltaTime <= (rocketFlyTime+explosionPhase1) ) {
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion1Mask48x48, BLACK);
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion148x48, WHITE);
    }
    else if (deltaTime > (rocketFlyTime+explosionPhase1) && deltaTime <= (rocketFlyTime+explosionPhase1+explosionPhase2) ) {
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion2Mask48x48, BLACK);
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion248x48, WHITE);
    }
    else if (deltaTime > (rocketFlyTime+explosionPhase1+explosionPhase2) && deltaTime <= (rocketFlyTime+explosionPhase1+explosionPhase2+explosionPhase3) ) {
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion3Mask48x48, BLACK);
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion348x48, WHITE);
    }
    else if (deltaTime > (rocketFlyTime+explosionPhase1+explosionPhase2+explosionPhase3) && deltaTime <= (rocketFlyTime+explosionPhase1+explosionPhase2+explosionPhase3+explosionPhase4) ) {
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion4Mask48x48, BLACK);
      ardbitmap.drawCompressed(explosionPosition.x, explosionPosition.y, BitmapExplosion448x48, WHITE);
    }



    arduboy.display();
  }
}

/// Draws a ship with the given settings
void BSGame::drawShipAtPosition(int16_t posX, int16_t posY, uint8_t length, bool vertical){

  int8_t x, y;

  unsigned const char *shipSprite = NULL;
  unsigned const char *shipMaskSprite = NULL;

  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + i * (vertical?-1:1) * 16;
    y = posY + i * 8;

    uint8_t spriteOffset = 0;

    if (i == 0) {
      // Ship rear
      shipSprite = BitmapShipEnd16x32;
      shipMaskSprite = BitmapShipEndMask16x32;
      if (vertical) spriteOffset = 16;
    }
    else if (i == length-1) {
      // ship front
      shipSprite = BitmapShipFront32x32;
      shipMaskSprite = BitmapShipFrontMask32x32;
    }
    else{
      // Middle
      shipSprite = BitmapShipMiddle16x32;
      shipMaskSprite = BitmapShipMiddleMask16x32;
      if (vertical) spriteOffset = 16;
    }

    // // Check if vertical
    ardbitmap.drawCompressed(x+spriteOffset, y, shipMaskSprite, BLACK, ALIGN_H_LEFT, vertical?MIRROR_NONE:MIRROR_HORIZONTAL);
    ardbitmap.drawCompressed(x+spriteOffset, y, shipSprite, WHITE, ALIGN_H_LEFT, vertical?MIRROR_NONE:MIRROR_HORIZONTAL);
  }
}

void BSGame::showOKDialog(const char *dialogTitle){

  // frame for the dialog
  static Rect frame;

  frame.width = 82;
  frame.height = 32;
  frame.x = 23;
  frame.y = 16;

  // OK button frame
  Rect okButtonFrame;
  okButtonFrame.width = 16;
  okButtonFrame.height = 8;
  okButtonFrame.x = 56;
  okButtonFrame.y = 35;

  Point okTextPos;
  okTextPos.x = 60;
  okTextPos.y = 37;

  Point dialogTitlePos;
  // check for linebreak
  char *lb = strchr(dialogTitle,'\n');
  if (lb != NULL)
    dialogTitlePos.x = frame.x + frame.width/2 - ((lb-dialogTitle)*5/2);
  // No line break
  else
    dialogTitlePos.x = frame.x + frame.width/2 - (strlen(dialogTitle)*5/2);
  dialogTitlePos.y = 22;

  uint8_t animator = 0;

  // dialog loop
  while (true) {
    // Get input
    arduboy.pollButtons();

    // Exit on button press
    if (arduboy.justPressed(B_BUTTON)){
      return;
    }

    // wait for next frame with drawing
    if (!arduboy.nextFrame()) continue;
    if (arduboy.everyXFrames(5)) animator = (animator+1)%2;

    // Drawing
    // Infobox
    arduboy.fillRoundRect(frame.x, frame.y, frame.width, frame.height, 5, BLACK);
    arduboy.drawRoundRect(frame.x + 1, frame.y + 1, frame.width-2, frame.height-2, 5, WHITE);

    // OK Button
    arduboy.drawRoundRect(okButtonFrame.x - 1*animator, okButtonFrame.y - 1*animator, okButtonFrame.width + 2*animator, okButtonFrame.height + 2*animator, 2+animator, WHITE);
    tinyfont.setCursor(okTextPos.x , okTextPos.y);
    tinyfont.print(F("OK"));

    tinyfont.setCursor(dialogTitlePos.x , dialogTitlePos.y);
    tinyfont.print(dialogTitle);

    arduboy.display();
  }
}
