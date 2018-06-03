#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "FastLED.h"

#define ANIMATION_FRAME_WIDTH 23
#define ANIMATION_FRAMES 28

// animation frame counts

#define SPARKLE_ANIMATION_FRAME_WIDTH 23
#define SPARKLE_ANIMATION_FRAMES 46

#define afc_f_slow_stop_ANIMATION_FRAME_WIDTH 17
#define afc_f_slow_stop_ANIMATION_FRAMES 65

#define afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH 25
#define afc_f_slow_stop_c_ANIMATION_FRAMES 82

#define afc_l_pulsar_a_ANIMATION_FRAME_WIDTH 24
#define afc_l_pulsar_a_ANIMATION_FRAMES 22

extern char afc_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
extern CRGB af_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

extern char
    afc_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];
extern CRGB
    af_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];

extern char
    afc_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];
extern CRGB
    af_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];

extern char afc_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];
extern CRGB af_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];

void createAnimationFrames();

#endif