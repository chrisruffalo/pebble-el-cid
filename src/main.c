#include <pebble.h>

// static values
//static const u_short WIDTH = 144;
//static const u_short HEIGHT = 168;
  
// config: hourly vibrate
static const u_short VIBRATE_TYPE = 4; // 0 = none, 1 = short, 2 = long, 3 = double, 4 = pattern
static const u_short QUARTER_HOUR_VIBRATE = 0; // 0 = none, 1 = short, 2 = long, 3 = double
static const bool ABBREVIATE_PULSES = true;
static const uint32_t VIBRATE_PULSE_MS = 110;
static const uint32_t VIBRATE_LONG_PULSE_MS = 330;
static const uint32_t VIBRATE_PAUSE_MS = 240;

// config: image change
static const int CHANGE_INTERVAL = 10; // change image every N minutes


// single array of time segments, allocated once
static uint32_t VIBRATE_SEGMENTS[48];
static u_short MAX_SEGMENTS = 48;


// rolling value for bitmap rotation
static const u_short START_IMAGE = 1;
static u_short image_pointer;
  
static Window *s_main_window;
static TextLayer *s_time_layer; 

// fonts
static GFont s_time_font;
static GFont s_date_font;

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
};
static GBitmap *s_next_bitmap;
static GBitmap *s_background_bitmap;
static const u_short MAX_IMAGES = 6;

static void load_fonts() {
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIN_STD_30));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LEKTON_REGULAR_10));
}

static void load_initial_image() {
  s_next_bitmap = gbitmap_create_with_resource(background_resources[START_IMAGE]);
  image_pointer = START_IMAGE+1;
}

static void update_image() {
  // wrap around to begin
  image_pointer = image_pointer % MAX_IMAGES;
    
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
    strftime(buffer, sizeof(" 0:00AP"), "%l:%M%p", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}


// handle hour vibrations
static void custom_ding_handler() {
  // get hour from local time  
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // create pattern for vibration
  int hours = tick_time->tm_hour;
  
  // adjust for non-24-hour time
  if(clock_is_24h_style() != true && hours > 12) {
    hours = hours - 12;
  }
  
  // need pattern count
  int pattern_count = hours * 2;
  int long_pulses = 0;

  // do math to abbreviate pulses
  if(ABBREVIATE_PULSES && hours > 5) {
    int long_pulses = hours / 5;
    pattern_count = pattern_count - (long_pulses * 2 * 5) + 1; // eats 10 "spaces" or 
  }
  // chop off remainder (last space is always silent/pause otherwise)
  pattern_count = pattern_count - 1;
  
  // fill array with pattern
  for(int i = 0; i < MAX_SEGMENTS; i += 2) {
    if(pattern_count > 0) {
      if(long_pulses > 0) {
        long_pulses--;
        VIBRATE_SEGMENTS[i] = VIBRATE_LONG_PULSE_MS; // longer pulse for abbreviation
      } else {
        VIBRATE_SEGMENTS[i] = VIBRATE_PULSE_MS; // pulse each hour
      }    
      if(MAX_SEGMENTS > (i + 1)) {
        VIBRATE_SEGMENTS[i + 1] = VIBRATE_PAUSE_MS; // followed by a pause
      }
      pattern_count--;
    } else {
      VIBRATE_SEGMENTS[i] = 0;
      if(MAX_SEGMENTS > (i + 1)) {
        VIBRATE_SEGMENTS[i + 1] = 0;
      }
    }
  }
  
  // enqueue pattern
  VibePattern pat = {
    .durations = VIBRATE_SEGMENTS,
    .num_segments = pattern_count,
  };
  vibes_enqueue_custom_pattern(pat);
}

static void hourly_handler() {
  if(1 == VIBRATE_TYPE) {
    vibes_short_pulse();
  } else if(2 == VIBRATE_TYPE) {
    vibes_long_pulse();
  } else if(3 == VIBRATE_TYPE) {
    vibes_double_pulse();
  } else if(4 == VIBRATE_TYPE) {
    custom_ding_handler();
  }
}

static void quarter_hour_handler() {
  if(0 == QUARTER_HOUR_VIBRATE) {
    return;
  }
  
  // get hour from local time  
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // create pattern for vibration
  int minutes = tick_time->tm_min;
  
  // abort if not on the quarter hour
  if(minutes == 0 || minutes % 15 != 0) {
    return;
  }
  
  // do pattern as configured
  if(1 == QUARTER_HOUR_VIBRATE) {
    vibes_short_pulse();
  } else if(2 == QUARTER_HOUR_VIBRATE) {
    vibes_long_pulse();
  } else if(3 == QUARTER_HOUR_VIBRATE) {
    vibes_double_pulse();
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // always update the time
  update_time();
  
  // do dings on the hour
  if(units_changed & HOUR_UNIT) {
    hourly_handler();
  }
  
  // do updates on minute changes
  if(units_changed & MINUTE_UNIT) {
    update_on_interval();
    quarter_hour_handler();
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