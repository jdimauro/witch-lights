#ifndef COLORSETS_H_
#define COLORSETS_H_

#include "FastLED.h"

#define NUM_COLORSETS					5
#define NUM_COLORS_PER_SET		9
#define ANIMATION_FRAME_WIDTH			23
#define ANIMATION_FRAMES					28

// animation frame counts

#define SPARKLE_ANIMATION_FRAME_WIDTH 23
#define SPARKLE_ANIMATION_FRAMES			46

#define afc_f_slow_stop_ANIMATION_FRAME_WIDTH		17
#define afc_f_slow_stop_ANIMATION_FRAMES				65

#define afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH 25
#define afc_f_slow_stop_c_ANIMATION_FRAMES			82

#define afc_l_pulsar_a_ANIMATION_FRAME_WIDTH		24
#define afc_l_pulsar_a_ANIMATION_FRAMES					22


extern CRGB colorSets[NUM_COLORSETS][NUM_COLORS_PER_SET];

void createColorsets();

#endif