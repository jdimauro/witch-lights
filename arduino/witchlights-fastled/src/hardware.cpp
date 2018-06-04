#include "hardware.h"

CRGB leds[NUM_LEDS];

void debug(int number) {
  if (!debugMode) return;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fill_solid(leds, number < NUM_LEDS ? number : NUM_LEDS, 0x202020);
  FastLED.show();
}

void debugNeg(int number) {
  if (!debugMode) return;
  fill_solid(leds + NUM_LEDS - number, number < NUM_LEDS ? number : NUM_LEDS, 0x202020);
}

void debugN(int startPos, int number) {
  if (!debugMode) return;
  fill_solid(leds + startPos, number < (NUM_LEDS - startPos) ? number : NUM_LEDS - startPos,
             0x202020);
}

void stripcpy(CRGB *leds, CRGB *source, int start, int width, int patternSize) {
  // We would be writing off the end anyway, so just bail. Nothing to do.
  if (start >= NUM_LEDS) {
    return;
  }

  // The position on the strip where the pattern will begin.
  int actualStartPosition = start >= 0 ? start : 0;

  int startPlusWidth = start + width;

  // The position on the strip AFTER the last pixel drawn.
  int actualEndPosition = startPlusWidth <= NUM_LEDS ? startPlusWidth : NUM_LEDS;

  // How many bytes into the source do we start copying?
  int patternStart = start >= 0 ? 0 : -start;

  // This may be negative, which indicates we'd finish writing before we get to
  // the start.
  int actualBytes = actualEndPosition - actualStartPosition;

  if (actualBytes > 0) {
    memcpy(leds + actualStartPosition,
           (patternStart <= patternSize) ? source + patternStart : source,
           actualBytes * sizeof(CRGB));
  }
}