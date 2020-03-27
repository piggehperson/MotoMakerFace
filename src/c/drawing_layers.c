#include <pebble.h>

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
  GRect bounds = layer_get_unobstructed_bounds(layer);
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
  GRect bounds = layer_get_unobstructed_bounds(layer);
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