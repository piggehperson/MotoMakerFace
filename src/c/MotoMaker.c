#include <pebble.h>

static Window *s_main_window;
static Layer *s_hands_layer;

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

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  const int16_t max_hand_length = (bounds.size.w -10) / 2;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // calculate minute hand
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

  // draw minute hand 
  draw_hand(ctx, center, minute_angle, max_hand_length, 6, GColorWhite);
  
  
  // calculate hour hand
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  
  // draw shadow for hour hand, so it doesnt get obscured
  draw_hand(ctx, center, hour_angle, max_hand_length * 0.6, 8, GColorBlack);
  // draw hour hand
  draw_hand(ctx, center, hour_angle, max_hand_length * 0.6, 6, GColorWhite);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *root_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root_layer);
  
  s_hands_layer = layer_create(bounds);
  layer_add_child(root_layer, s_hands_layer);
  layer_set_update_proc(s_hands_layer, hands_layer_update_proc);
}

static void main_window_unload(Window *window) {

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
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}