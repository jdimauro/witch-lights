#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <Arduino.h>

#include "FastLED.h"

// FastLED constants
#define NUM_LEDS							750 // 750 or 600 in production

#define SCANNER_SPRITE_FRAME_DELAY_IN_MS			1
#define TEST_PATTERN_FRAME_DELAY_IN_MS				1
#define SCANNER_DELAY_INTERVAL_IN_MS					 20

// Array representing the strip.
extern CRGB leds[];

const bool debugMode = true;					// turns on debug() statements

void debug(int number);
void debugNeg(int number);
void debugN(int startPos, int number);
// TODO: faeries go both ways
void stripcpy(CRGB *leds, CRGB *source, int start, int width, int patternSize);

#endif