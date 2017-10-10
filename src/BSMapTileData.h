#ifndef BSMAPTILEDATA_H
#define BSMAPTILEDATA_H

// Size of map
#define BS_MAP_SIZE 10
#define BS_SHIPS_PER_PLAYER 8

// -------------------------------------------------------
// MAP Flags
// The full player map is stored in a 2d 16bit array.
// These flags define the tile at a certain coordinate.
// The coordinate can hold a TILE like water or a mountain, or a shippart.

#define MAP_TILE_TYPE_POS       0

// 4 bits tell the type of TILE
#define MAP_TILE_TYPE(V) ((V >> MAP_TILE_TYPE_POS) & 0b111)

#define MAP_TILE_TYPE_EMPTY     0
#define MAP_TILE_TYPE_SHIP      1
#define MAP_TILE_TYPE_MOUNTAIN  2
#define MAP_TILE_TYPE_WATER     3

////////////////////////////////
// Ship specific information
#define MAP_SHIP_INDEX_BIT_POS      3
#define MAP_SHIPLENGTH_BIT_POS      6
#define MAP_SHIPTILE_INDEX_BIT_POS  9
#define MAP_SHIPTILE_DESTROYED_POS  12
#define MAP_SHIPTILE_ISVERTICAL_POS 13

#define MAP_FLAG_IS_DESTROYED    _BV(12)  // Set if the shippart is destroyed
#define MAP_FLAG_IS_VERTICAL     _BV(13)  // Set if the shipTile (thus the full ship) is vertically aligned

// 3 bits tell the index/number of the ship
#define MAP_SHIP_INDEX(V) ((V >> MAP_SHIP_INDEX_BIT_POS) & 0b111)

// 3 bits tell the index/number of the shippart inside the ship
#define MAP_SHIPTILE_INDEX(V) ((V >> MAP_SHIPTILE_INDEX_BIT_POS) & 0b111)

// 3 bits tell the length/part count of the ship
#define MAP_SHIPLENGTH(V) ((V >> MAP_SHIPLENGTH_BIT_POS) & 0b111)

#endif
