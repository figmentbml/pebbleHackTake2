#include <pebble.h>
#include <get_terrain.h>
#include <data.h>
#include <math.h>
  
#define LAT_START   39950
#define LAT_END     40190
#define LON_START   105035
#define LON_END     105350

Window *my_window;
TextLayer *text_layer;

TerrainType get_terrain( int32_t lat, int32_t lon ) {
  
  uint8_t type;
  
  // Round lat and lon
  lat = (int32_t)round(lat / 5.0) * 5;
  lon = (int32_t)round(lon / 5.0) * 5;
  lon = (int32_t)abs(lon);
  // Check for out of bounds
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: %ld , %ld", lat, lon );

 if ( (lat < LAT_START) || (lat > LAT_END) ||
      (lon < LON_START) || (lon > LON_END) ) {

    return UNKNOWN;
 }
  
  // Offset lat and lon to become array lookup
  lat = (lat - LAT_START) / 5;
  lon = (lon - LON_START) / 5;
  
  // Lookup terrain type
  type = gps_lookup[lon][lat];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cheese: %ld , %ld, type: %i", (long)lat, (long)lon, type );

  
  // Return terrain type based on type number
  if ( type < 50 ) {
    return OTHER;
  } else if ( (type >= 50) && (type < 110) ) {
    return TREES;
  } else if ( (type >= 110) && (type < 190) ) {
    return OTHER;
  } else if ( ( type >= 190) && (type < 200) ) {
    return URBAN;
  } else if ( (type >= 200) && (type < 210) ) {
    return OTHER;
  } else if ( (type >= 210) && (type < 230) ) {
    return WATER;
  } else {
    return UNKNOWN;
  }
}
