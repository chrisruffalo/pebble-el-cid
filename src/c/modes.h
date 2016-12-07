#pragma once

// watchface modes
#define MODE_RING 0
#define MODE_CITADEL_LOGO 1
#define MODE_CADET_COMPANY 2
#define MODE_BIG_RED 3
#define MODE_SPIKE 4
#define MODE_CAA_LOGO 5
#define MODE_CITADEL_SEAL 6

// watch mode order
static uint32_t WATCH_MODES[] = {
  MODE_RING,
  MODE_CITADEL_LOGO,
  MODE_CADET_COMPANY,
  MODE_BIG_RED,
  MODE_SPIKE,
  MODE_CAA_LOGO,
  MODE_CITADEL_SEAL,
};

// move watchface to the next mode
void next_mode();