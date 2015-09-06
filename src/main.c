#include <pebble.h>
#include <get_terrain.h>
#include <gps_strap.h>
  
static Window *s_main_window;
static ActionBarLayer *s_action_bar_layer;
static TextLayer *companion_text_layer;
static BitmapLayer *bitmap_layer;
static TerrainType terrain;

#define NUM_WINDOWS 10
static MenuLayer *list_menu_layer;
static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_WINDOWS;
}
static Window *category_list_window;

static Window *found_window;
static ActionBarLayer *found_action_bar_layer;
static TextLayer *found_text_layer;
static BitmapLayer *found_bitmap_layer;

static void found_window_load(Window *window);
static void found_window_unload(Window *window);

static void category_list_window_load(Window *window);
static void category_list_window_unload(Window *window);

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main select pressed");
  
  category_list_window = window_create();
  window_set_window_handlers(category_list_window, (WindowHandlers) {
      .load = category_list_window_load,
      .unload = category_list_window_unload,
  });
  window_stack_push(category_list_window, true);
}

static void found_window_unload(Window *window) {
  //menu_layer_destroy(s_menu_layer);
}

static void found_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  bitmap_layer = bitmap_layer_create(GRect(15, 80, 80, 80));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading bitmap: %d", terrain);
  GBitmap *bitmap;
  if ( terrain == URBAN ) {
    bitmap = gbitmap_create_with_resource(RESOURCE_ID_URBAN_COM);
  } else if ( terrain == TREES) {
    bitmap = gbitmap_create_with_resource(RESOURCE_ID_TREE_COM);
  } else if ( terrain == WATER ) {
    bitmap = gbitmap_create_with_resource(RESOURCE_ID_WATER_COM);
  } else if ( terrain == OTHER ) {
    bitmap = gbitmap_create_with_resource(RESOURCE_ID_OTHER_COM);
  } else {
    bitmap = gbitmap_create_with_resource(RESOURCE_ID_OTHER_COM);
    return;
  }
  
  bitmap_layer_set_bitmap(bitmap_layer, bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  
  companion_text_layer = text_layer_create(GRect(0, 10, 110, 50));
  text_layer_set_text(companion_text_layer, "You caputured this one!");
  text_layer_set_font(companion_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(companion_text_layer));
  
  found_action_bar_layer = action_bar_layer_create();
  GBitmap *up_icon = gbitmap_create_with_resource(RESOURCE_ID_CHECK_ICON);
  action_bar_layer_set_icon(found_action_bar_layer, BUTTON_ID_SELECT, up_icon);
  action_bar_layer_add_to_window(found_action_bar_layer, found_window);
  
  //window_set_click_config_provider(found_window, click_config_provider);
}

static void category_list_window_unload(Window *window) {
  menu_layer_destroy(list_menu_layer);
}

static void category_list_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading list");
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  companion_text_layer = text_layer_create(GRect(0, 10, 110, 50));
  text_layer_set_text(companion_text_layer, "Capture the Companions");
  text_layer_set_font(companion_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(companion_text_layer));

  list_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(list_menu_layer, category_list_window);
  menu_layer_set_callbacks(list_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback)get_num_rows_callback,
//       .draw_row = (MenuLayerDrawRowCallback)draw_row_callback,
//       .get_cell_height = (MenuLayerGetCellHeightCallback)get_cell_height_callback,
//       .select_click = (MenuLayerSelectCallback)select_callback,
//       .draw_header = (MenuLayerDrawHeaderCallback)draw_header_callback,
//       .get_header_height = (MenuLayerGetHeaderHeightCallback)get_header_height_callback,
//       .get_num_sections = (MenuLayerGetNumberOfSectionsCallback)get_num_sections_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(list_menu_layer));
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  bitmap_layer = bitmap_layer_create(GRect(15, 80, 80, 80));
  GBitmap *bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOGO);
  bitmap_layer_set_bitmap(bitmap_layer, bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  
  companion_text_layer = text_layer_create(GRect(0, 10, 110, 50));
  text_layer_set_text(companion_text_layer, "EXPLOREIANS");
  text_layer_set_font(companion_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(companion_text_layer));
  
  s_action_bar_layer = action_bar_layer_create();
  GBitmap *up_icon = gbitmap_create_with_resource(RESOURCE_ID_LIST_ICON);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT, up_icon);
  action_bar_layer_add_to_window(s_action_bar_layer, window);
  
  window_set_click_config_provider(s_main_window, click_config_provider);
}

static void window_unload(Window *window) {
  //menu_layer_destroy(s_menu_layer);
}

void terrain_thread() {
  
  int32_t lat = gps_get_lat();
  int32_t lon = gps_get_lon();
  
  terrain = get_terrain(lat, lon);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Lat: %ld Lon: %ld Terrain: %d", (long)lat, (long)lon, terrain);
  
  if ( terrain != UNKNOWN ) {
    found_window = window_create();
      window_set_window_handlers(found_window, (WindowHandlers) {
      .load = found_window_load,
      .unload = found_window_unload,
    });
    
    //turn this on to go to screen with companion found
    window_stack_push(found_window, true);
  }
  
  app_timer_register(10000, terrain_thread, NULL);
}
  
static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  gps_init();
  init();
  app_timer_register(1000, terrain_thread, NULL);
  app_event_loop();
  deinit();
  gps_deinit();
}