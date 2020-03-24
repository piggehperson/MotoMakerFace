#include <pebble.h>

static Window *s_main_window;

static Layer *s_background_layer;
static Layer *s_hands_layer;
static Layer *s_seconds_layer;

//temporary, until settings work
static bool enable_seconds = false;

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
  int length,
  int thickness,
  GColor color
) {
  // Calculate where the end point of the hand goes
  GPoint hand_end = {
    .x = (int16_t)(sin_lookup(angle) * (int32_t)length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(angle) * (int32_t)length / TRIG_MAX_RATIO) + center.y,
  };

  draw_line(ctx, center, hand_end, thickness, color);
}

static void hands_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  //graphics_context_set_fill_color(ctx, GColorClear);
  //graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  const int16_t max_hand_length = PBL_IF_ROUND_ELSE(((bounds.size.w - 30) / 2), (bounds.size.w - 10) / 2);
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // calculate minute hand
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

  // draw minute hand 
  draw_hand(ctx, center, minute_angle, max_hand_length, 5, GColorWhite);
  
  
  // calculate hour hand
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  
  // draw shadow for hour hand, so it doesnt get obscured
  draw_hand(ctx, center, hour_angle, max_hand_length * 0.6, 7, GColorBlack);
  // draw hour hand
  draw_hand(ctx, center, hour_angle, max_hand_length * 0.6, 5, GColorWhite);

  
  // Draw a shadow around the center dot
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 7);

  // Draw the center dot
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 3);
}

static void seconds_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // calculate hand
  int32_t angle = TRIG_MAX_ANGLE * t->tm_sec / 60;

  // draw hand 
  draw_hand(ctx, center, angle, bounds.size.h, 2, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
  
  
  // Draw a shadow around the center dot
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 7);

  // Draw the center dot
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 3);
}

static void background_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // add ticks
}

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