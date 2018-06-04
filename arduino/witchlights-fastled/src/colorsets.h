#ifndef COLORSETS_H_
#define COLORSETS_H_

#include "FastLED.h"

#define NUM_COLORSETS 5
#define NUM_COLORS_PER_SET 9

extern CRGB colorSets[NUM_COLORSETS][NUM_COLORS_PER_SET];

void createColorsets();

#endif