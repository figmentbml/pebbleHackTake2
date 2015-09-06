#pragma once
  
typedef enum {
  UNKNOWN,
  URBAN,
  TREES,
  WATER,
  OTHER
} TerrainType;

TerrainType get_terrain(int32_t lat, int32_t lon);