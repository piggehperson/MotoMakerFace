#include <pebble.h>
#include "settings.h"


static Window *s_main_window;
static Layer *s_window_layer;

static Layer *s_background_layer;
static Layer *s_hands_layer;
static Layer *s_seconds_layer;

// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.enable_wordmark = true;
  // const char *default = "pebble";
  // memset(settings.text_wordmark, 0, sizeof settings.text_wordmark);
  // strncpy(settings.text_wordmark, default, sizeof settings.text_wordmark-1);
  settings.enable_double_12 = true;
  
  settings.color_background = GColorBlack;
  settings.color_dot = GColorWhite;
  settings.color_hour_hand = GColorWhite;
  settings.color_minute_hand = GColorWhite;
  settings.color_second_hand = PBL_IF_COLOR_ELSE(GColorRed, GColorWhite);
  settings.color_hour_markers = GColorLightGray;
  settings.color_minute_markers = PBL_IF_COLOR_ELSE(GColorLightGray, GColorWhite);
  settings.color_wordmark = PBL_IF_COLOR_ELSE(GColorLightGray, GColorWhite);

  settings.enable_second_hand = true;
  settings.enable_vibrate_on_disconnect = true;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick handler fired");
  // Update minute/hour hands if a minute has passed
  if ((units_changed & MINUTE_UNIT) != 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mark hands layer dirty");
    layer_mark_dirty(s_hands_layer);
  }

  // Update seconds hand if enabled
  if (settings.enable_second_hand) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mark seconds layer dirty");
    layer_mark_dirty(s_seconds_layer);
  }
}

