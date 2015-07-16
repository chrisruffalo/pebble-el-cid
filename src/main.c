#include <pebble.h>

// static values
//static const u_short WIDTH = 144;
//static const u_short HEIGHT = 168;
  
// config
static const int VIBRATE_PULSE_MS = 120;
static const int VIBRATE_PAUSE_MS = 100;
static const int CHANGE_INTERVAL = 15; // change image every 15 minutes
  
// rolling value for bitmap rotation
static u_short image_pointer = 1;
  
static Window *s_main_window;
static TextLayer *s_time_layer; 

// bitmap layer
static BitmapLayer *s_background_layer;

// image loader/cache stuff
static uint32_t background_resources[] = {
  RESOURCE_ID_RING_WHOLE_SCREEN,
  RESOURCE_ID_CITADEL_BARRACKS_LOGO,
  RESOURCE_ID_BIGRED,
  RESOURCE_ID_SPIKE,
  RESOURCE_ID_CAA_DECAL,
  RESOURCE_ID_CITADEL_SEAL,
  RESOURCE_ID_LOGO_TEXT
};
static GBitmap *s_next_bitmap;
static GBitmap *s_background_bitmap;
static const u_short MAX_IMAGES = 7;

static void update_image() {
  // wrap around to begin
  if(image_pointer >= MAX_IMAGES) {
    image_pointer = 0;
  }

  // free current bitmap
  gbitmap_destroy(s_background_bitmap);
  
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
  update_image();
}

static void update_on_interval() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // check if on change interval
  if(tick_time->tm_min % CHANGE_INTERVAL == 0) {
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
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}


// handle hour vibrations
static void ding_handler() {
  // get hour from local time  
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // create pattern for vibration
  int hours = tick_time->tm_hour;
  uint32_t segments[hours * 2];
  for(int i = 0; i < hours; i += 2) {
    segments[i] = VIBRATE_PULSE_MS; // short pulse each hour
    segments[i + 1] = VIBRATE_PAUSE_MS; // followed by a small pause
  }
  // enqueue pattern
  VibePattern pat = {
    .durations = segments,
    .num_segments = hours,
  };
  vibes_enqueue_custom_pattern(pat);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // always update the time
  update_time();
  
  // do dings on the hour
  if(units_changed & HOUR_UNIT) {
    ding_handler();
  }
  
  // rotate image?
  if(units_changed & MINUTE_UNIT) {
    update_on_interval();
  }
}
static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  update_image();
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 113, 144, 55));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_time_layer);
  
    if(NULL != s_next_bitmap) {
      gbitmap_destroy(s_next_bitmap);
    }
    if(NULL != s_background_bitmap) {
      gbitmap_destroy(s_background_bitmap);
    }
}

static void init() {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // load
  s_next_bitmap = gbitmap_create_with_resource(background_resources[0]);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
 
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // update the time after pushing
  update_time();
  
  // subscribe to tap serice
  accel_tap_service_subscribe(on_tap);
}

static void deinit() {
  // unsubscribe (all) from tap service
  accel_tap_service_unsubscribe();
  
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}