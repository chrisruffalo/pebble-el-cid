#include <pebble.h>
#include "face.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_din_std_30;
static BitmapLayer *image_layer;
static TextLayer *time_layer;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 1);
  #endif
  
  s_res_din_std_30 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIN_STD_30));
  // image_layer
  image_layer = bitmap_layer_create(GRect(20, 20, 102, 102));
  layer_add_child(window_get_root_layer(s_window), (Layer *)image_layer);
  
  // time_layer
  time_layer = text_layer_create(GRect(5, 130, 132, 30));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text(time_layer, "00:00 PM");
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, s_res_din_std_30);
  layer_add_child(window_get_root_layer(s_window), (Layer *)time_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(image_layer);
  text_layer_destroy(time_layer);
  fonts_unload_custom_font(s_res_din_std_30);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_face(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_face(void) {
  window_stack_remove(s_window, true);
}

// utility method to set the time string
void face_set_time(char *time_string) {
  text_layer_set_text(time_layer, time_string);
}

// utility method to set the face bitmap
void face_set_bitmap() {
  
}

