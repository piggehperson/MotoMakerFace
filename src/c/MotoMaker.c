#include <pebble.h>

static Window *s_main_window;

static Layer *s_background_layer;
static TextLayer *s_logo_layer;
static Layer *s_hands_layer;
static Layer *s_seconds_layer;

//temporary, until settings work
static bool enable_seconds = true;

// Where layer update_procs live to keep things clean
#include "drawing_layers.c"

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick handler fired");
  // Update minute/hour hands if a minute has passed
  if ((units_changed & MINUTE_UNIT) != 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mark hands layer dirty");
    layer_mark_dirty(s_hands_layer);
  }


  // Update seconds hand if enabled
  if (enable_seconds) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mark seconds layer dirty");
    layer_mark_dirty(s_seconds_layer);
  }
}

static void initialize_seconds() {
  tick_timer_service_unsubscribe();

  if (enable_seconds) {
    // Register with TickTimerService
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Subscribed to tick handler for seconds");

    // Show seconds layer
    layer_set_hidden(s_seconds_layer, false);
  } else {
    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Subscribed to tick handler for minutes");

    // Hide seconds layer
    layer_set_hidden(s_seconds_layer, true);
  }
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *root_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root_layer);
  
  // Add background layer
  s_background_layer = layer_create(bounds);
  layer_add_child(root_layer, s_background_layer);
  layer_set_update_proc(s_background_layer, background_layer_update_proc);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Background layer added");

  // Add pebble logo layer
  GRect logo_bounds = GRect(0, 37, bounds.size.w, 24);
  s_logo_layer = text_layer_create(logo_bounds);
  text_layer_set_text(s_logo_layer, "pebble");
  text_layer_set_text_color(s_logo_layer, PBL_IF_COLOR_ELSE(GColorLightGray, GColorWhite));
  text_layer_set_text_alignment(s_logo_layer, GTextAlignmentCenter);
  //text_layer_set_font(s_logo_layer, fonts_get_system_font());
  layer_add_child(root_layer, text_layer_get_layer(s_logo_layer));

  // Add hour and minute hands layer
  s_hands_layer = layer_create(bounds);
  layer_add_child(root_layer, s_hands_layer);
  layer_set_update_proc(s_hands_layer, hands_layer_update_proc);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Hands layer added");

  // Create seconds layer
  s_seconds_layer = layer_create(bounds);
  layer_set_update_proc(s_seconds_layer, seconds_layer_update_proc);
  layer_add_child(root_layer, s_seconds_layer);
  // Show or hide seconds layer, and
  // subscribe to correct tick timer service
  initialize_seconds();
}

static void main_window_unload(Window *window) {
  layer_destroy(s_background_layer);
  layer_destroy(s_hands_layer);
  layer_destroy(s_seconds_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}