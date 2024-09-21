#ifndef DEFINES_H
#define DEFINES_H

#define TS 32  // TILE SIZE

#define MAP_HEIGHT 14
#define MAP_WIDTH  40

#define EMPTY_TILE 0
#define METEORITE  1
#define WICHER     2

#define DIR_NONE 0
#define RIGHT    1
#define LEFT     2
#define UP       3
#define DOWN     4

#define STATE_IDLE 0
#define STATE_ANIM 1
#define STATE_HUD  2 

#define FACE_RIGHT 64
#define FACE_LEFT  96

#define WICHER_ANIM_SPEED 4

#define TILE_EMPTY 0

#define HUD_SORDAN 11
#define HUD_RASTPORT 12
#define HUD_RANDOM_ENCOUNTER 13

#define S 'S' // SORDAN TILE
#define s 's' // SORDAN CHECK TILES
#define R 'R' // RASTPORT TILE
#define r 'r' // RASTPORT CHECK TILES

#define ENCOUNTER_REGION_EASY 24  // rand 0-24 chance to encounter

#define CAPACITOR_SELECTION_SORDAN 1
#define RESISTOR_SELECTION_SORDAN 2

#define LASER_SPEAR_UPGRADE_RASTPORT 1
#define SHIELDING_UPGRADE_RASTPORT 2
#define ENERGY_STORAGE_UPGRADE_RASTPORT 3
#define COAL_CONTAINER_UPGRADE_RASTPORT 4

#define SPEAR_LEVELS 4

#endif