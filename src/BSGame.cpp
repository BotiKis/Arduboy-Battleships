#include "BSGame.h"

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
  gameState = BSGameStateMenu;
}

// -------------------------------------------------------
// Main game method.
// Handles gamestates and switches to the desired one

void BSGame::run(){

  // Game loop
  while(true){

      switch (gameState) {
        case BSGameStatePlaying:{
          startNewGame();
          break;
        }
        case BSGameStateMenu:
        default:
          showMenu();
      }
  }

}

// ======================


void BSGame::showMenu(){

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
      arduboy.clear();
      arduboy.display();
      this->gameState = BSGameStatePlaying;
      arduboy.initRandomSeed();
      return;
    }
    cursorIdx = cursorIdx%2;

    arduboy.clear();

    tinyfont.setCursor(2, 2);
    tinyfont.print(F("ONE PLAYER"));
    tinyfont.setCursor(2, 8);
    tinyfont.print(F("OPTIONS"));
    tinyfont.setCursor(52, 2 + cursorIdx*6);
    tinyfont.print("<");

    arduboy.display();
  }
}


void BSGame::startNewGame(){

  // reset Game
  resetGame();

  // place ships
  showPlaceShips();
}

bool BSGame::detectShipCollisionOnMap(uint8_t posX, uint8_t posY, uint8_t length, bool vertical){

  // check for out of bounds
  if (posX < 0 ||
      posY < 0 ||
      (posX + (vertical?0:length-1)) >= BS_MAP_SIZE ||
      (posY + (vertical?length-1:0)) >= BS_MAP_SIZE) return true;

  // check for mountains or other ships on the map
  uint8_t x, y;
  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (vertical?0:i);
    y = posY + (vertical?i:0);

    // check for mountain
    if(MAP_TILE_TYPE(player1Map[x][y]) == MAP_TILE_TYPE_MOUNTAIN) return true;
    if(MAP_TILE_TYPE(player1Map[x][y]) == MAP_TILE_TYPE_SHIP) return true;
  }
  return false;
}

//
void BSGame::drawMapAtPosition(uint16_t posX, uint16_t posY, uint16_t playerMap[BS_MAP_SIZE][BS_MAP_SIZE], bool drawShips){
  // draw map
  for (uint8_t i = 0; i < BS_MAP_SIZE; i++) {
    for (uint8_t j = 0; j < BS_MAP_SIZE; j++) {

      tinyfont.setCursor(posX + i*6, posY + j*6);


      // Check for mountain
      if ((MAP_TILE_TYPE(playerMap[i][j]) == MAP_TILE_TYPE_MOUNTAIN) != 0)
        tinyfont.print("X");

      // Check for Ship
      else if ( MAP_TILE_TYPE(playerMap[i][j]) == MAP_TILE_TYPE_SHIP){

        // don't draw
        if (!drawShips) continue;

        // Check if vertical
        if ( (playerMap[i][j] & MAP_FLAG_IS_VERTICAL) != 0)
          tinyfont.print("v");
        else
          tinyfont.print(">");
      }
      else
        tinyfont.print(".");
    }
  }
}


/// Draws a ship with the given settings
void BSGame::drawShipAtPosition(uint16_t posX, uint16_t posY, uint8_t length, bool vertical){

  uint8_t x, y;
  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (vertical?0:i*6);
    y = posY + (vertical?i*6:0);

    tinyfont.setCursor(x, y);

    // Check if vertical
    if (vertical)
      tinyfont.print("v");
    else
      tinyfont.print(">");
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

void BSGame::writeShipToMap(uint8_t posX, uint8_t posY, uint16_t playerMap[BS_MAP_SIZE][BS_MAP_SIZE], uint8_t length, uint8_t shipIndex , bool vertical){
  uint8_t x, y;
  for (uint8_t i = 0; i < length; i++) {

    // add index offset to coords
    x = posX + (vertical?0:i);
    y = posY + (vertical?i:0);


    // Create tile data
    uint16_t tileData = 0;

    // tile type
    tileData |= (MAP_TILE_TYPE_SHIP << MAP_TILE_TYPE_POS);

    // Ships index
    tileData |= ( (shipIndex & 0b111) << MAP_SHIP_INDEX_BIT_POS);

    // ship length
    tileData |= ( (length & 0b111) << MAP_SHIPLENGTH_BIT_POS);

    // tile index
    tileData |= ( (i & 0b111) << MAP_SHIPTILE_INDEX_BIT_POS);

    // vertical
    if (vertical) tileData |= MAP_FLAG_IS_VERTICAL;

    // write to map
    player1Map[x][y] = tileData;


    printMapTileBinary(45, 59, tileData);
    arduboy.display();
    delay(1000);
  }
}
