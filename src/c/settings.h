#pragma once

#define PERSIST_SETTINGS 1

// configuration struct for storing settings
// that are returned by the JS
typedef struct ElCidSettings {
  uint32_t ClassYear;
  const char * CadetCompany;
  bool WatchfaceRotation[7];
  u_short ModeChangeInterval;
  bool HourlyVibrate;
  u_short HourVibrationType;
  bool QuarterHourlyVibrate;
  u_short QuarterHourVibrationType;
  bool ModeChangeShake;
} ElCidSettings;

ElCidSettings * settings_load();
void settings_init();
void settings_destroy();
void settings_reconfig();