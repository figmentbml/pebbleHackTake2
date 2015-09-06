#include <pebble.h>
#include <data.h>
#include <math.h>
  
#define LAT_START   39950
#define LAT_END     40190
#define LON_START   105035
#define LON_END     105350
  
typedef enum {
  UNKNOWN,
  URBAN,
  TREES,
  WATER,
  OTHER
} TerrainType;

Window *my_window;
TextLayer *text_layer;

void handle_init(void) {
  my_window = window_create();
  


  text_layer = text_layer_create(GRect(0, 0, 144, 20));
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

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
  lat = lat - LAT_START;
  lon = lon - LON_START;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cheese: %ld , %ld", lat, lon );
  
  // Lookup terrain type
  type = gps_lookup[lon][lat];
  
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

int main(void) {
  handle_init();
  TerrainType t = get_terrain(39950, -105035);
  switch (t) {
    case UNKNOWN:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: UNKNOWN");
      break;
    case URBAN:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: URBAN");
      break;
    case TREES:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: TREES");
      break;
    case WATER:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: WATER");
      break;
    case OTHER:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: OTHER");
      break;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Terrain: %u", get_terrain( 39950, -105035));
  app_event_loop();
  handle_deinit();
}

