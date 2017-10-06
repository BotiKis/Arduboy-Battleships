#include "BSGame.h"
#include "BSIsometricBitmaps.h"
#include "BSMapTileData.h"

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
  gameState = BSGameStateMenu;
  gameMapZoom = 1;
}

// -------------------------------------------------------
// Main game method.
// Handles gamestates and switches to the desired one

void BSGame::run(){

  // Game loop
  while(true){

      switch (gameState) {
        case BSGameStatePlayingSinglePlayer:{
          // reset Game
          resetGame();

          // place ships
          showPlaceShipsForPlayer(&player1);
          createMapForAI();

          // start game
          startNewSinglePlayerGame();

          // end
          this->gameState = BSGameStateMenu;

          break;
        }
        case BSGameStatePlayingMultiPlayer:{
          // reset Game
          resetGame();

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
          startNewMultiPlayerGame();

          // end
          this->gameState = BSGameStateMenu;

          break;
        }
        case BSGameStateOptions:{
          showOKDialog("Options here");
          this->gameState = BSGameStateMenu;
          break;
        }
        case BSGameStateMenu:
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
    arduboy.pollButtons();

    if (arduboy.justPressed(DOWN_BUTTON)){
      cursorIdx++;
    }
    if (arduboy.justPressed(UP_BUTTON)){
      cursorIdx--;
    }
    if (arduboy.justPressed(B_BUTTON)){
      arduboy.initRandomSeed();

      // Single player
      if (cursorIdx == 0)
        return BSGameStatePlayingSinglePlayer;
      if (cursorIdx == 1)
        return BSGameStatePlayingMultiPlayer;
      if (cursorIdx == 2)
        return BSGameStateOptions;

    }
    cursorIdx = cursorIdx%3;

    arduboy.clear();

    tinyfont.setCursor(2, 47);
    tinyfont.print(F("SINGLE PLAYER"));
    tinyfont.setCursor(2, 53);
    tinyfont.print(F("TWO PLAYER"));
    tinyfont.setCursor(2, 59);
    tinyfont.print(F("OPTIONS"));
    tinyfont.setCursor(75, 47 + cursorIdx*6);
    tinyfont.print("<");

    arduboy.display();
  }
}

void BSGame::resetGame(){

  player1.resetPlayer();
  player2.resetPlayer();

  player1.setPlayerName("Player 1");
  player2.setPlayerName("Player 2");

  // reset cursor
  cursorPosition = {0,0};
}

//
void BSGame::drawMapAtPosition(int16_t posX, int16_t posY, BSPlayer *aPlayer, bool drawShips){

  // stores the current maptile
  uint16_t mapTile = 0;
  uint16_t mapTileType = 0;
  Point drawPosition = {0,0};

  // draw map
  for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
    for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {
      mapTile = aPlayer->getMapTileAtPosition(j,i);
      mapTileType = MAP_TILE_TYPE(mapTile);

      drawPosition.x = posX - j*16*gameMapZoom + i*16*gameMapZoom;
      drawPosition.y = posY + j*8*gameMapZoom + i*8*gameMapZoom;

      // draw wireframe
      ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, BitmapWireframeTop, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

      // Check for mountain
      if (mapTileType == MAP_TILE_TYPE_MOUNTAIN){
        ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, BitmapMountainMask, BLACK, ALIGN_H_LEFT, MIRROR_NONE);
        ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, BitmapMountain, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      }

      // Check for Ship
      else if (mapTileType == MAP_TILE_TYPE_SHIP){

        // don't draw
        if (!drawShips) continue;

        bool isVertical = (mapTile & MAP_FLAG_IS_VERTICAL) != 0;

        uint8_t shipLength = MAP_SHIPLENGTH(mapTile);
        uint8_t tileIdx = MAP_SHIPTILE_INDEX(mapTile);

        unsigned const char *shipSprite = NULL;
        unsigned const char *shipMaskSprite = NULL;

        if (tileIdx == 0) {
          // Ship rear
          shipSprite = BitmapShipEnd;
          shipMaskSprite = BitmapShipEndMask;
        }
        else if (tileIdx == shipLength-1) {
          // ship front
          shipSprite = BitmapShipFront;
          shipMaskSprite = BitmapShipFrontMask;
        }
        else{
          // Middle
          shipSprite = BitmapShipMiddle;
          shipMaskSprite = BitmapShipMiddleMask;
        }

        ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, shipMaskSprite, BLACK, ALIGN_H_LEFT, isVertical?MIRROR_NONE:MIRROR_HORIZONTAL);
        ardbitmap.drawCompressed(drawPosition.x, drawPosition.y, shipSprite, WHITE, ALIGN_H_LEFT, isVertical?MIRROR_NONE:MIRROR_HORIZONTAL);

      }
    }
  }
}

void BSGame::animateFromPlayerToPlayer(BSPlayer *aPlayer, BSPlayer *aOpponent, bool animateUp){

}

/// Draws a ship with the given settings
void BSGame::drawShipAtPosition(int16_t posX, int16_t posY, uint8_t length, bool vertical){

  int8_t x, y;

  unsigned const char *shipSprite = NULL;
  unsigned const char *shipMaskSprite = NULL;

  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (i * (vertical?-1:1) * 16 * gameMapZoom);
    y = posY + (i * 8 * gameMapZoom);


    if (i == 0) {
      // Ship rear
      shipSprite = BitmapShipEnd;
      shipMaskSprite = BitmapShipEndMask;
    }
    else if (i == length-1) {
      // ship front
      shipSprite = BitmapShipFront;
      shipMaskSprite = BitmapShipFrontMask;
    }
    else{
      // Middle
      shipSprite = BitmapShipMiddle;
      shipMaskSprite = BitmapShipMiddleMask;
    }

    // // Check if vertical
    ardbitmap.drawCompressed(x, y, shipMaskSprite, BLACK, ALIGN_H_LEFT, vertical?MIRROR_NONE:MIRROR_HORIZONTAL);
    ardbitmap.drawCompressed(x, y, shipSprite, WHITE, ALIGN_H_LEFT, vertical?MIRROR_NONE:MIRROR_HORIZONTAL);
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
