#include <pebble.h>
#include "settings.h"
#include "modes.h"
#include "vibrate.h"

// defines
#define PERSIST_CURRENT_WATCHFACE_MODE 0

// rolling value for bitmap rotation
static const u_short START_IMAGE = 0;
static u_short image_pointer;

// UI elements
static Window *s_main_window;
static TextLayer *s_time_layer; 
static BitmapLayer *s_background_layer;

// fonts
static GFont s_time_font;
static GFont s_date_font;

// image loader/cache stuff
static uint32_t background_resources[] = {
  RESOURCE_ID_RING_WHOLE_SCREEN,
  RESOURCE_ID_CITADEL_BARRACKS_LOGO,
  RESOURCE_ID_BIGRED,
  RESOURCE_ID_SPIKE,
  RESOURCE_ID_CAA_DECAL,
  RESOURCE_ID_CITADEL_SEAL,
};

static GBitmap *s_next_bitmap;
static GBitmap *s_background_bitmap;
static const u_short MAX_IMAGES = 6;

static void load_fonts() {
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIN_STD_30));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LEKTON_REGULAR_10));
}

static void load_initial_image() {
  // read persisted watchface mode, use START_IMAGE if not available
  image_pointer = persist_exists(PERSIST_CURRENT_WATCHFACE_MODE) ? persist_read_int(PERSIST_CURRENT_WATCHFACE_MODE) : START_IMAGE;
  
  // control out of bounds persist (on some platforms it seems to start at max int)
  if(image_pointer >= MAX_IMAGES) {
    image_pointer = 0;
    persist_write_int(PERSIST_CURRENT_WATCHFACE_MODE, image_pointer);
  }
  
  // use image pointer as normal
  s_next_bitmap = gbitmap_create_with_resource(background_resources[image_pointer]);

  // advance to next
}

static void update_image() {
  // wrap around to begin
  image_pointer = image_pointer % MAX_IMAGES;
  
  persist_write_int(PERSIST_CURRENT_WATCHFACE_MODE, image_pointer);
  
  // free current bitmap
  if(NULL != s_background_bitmap) {
    gbitmap_destroy(s_background_bitmap);
  }
  
  // swap next into current
  s_background_bitmap = s_next_bitmap;
  
  // update layer from current
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  
  // load next bitmap into next pointer
  s_next_bitmap = gbitmap_create_with_resource(background_resources[image_pointer]);
  
  // cycle update pointer
  image_pointer++;
}

// when a tap is fired, do this
static void on_tap(AccelAxisType axis, int32_t direction) {
  // debug: vibrate
  //vibes_short_pulse();
  
  // do next image
  update_image();
}

static void update_on_interval() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // get settings
  ElCidSettings *settings = settings_load();

  // check if on change interval
  if((tick_time->tm_min % settings->ModeChangeInterval) < 1) {
    // if on interval, change image
    update_image();
  }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof(" 0:00AP"), "%l:%M%p", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // always update the time
  update_time();
  
  // do dings on the hour
  if(units_changed & HOUR_UNIT) {
    // first call out to vibrate
    vibrate_hourly();
  }
  
  // do updates on minute changes
  if(units_changed & MINUTE_UNIT) {
    // update based on interval
    update_on_interval();
    // handle vibration
    vibrate_quarterly();
  }
}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  update_image();
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 130, 144, 30));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // destroy TextLayer
    text_layer_destroy(s_time_layer);
  
    // destroy background layer
    bitmap_layer_destroy(s_background_layer);
  
    if(NULL != s_next_bitmap) {
      gbitmap_destroy(s_next_bitmap);
    }
    if(NULL != s_background_bitmap) {
      gbitmap_destroy(s_background_bitmap);
    }
}

static void init() {
  // initialize settings
  settings_init();
  
  // load fonts
  load_fonts();
  
  // load initial bitmap
  load_initial_image();
        
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // update the time after pushing
  update_time();
  
  // reconfigure options/subscriptions based on current settings
  //settings_reconfig();
}

static void deinit() {
  // destroy settings
  settings_destroy();
  
  // unsubscribe (all) from tap service
  accel_tap_service_unsubscribe();
  
  // unsubscribe (all) from minute tick
  tick_timer_service_unsubscribe();
  
  // unload fonts
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  // destroy window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}