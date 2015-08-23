#include "main.h"
#include "pebble.h"
  

static Window *window;
static Layer *s_hands_layer, *s_simple_bg_layer;


static GPath *s_minute_arrow, *s_hour_arrow, *s_second_arrow;

static void bg_update_proc(Layer *layer, GContext *ctx) {
  /*
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_draw_filled(ctx, s_tick_paths[i]);
  }
  */
  graphics_context_set_fill_color(ctx, GColorBlack);
  //12
  graphics_fill_circle(ctx, GPoint(72, 48 - 12), 2);
  //3
  graphics_fill_circle(ctx, GPoint(48 + 72, 84), 2);
  //6
  graphics_fill_circle(ctx, GPoint(72,84 + 48), 2);
  //9
  graphics_fill_circle(ctx, GPoint(72 - 48, 84), 2);
  
  //off by a bit. Old differences were 24 and 42
  //1
  graphics_fill_circle(ctx, GPoint(72 + 23, 84 - 41), 2);
  //2
  graphics_fill_circle(ctx, GPoint(72 + 41, 84 - 24), 2);
  
  //4
  graphics_fill_circle(ctx, GPoint(72 + 41, 84 + 23), 2);
  //5
  graphics_fill_circle(ctx, GPoint(72 + 24, 84 + 41), 2);
  
  //7
  graphics_fill_circle(ctx, GPoint(72 - 23, 84 + 41), 2);
  //8
  graphics_fill_circle(ctx, GPoint(72 - 41, 84 + 24), 2);
  
  //10
  graphics_fill_circle(ctx, GPoint(72 - 41, 84 - 23), 2);
  //11
  graphics_fill_circle(ctx, GPoint(72 - 24, 84 - 41), 2);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  graphics_draw_circle(ctx, center, bounds.size.w / 2);
  
  int16_t second_hand_length = bounds.size.w / 3;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  };

  gpath_rotate_to(s_second_arrow, second_angle);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, second_hand, 5);
  // second hand
  //graphics_context_set_stroke_color(ctx, GColorBlack);
  //graphics_draw_line(ctx, second_hand, center);

  // minute/hour hand
  //graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_context_set_stroke_color(ctx, GColorBlack);
  int16_t minute_hand_length = bounds.size.w / 3;
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  
  gpath_rotate_to(s_minute_arrow, minute_angle);
  //gpath_draw_filled(ctx, s_minute_arrow);
  //gpath_draw_outline(ctx, s_minute_arrow);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  //graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, minute_hand, 15);
  //graphics_context_set_fill_color(ctx, GColorWhite);
  //graphics_fill_circle(ctx, minute_hand, 8);

  int16_t hour_hand_length = bounds.size.w / 3;
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  gpath_rotate_to(s_hour_arrow, hour_angle);
  //gpath_draw_filled(ctx, s_hour_arrow);
  //gpath_draw_outline(ctx, s_hour_arrow);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, hour_hand, 8);
  
  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
  graphics_fill_circle(ctx, center, 3);
}
  
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);
  
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

}

static void window_unload(Window *window) {

  layer_destroy(s_hands_layer);
}
  
static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  // init hand paths
  /*
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);
  */

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}
  
static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  tick_timer_service_unsubscribe();
  window_destroy(window);
}
    
int main(void) {
  init();
  app_event_loop();
  deinit();
}