static void initialize_seconds() {
  tick_timer_service_unsubscribe();

  if (settings.enable_second_hand) {
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

// Update the display elements
static void prv_update_display() {
  layer_mark_dirty(s_background_layer);
  layer_mark_dirty(s_hands_layer);
  layer_mark_dirty(s_seconds_layer);
  initialize_seconds();
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Widgets
  Tuple *enable_wordmark_t = dict_find(iter, MESSAGE_KEY_enableWordmark);
  if (enable_wordmark_t) {
    settings.enable_wordmark = enable_wordmark_t->value->int32 == 1;
  }
  // Tuple *text_wordmark_t = dict_find(iter, MESSAGE_KEY_textWordmark);
  // if (text_wordmark_t) {
  //   settings.text_wordmark = (char) text_wordmark_t->value->cstring;
  // }
  Tuple *enable_double_12_t = dict_find(iter, MESSAGE_KEY_enableDouble12);
  if (enable_double_12_t) {
    settings.enable_double_12 = enable_double_12_t->value->int32 == 1;
  }
  
  // Colors
  Tuple *color_background_t = dict_find(iter, MESSAGE_KEY_colorBackground);
  if (color_background_t) {
    settings.color_background = GColorFromHEX(color_background_t->value->int32);
  }
  Tuple *color_dot_t = dict_find(iter, MESSAGE_KEY_colorDot);
  if (color_dot_t) {
    settings.color_dot = GColorFromHEX(color_dot_t->value->int32);
  }
  Tuple *color_hour_markers_t = dict_find(iter, MESSAGE_KEY_colorHourMarkers);
  if (color_hour_markers_t) {
    settings.color_hour_markers = GColorFromHEX(color_hour_markers_t->value->int32);
  }
  Tuple *color_minute_markers_t = dict_find(iter, MESSAGE_KEY_colorMinuteMarkers);
  if (color_minute_markers_t) {
    settings.color_minute_markers = GColorFromHEX(color_minute_markers_t->value->int32);
  }
  Tuple *color_hour_hand_t = dict_find(iter, MESSAGE_KEY_colorHourHand);
  if (color_hour_hand_t) {
    settings.color_hour_hand = GColorFromHEX(color_hour_hand_t->value->int32);
  }
  Tuple *color_minute_hand_t = dict_find(iter, MESSAGE_KEY_colorMinuteHand);
  if (color_minute_hand_t) {
    settings.color_minute_hand = GColorFromHEX(color_minute_hand_t->value->int32);
  }
  Tuple *color_second_hand_t = dict_find(iter, MESSAGE_KEY_colorSecondHand);
  if (color_second_hand_t) {
    settings.color_second_hand = GColorFromHEX(color_second_hand_t->value->int32);
  }
  Tuple *color_logo_t = dict_find(iter, MESSAGE_KEY_colorWordmark);
  if (color_logo_t) {
    settings.color_wordmark = GColorFromHEX(color_logo_t->value->int32);
  }

  // Features
  Tuple *enable_second_hand_t = dict_find(iter, MESSAGE_KEY_enableSecondHand);
  if (enable_second_hand_t) {
    settings.enable_second_hand = enable_second_hand_t->value->int32 == 1;
  }
  Tuple *enable_vibrate_on_disconnect_t = dict_find(iter, MESSAGE_KEY_enableVibrateOnDisconnect);
  if (enable_vibrate_on_disconnect_t) {
    settings.enable_vibrate_on_disconnect = enable_vibrate_on_disconnect_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

static void bluetooth_callback(bool connected) {
  if (!connected && settings.enable_vibrate_on_disconnect) {
    // vibrate watch
    vibes_double_pulse();
  }
}

// Where layer update_procs live to keep things clean
static void draw_line(
  GContext *ctx,
  GPoint start,
  GPoint end,
  int thickness,
  GColor color
  ) {
    // Set stroke style
    graphics_context_set_stroke_color(ctx, color);
    graphics_context_set_stroke_width(ctx, thickness);

    // Draw stroke
    graphics_draw_line(ctx, start, end);
}

static void draw_hand(
  GContext *ctx,
  GPoint center,
  int angle,
  int inset,
  int length,
  int thickness,
  GColor color
) {

  // Calculate where the start point of the hand goes
  GPoint hand_start = {
    .x = (int16_t)(sin_lookup(angle) * (int32_t)inset / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(angle) * (int32_t)inset / TRIG_MAX_RATIO) + center.y,
  };

  // Calculate where the end point of the hand goes
  GPoint hand_end = {
    .x = (int16_t)(sin_lookup(angle) * (int32_t)length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(angle) * (int32_t)length / TRIG_MAX_RATIO) + center.y,
  };

  // Draw a shadow around the hand
  draw_line(ctx, hand_start, hand_end, thickness + 2, settings.color_background);
  // Draw the actual hand
  draw_line(ctx, hand_start, hand_end, thickness, color);
}

static void hands_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  const int16_t max_hand_length = PBL_IF_ROUND_ELSE(((bounds.size.w - 30) / 2), (bounds.size.w - 10) / 2);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // calculate minute hand
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

  // draw minute hand
  draw_hand(ctx, center, minute_angle, 10, max_hand_length, 5, settings.color_minute_hand);


  // calculate hour hand
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);

  // draw hour hand
  draw_hand(ctx, center, hour_angle, 10, max_hand_length * 0.6, 5, settings.color_hour_hand);

  // Draw the center dot
  graphics_context_set_fill_color(ctx, settings.color_dot);
  graphics_fill_circle(ctx, center, 3);
}

static void seconds_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // calculate hand
  int32_t angle = TRIG_MAX_ANGLE * t->tm_sec / 60;

  // draw hand
  draw_hand(ctx, center, angle, 10, bounds.size.h, 2, settings.color_second_hand);

  // Draw the center dot
  graphics_context_set_fill_color(ctx, settings.color_dot);
  graphics_fill_circle(ctx, center, 3);
}

static void background_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  const int16_t max_hand_length = PBL_IF_ROUND_ELSE(((bounds.size.w - 30) / 2), (bounds.size.w - 10) / 2);

  graphics_context_set_fill_color(ctx, settings.color_background);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // add ticks
  for (int i = 5; i < 60; i = i + 5) {
    // calculate hand
    int32_t angle = TRIG_MAX_ANGLE * i / 60;

    // draw hand
    draw_hand(ctx, center, angle, max_hand_length * 0.8, bounds.size.h, 1, settings.color_minute_markers);
  }
  for (int i = 0; i < 60; i = i + 15) {
    // calculate hand
    int32_t angle = TRIG_MAX_ANGLE * i / 60;

    // draw hand
    draw_hand(ctx, center, angle, max_hand_length * 0.8, bounds.size.h, 2, settings.color_hour_markers);
  }

  if (
    layer_get_bounds(layer).size.h == layer_get_unobstructed_bounds(layer).size.h &&
    settings.enable_wordmark
  ) {
    // Draw wordmark
    graphics_context_set_text_color(ctx, settings.color_wordmark);
    graphics_draw_text(
      ctx,
      "pebble",
      fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
      GRect(0, 37, bounds.size.w, 24),
      GTextOverflowModeTrailingEllipsis,
      GTextAlignmentCenter,
      0
    );
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing background layer");
}

static void main_window_load(Window *window) {
  // Get information about the Window
  s_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(s_window_layer);

  // Add background layer
  s_background_layer = layer_create(bounds);
  layer_add_child(s_window_layer, s_background_layer);
  layer_set_update_proc(s_background_layer, background_layer_update_proc);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Background layer added");

  // Add hour and minute hands layer
  s_hands_layer = layer_create(bounds);
  layer_add_child(s_window_layer, s_hands_layer);
  layer_set_update_proc(s_hands_layer, hands_layer_update_proc);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Hands layer added");

  // Create seconds layer
  s_seconds_layer = layer_create(bounds);
  layer_set_update_proc(s_seconds_layer, seconds_layer_update_proc);
  layer_add_child(s_window_layer, s_seconds_layer);
  // Show or hide seconds layer, and
  // subscribe to correct tick timer service
  initialize_seconds();

  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  // Show the correct state of the BT connection from the start
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void main_window_unload(Window *window) {
  layer_destroy(s_background_layer);
  layer_destroy(s_hands_layer);
  layer_destroy(s_seconds_layer);
}

static void init() {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  prv_load_settings();

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
