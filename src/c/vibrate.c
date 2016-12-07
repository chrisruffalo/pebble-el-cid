#include <pebble.h>
#include "vibrate.h"
#include "settings.h"

// constant values
static const bool ABBREVIATE_PULSES = true;
static const uint32_t VIBRATE_PULSE_MS = 110;
static const uint32_t VIBRATE_LONG_PULSE_MS = 330;
static const uint32_t VIBRATE_PAUSE_MS = 240;

static void vibrate_hourly_pattern() {
  // values
  uint32_t VIBRATE_SEGMENTS[48];
  u_short MAX_SEGMENTS = 48;
  
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
    long_pulses = hours / 5;
    pattern_count = pattern_count - (long_pulses * 2 * 5) + 1; // eats 10 "spaces" or 
  }
  // chop off remainder (last space is always silent/pause otherwise)
  pattern_count = pattern_count - 1;
  int vibrate_segment_count = pattern_count; // store value
  
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
      pattern_count-=2; // subtract two because two pattern blocks
    } else {
      VIBRATE_SEGMENTS[i] = 0;
      if(MAX_SEGMENTS > (i + 1)) {
        VIBRATE_SEGMENTS[i + 1] = 0;
      }
    }
  }
  
  // enqueue
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enqueueing %d segments for hours %d", vibrate_segment_count, hours);
  
  // enqueue pattern
  VibePattern pat = {
    .durations = VIBRATE_SEGMENTS,
    .num_segments = vibrate_segment_count,
  };
  vibes_enqueue_custom_pattern(pat);
}

void vibrate_hourly() {
  // load settings
  ElCidSettings *settings = settings_load();
 
  // don't vibrate if not set
  if(!settings->HourlyVibrate) {
    return;
  }

  u_short vibration_type = settings->HourVibrationType;
  
  // chose what type of vibration to do
  if(1 == vibration_type) {
    vibes_short_pulse();
  } else if(2 == vibration_type) {
    vibes_long_pulse();
  } else if(3 == vibration_type) {
    vibes_double_pulse();
  } else if(4 == vibration_type) {
    vibrate_hourly_pattern();
  }  
}

static void vibrate_quarterly_pattern(int minutes) {
  if(minutes % 15 == 0) {
    vibes_short_pulse();
  }
  
  if(minutes % 30 == 0) {
    vibes_short_pulse();
  }
  
  if(minutes % 45 == 0) {
    vibes_short_pulse();
  }
}

void vibrate_quarterly() {
  // load settings
  ElCidSettings *settings = settings_load();
    
  // don't vibrate if not set
  if(!settings->QuarterHourlyVibrate) {
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
  
  // get settings value after we know we need it
  u_short vibration_type = settings->QuarterHourVibrationType;
    
  // do pattern as configured
  if(1 == vibration_type) {
    vibes_short_pulse();
  } else if(2 == vibration_type) {
    vibes_long_pulse();
  } else if(3 == vibration_type) {
    vibes_double_pulse();
  } else if(4 == vibration_type) {
    vibrate_quarterly_pattern(minutes);
  }
}

