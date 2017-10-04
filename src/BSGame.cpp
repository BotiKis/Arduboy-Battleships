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
      ardbitmap.drawCompressedResized(drawPosition.x, drawPosition.y, BitmapWireframeTop, WHITE, ALIGN_H_LEFT, MIRROR_NONE, gameMapZoom);

      // Check for mountain
      if (mapTileType == MAP_TILE_TYPE_MOUNTAIN){
        ardbitmap.drawCompressedResized(drawPosition.x, drawPosition.y, BitmapMountainMask, BLACK, ALIGN_H_LEFT, MIRROR_NONE, gameMapZoom);
        ardbitmap.drawCompressedResized(drawPosition.x, drawPosition.y, BitmapMountain, WHITE, ALIGN_H_LEFT, MIRROR_NONE, gameMapZoom);
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

        ardbitmap.drawCompressedResized(drawPosition.x, drawPosition.y, shipMaskSprite, BLACK, ALIGN_H_LEFT, isVertical?MIRROR_NONE:MIRROR_HORIZONTAL, gameMapZoom);
        ardbitmap.drawCompressedResized(drawPosition.x, drawPosition.y, shipSprite, WHITE, ALIGN_H_LEFT, isVertical?MIRROR_NONE:MIRROR_HORIZONTAL, gameMapZoom);

      }
    }
  }
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
    ardbitmap.drawCompressedResized(x, y, shipMaskSprite, BLACK, ALIGN_H_LEFT, vertical?MIRROR_NONE:MIRROR_HORIZONTAL, gameMapZoom);
    ardbitmap.drawCompressedResized(x, y, shipSprite, WHITE, ALIGN_H_LEFT, vertical?MIRROR_NONE:MIRROR_HORIZONTAL, gameMapZoom);
  }
}

void BSGame::printMapTileBinary(uint8_t posX, uint8_t posY, uint16_t val){
  char buff[20] = {'\0'};
  for (uint8_t i = 0; i < 16; i++) {
    buff[i] = ((val >> (15-i))&0b1) ? '1':'0';
  }

  tinyfont.setCursor(posX, posY);
  tinyfont.print(buff);
}


        // char buff[32] = {'\0'};
        // sprintf(buff, "sIdx:\t%d\nsL  :\t%d\ntIdx:\t%d", shipIdx, shipLength, tileIdx);
        // static Point infoOrigin = {40, 0};
        //
        // while(1){
        //   arduboy.pollButtons();
        //
        //   if (arduboy.justPressed(B_BUTTON)) break;
        //   arduboy.clear();
        //
        //   // Infobox
        //   arduboy.fillRect(infoOrigin.x, infoOrigin.y, WIDTH-infoOrigin.x, 24,WHITE);
        //   arduboy.fillRect(infoOrigin.x+1, infoOrigin.y+1, WIDTH-infoOrigin.x-1, 22,BLACK);
        //   tinyfont.setCursor(infoOrigin.x+3, infoOrigin.y+3);
        //   tinyfont.print(buff);
        //
        //   printMapTileBinary(infoOrigin.x+3, infoOrigin.y+19, mapTile);
        //
        //   arduboy.display();
        // }
