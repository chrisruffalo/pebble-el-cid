#include <pebble.h>
#include "settings.h"

static const u_short MODES = 7;

// static settings object
static ElCidSettings *app_settings;

static void settings_default(ElCidSettings *settings) {
  // set default values
  settings->ClassYear = -1;
  settings->CadetCompany = "";
  settings->ModeChangeInterval = 10;
  settings->HourlyVibrate = true;
  settings->HourVibrationType = 2;
  settings->QuarterHourlyVibrate = false;
  settings->QuarterHourVibrationType = 1;

  // copy enabled watch faces array over
  static const bool temporary[] = { true, true, true, true, true, true, true };  
  memcpy(settings->WatchfaceRotation, temporary, sizeof(temporary));
}

ElCidSettings * settings_load() {
  
  // if the settings object is not loaded then load it
  if(!app_settings) {
    // create settings instance
    app_settings = (ElCidSettings*)malloc(sizeof(ElCidSettings));
    // set instance to default values
    settings_default(app_settings);
  }
    
  // check and see if the settings instance was defined
  if(persist_exists(PERSIST_SETTINGS)) {
    persist_read_data(PERSIST_SETTINGS, app_settings, sizeof(ElCidSettings));
  }
  
  return app_settings;
}

static void settings_save(ElCidSettings * settings) {
  // persist settings
  persist_write_data(PERSIST_SETTINGS, settings, sizeof(ElCidSettings));
}

// method to handle inbox messages from 
static void settings_inbox_handler(DictionaryIterator *iter, void *context) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Parsing settings from PebbleKit...");
  
  // read hourly configuration
  Tuple *hourlyVibrate = dict_find(iter, MESSAGE_KEY_HourVibration);
  if(hourlyVibrate) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Hourly Vibrate: '%d'", (int)hourlyVibrate->value->int32);
    app_settings->HourlyVibrate = hourlyVibrate->value->int32 > 0;
  }
  Tuple *hourlyVibrateType = dict_find(iter, MESSAGE_KEY_HourVibrationType);
  if(hourlyVibrateType) {    
    char *vibrateType = hourlyVibrateType->value->cstring;
    if(strlen(vibrateType) > 0) {
      u_short value = vibrateType[0] - '0';
      app_settings->HourVibrationType = value;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Hourly Vibrate Type: '%d'", (int)value);
    }    
  }
  
  // read quarter-hour configuration
  Tuple *quarterlyVibrate = dict_find(iter, MESSAGE_KEY_QuarterHourVibration);
  if(quarterlyVibrate) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Quarterly Vibrate: '%d'", (int)quarterlyVibrate->value->int32);
    app_settings->QuarterHourlyVibrate = quarterlyVibrate->value->int32 > 0;
  }
  Tuple *quarterVibrateType = dict_find(iter, MESSAGE_KEY_QuarterHourVibrationType);
  if(quarterVibrateType) {    
    char *vibrateType = quarterVibrateType->value->cstring;
    if(strlen(vibrateType) > 0) {
      u_short value = vibrateType[0] - '0';
      app_settings->QuarterHourVibrationType = value;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Quarterly Vibrate Type: '%d'", (int)value);
    }
  } 
  
  // mode change interval
  Tuple *modeChangeInterval = dict_find(iter, MESSAGE_KEY_ModeChangeInterval);
  if(modeChangeInterval) {
    app_settings->ModeChangeInterval = (u_short)modeChangeInterval->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode Change Interval: '%d'", (int)app_settings->ModeChangeInterval);
  }
  
  // read modes
  for(int offset = 0; offset < MODES; offset++) {
    Tuple *watchfaceMode = dict_find(iter, MESSAGE_KEY_WatchfaceRotation + offset);
    if(watchfaceMode) {
      app_settings->WatchfaceRotation[offset] = watchfaceMode->value->int32 > 0;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode [%d] value: '%d'", offset, app_settings->WatchfaceRotation[offset]);
    }
  }
  
  // read change-mode on shake
  Tuple *modeChangeShake = dict_find(iter, MESSAGE_KEY_ModeChangeShake);
  if(modeChangeShake) {
    app_settings->ModeChangeShake = modeChangeShake->value->int32 > 0;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode Change on Shake: '%d'", (int)app_settings->ModeChangeShake);
  }
    
  // save settings when done
  settings_save(app_settings);
  
  // and then reconfigure based on settings
  settings_reconfig();
}

// initialize settings collection/control
void settings_init() {
  // load settings
  settings_load();
  
  // register for callbacks
  app_message_register_inbox_received(settings_inbox_handler);
  app_message_open(256, 32);
}

void settings_destroy() {
  // deregister
  app_message_deregister_callbacks();
  
  // free settings object
  free(app_settings);
}

void settings_reconfig() {
  // register for shake events as needed
  /*
  if(app_settings->ModeChangeShake) {
    accel_tap_service_subscribe(on_tap);    
  } else {
    accel_tap_service_unsubscribe();
  }
  */
}
