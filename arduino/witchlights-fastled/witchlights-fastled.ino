#include <FastLED.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define NUM_LEDS            750
#define MAXSPRITES            4

#define NUM_COLORSETS         5
#define NUM_COLORS_PER_SET    9

#define PIR_SENSOR_1_PIN     3
#define PIR_SENSOR_2_PIN     4
#define NEOPIXEL_DATA_PIN    6                // Pin for neopixels

#define INFRARED_SENSOR_TIMEOUT_IN_MS   10000  // Ten seconds.

#define SCANNER_SPRITE_FRAME_DELAY_IN_MS     1
#define TEST_PATTERN_FRAME_DELAY_IN_MS       1

#define SCANNER_MIN_SCANS    2
#define SCANNER_MAX_SCANS    5

// currently set this to be consistent for animation design
#define SCANNER_MIN_STOP_DISTANCE    50   // This probably shouldn't be smaller than 40. If it is scanners may get stuck in place if they don't have enough "exit velocity". // 40
#define SCANNER_MAX_STOP_DISTANCE    60   // 120

#define SPRITE_STARTING_DELAY_INTERVAL_IN_MS   40
#define SCANNER_DELAY_INTERVAL_IN_MS           10

// For testing use only. In production, set this equal to 1. Use this to exaggerate the acceleration effects. 10-20 is good for testing.
#define ACCELERATION_DELAY_OBVIOUSNESS_FACTOR        1

#define ANIMATION_FRAME_WIDTH     23
#define ANIMATION_FRAMES          28

// this is obviously a temporary hack, we don't want to have to specify constants for every single animation at the start, do we?

#define SPARKLE_ANIMATION_FRAME_WIDTH 23
#define SPARKLE_ANIMATION_FRAMES      46

#define ZOOMIE_ANIMATION_FRAME_WIDTH  200
#define ZOOMIE_ANIMATION_FRAMES       37

#define TIMINGTEST_ANIMATION_FRAME_WIDTH  58
#define TIMINGTEST_ANIMATION_FRAMES       26

// #define afc_3_pulse_ANIMATION_FRAME_WIDTH   87
// #define afc_3_pulse_ANIMATION_FRAMES   268




// ...TO HERE.

// Array representing the strip.
CRGB leds[NUM_LEDS];
// -------
// By defining a single element in memory which SHOULD BE immediately before the large pixel array,
// I've effectively created the (-1)th element of the array. This frees me from having to check
// that I'm writing pixels to that area and will unknowingly blow something up, and saves a costly
// comparison (which is false ALMOST always) each time through each Sprite's Update() method.
// TODO: Do this properly with malloc()/calloc().

CRGB colorSets[NUM_COLORSETS][NUM_COLORS_PER_SET];

char animationFramesChars[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB animationFrames[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

char animationFramesCharsReverse[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB animationFramesReverse[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

char afc_w8v1[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB af_w8v1[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

char afc_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB af_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

char afc_timing_test[TIMINGTEST_ANIMATION_FRAME_WIDTH * TIMINGTEST_ANIMATION_FRAMES];
CRGB af_timing_test[TIMINGTEST_ANIMATION_FRAME_WIDTH * TIMINGTEST_ANIMATION_FRAMES];

char afc_2_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];
CRGB af_2_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];

// char afc_3_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];
// CRGB af_3_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];

// char afc_zoomie_intro[ZOOMIE_ANIMATION_FRAME_WIDTH * ZOOMIE_ANIMATION_FRAMES];
// CRGB af_zoomie_intro[ZOOMIE_ANIMATION_FRAME_WIDTH * ZOOMIE_ANIMATION_FRAMES];
//
// char afc_3_pulse[afc_3_pulse_ANIMATION_FRAME_WIDTH * afc_3_pulse_ANIMATION_FRAMES];
// CRGB af_3_pulse[afc_3_pulse_ANIMATION_FRAME_WIDTH * afc_3_pulse_ANIMATION_FRAMES];

// Function prototypes.
void resetStrip(void);
void debug(int);
void debugNeg(int);
void debugN(int, int);
void stripcpy(CRGB *, CRGB *, int, int, int);
void createColorsets(void);
void createAnimationFrames(void);

class InfraredSensor {
private:
    int _pinNumber;
    uint32_t lastPollTime;

public:
    InfraredSensor(int pinNumber) {
        this->_pinNumber = pinNumber;
        this->lastPollTime = millis();

        pinMode(this->_pinNumber, INPUT);
    }

    // Put sensor read code here. Return true if triggered, false otherwise.
    bool IsActuated() {
        if (millis() - this->lastPollTime < INFRARED_SENSOR_TIMEOUT_IN_MS) {
            return false;
        }

        if (digitalRead(this->_pinNumber) == HIGH) {
            // Test pixel to indicate when the sensor's been actuated. Feel free to remove this when you like.
            // leds[29] = CRGB::Red;

            // Make sure these stay at the end of the sensor "if" block. This will set the last polling time to
            // ensure that the sensor is properly "debounced".
            this->lastPollTime = millis();
            return true;
        }

        return false;
    }
};

class Sprite {
  public:
    Sprite() {
      lastUpdateTime = 0;
      done = false;
    }

    virtual ~Sprite() {
    }

    virtual bool Update() = 0;

    boolean UpdateNow() {
      if (millis() - lastUpdateTime >= updateInterval) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    void MarkDone() {
        this->done = true;
    }

    bool IsDone() {
        return this->done;
    }

  protected:
    uint32_t lastUpdateTime;
    int updateInterval;
    boolean done;
};


class SpriteVector {
    private:
        Sprite **sprites;
        int maxCapacity;
        int count;

    public:
        SpriteVector(int maxCap) {
            this->count = 0;
            this->maxCapacity = maxCap;
            sprites = new Sprite*[maxCap];

            for (int i = 0; i < maxCap; i++) {
                sprites[i] = NULL;
            }
        }

        ~SpriteVector() {
        }

        Sprite *Get(int i) {
            if (i < this->count) {
                return sprites[i];
            } else {
                return NULL;
            }
        }

        boolean Add(Sprite *sprite) {
            if (count >= this->maxCapacity) {
                return false;
            }

            sprites[count] = sprite;
            ++count;

            return true;
        }

        int Count() {
            return this->count;
        }

        boolean RemoveAt(int i) {
            Sprite *ptr = sprites[i];
            sprites[i] = NULL;
            delete ptr;

            for (int j = i + 1; j < count; j++) {
                sprites[j - 1] = sprites[j];
            }
            sprites[count - 1] = NULL;

            --this->count;

            return true;
        }
};


// TODO: create new Sprite class with input functions for color (select which entry in the color array), acceleration factor (0-5; 1 means subtract 1ms per animation frame, ask Jim if this takex floats?), scanner min and max distance, start speed (like delay function), start position?

// TODO: maybe just create named sprites with different scanner patterns first?


  

class AnimationTestSprite : public Sprite {
  private:
    int updateInterval;
    int currentPixel;
    bool isScanning;
    int scanningFrame;
    int lastInflection;
    int nextInflection;
    int scanCount;
    int scanCountTotal;

    // pattern is one black pixel plus remaining pixels in order of increasing brightness with brightest pixel doubled.
    CRGB pattern[NUM_COLORS_PER_SET + 1];
    int patternLength = NUM_COLORS_PER_SET + 1;

    void SetNextInflection() {
        lastInflection = nextInflection;
        nextInflection += random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }

  public:
    AnimationTestSprite() : Sprite() {
        // Initial state.
        this->currentPixel = -8;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->lastInflection = 0;
        this->nextInflection = 0;
        SetNextInflection();
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();
        this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;

        // Choose a random color palette from the palettes available.
        int colorPalette = random(0, NUM_COLORSETS);

        // Set the colors in the pattern.
        this->pattern[0] = colorSets[colorPalette][0];
        this->pattern[1] = colorSets[colorPalette][1];
        this->pattern[2] = colorSets[colorPalette][2];
        this->pattern[3] = colorSets[colorPalette][3];
        this->pattern[4] = colorSets[colorPalette][4];
        this->pattern[5] = colorSets[colorPalette][5];
        this->pattern[6] = colorSets[colorPalette][6];
        this->pattern[7] = colorSets[colorPalette][7];
        this->pattern[8] = colorSets[colorPalette][8];
        this->pattern[9] = colorSets[colorPalette][8];

        this->patternLength = 10;

        for (int i = 0; i < ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES; i++) {
            afc_timing_test[i] = afc_timing_test[i] > ' ' ? colorSets[colorPalette][afc_timing_test[i] - '0'] : CRGB::Black;
        }
    }

    ~AnimationTestSprite() {
    }

    boolean UpdateNow() {
      if (millis() - lastUpdateTime >= ACCELERATION_DELAY_OBVIOUSNESS_FACTOR * updateInterval) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }

        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            currentPixel += 8;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = GetNewScanCountTotal();
            this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            leds[currentPixel - 6] = CRGB::Black;
            leds[currentPixel - 8] = CRGB::Black;
            leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel - 10] = CRGB::Black;
        }

        if (! isScanning) {
            // Traveling and continuing to travel.
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            ++currentPixel;

            // Are we nearer the last inflection than the next inflection? If so, speed up. Otherwise, slow down.
/*            int updateInterval = (currentPixel >= (lastInflection + nextInflection) / 2)
                                                      ? (updateInterval + ACCELERATION_RATE_IN_MS_PER_PIXEL)
                                                      : (updateInterval - ACCELERATION_RATE_IN_MS_PER_PIXEL); */
            if (currentPixel >= nextInflection - (SCANNER_DELAY_INTERVAL_IN_MS - 1)) {
                updateInterval += 1;
            } else {
                updateInterval -= 1;
            }

            if (updateInterval < 1) {
                updateInterval = 1;
            } else if (updateInterval > SPRITE_STARTING_DELAY_INTERVAL_IN_MS) {
                updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            }

            // Transition from travel mode to scanning.
            if (currentPixel >= nextInflection) {
                // Safety. Since I don't trust my math, once we enter scanning mode, ALWAYS go back to the constant speed for scanning
                // regardless of what the math said.
                updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 8;
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, af_timing_test + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                SetNextInflection();
            }
        }

        return true;
    }
};


class W8V1ScannerDebrisV1Sprite : public Sprite {
  private:
    int updateInterval;
    int currentPixel;
    bool isScanning;
    int scanningFrame;
    int lastInflection;
    int nextInflection;
    int scanCount;
    int scanCountTotal;

    // pattern is one black pixel plus remaining pixels in order of increasing brightness with brightest pixel doubled.
    CRGB pattern[NUM_COLORS_PER_SET + 1];
    int patternLength = NUM_COLORS_PER_SET + 1;

    void SetNextInflection() {
        lastInflection = nextInflection;
        nextInflection += random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }

  public:
    W8V1ScannerDebrisV1Sprite() : Sprite() {
        // Initial state.
        this->currentPixel = -8;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->lastInflection = 0;
        this->nextInflection = 0;
        SetNextInflection();
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();
        this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;

        // Choose a random color palette from the palettes available.
        int colorPalette = random(0, NUM_COLORSETS);

        // Set the colors in the pattern.
        this->pattern[0] = colorSets[colorPalette][0];
        this->pattern[1] = colorSets[colorPalette][1];
        this->pattern[2] = colorSets[colorPalette][2];
        this->pattern[3] = colorSets[colorPalette][3];
        this->pattern[4] = colorSets[colorPalette][4];
        this->pattern[5] = colorSets[colorPalette][5];
        this->pattern[6] = colorSets[colorPalette][6];
        this->pattern[7] = colorSets[colorPalette][7];
        this->pattern[8] = colorSets[colorPalette][8];
        this->pattern[9] = colorSets[colorPalette][8];

        this->patternLength = 10;

        for (int i = 0; i < ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES; i++) {
            af_w8v1[i] = afc_w8v1[i] > ' ' ? colorSets[colorPalette][afc_w8v1[i] - '0'] : CRGB::Black;
        }
    }

    ~W8V1ScannerDebrisV1Sprite() {
    }

    boolean UpdateNow() {
      if (millis() - lastUpdateTime >= ACCELERATION_DELAY_OBVIOUSNESS_FACTOR * updateInterval) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }

        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            currentPixel += 8;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = GetNewScanCountTotal();
            this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            leds[currentPixel - 6] = CRGB::Black;
            leds[currentPixel - 8] = CRGB::Black;
            leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel - 10] = CRGB::Black;
        }

        if (! isScanning) {
            // Traveling and continuing to travel.
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            ++currentPixel;

            // Are we nearer the last inflection than the next inflection? If so, speed up. Otherwise, slow down.
/*            int updateInterval = (currentPixel >= (lastInflection + nextInflection) / 2)
                                                      ? (updateInterval + ACCELERATION_RATE_IN_MS_PER_PIXEL)
                                                      : (updateInterval - ACCELERATION_RATE_IN_MS_PER_PIXEL); */
            if (currentPixel >= nextInflection - (SCANNER_DELAY_INTERVAL_IN_MS - 1)) {
                updateInterval += 1;
            } else {
                updateInterval -= 1;
            }

            if (updateInterval < 1) {
                updateInterval = 1;
            } else if (updateInterval > SPRITE_STARTING_DELAY_INTERVAL_IN_MS) {
                updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            }

            // Transition from travel mode to scanning.
            if (currentPixel >= nextInflection) {
                // Safety. Since I don't trust my math, once we enter scanning mode, ALWAYS go back to the constant speed for scanning
                // regardless of what the math said.
                updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 8;
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, af_w8v1 + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                SetNextInflection();
            }
        }

        return true;
    }
};


class W8V1ScannerDebrisV1ReverseSprite : public Sprite {
  private:
    int updateInterval;
    int currentPixel;
    bool isScanning;
    int scanningFrame;
    int lastInflection;
    int nextInflection;
    int scanCount;
    int scanCountTotal;
    int velocity;

    // pattern is one black pixel plus remaining pixels in order of increasing brightness with brightest pixel doubled.
    CRGB pattern[NUM_COLORS_PER_SET + 1];
    int patternLength = NUM_COLORS_PER_SET + 1;

    void SetNextInflection() {
        lastInflection = nextInflection;
        nextInflection -= random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }

  public:
    W8V1ScannerDebrisV1ReverseSprite() : Sprite() {
        // Initial state.
        this->currentPixel = NUM_LEDS + 7;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->lastInflection = NUM_LEDS;
        this->nextInflection = NUM_LEDS;
        SetNextInflection();
        this->velocity = 1;
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();
        this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;

        // Choose a random color palette from the palettes available.
        int colorPalette = random(0, NUM_COLORSETS);

        // Set the colors in the pattern.
        this->pattern[9] = colorSets[colorPalette][0];
        this->pattern[8] = colorSets[colorPalette][1];
        this->pattern[7] = colorSets[colorPalette][2];
        this->pattern[6] = colorSets[colorPalette][3];
        this->pattern[5] = colorSets[colorPalette][4];
        this->pattern[4] = colorSets[colorPalette][5];
        this->pattern[3] = colorSets[colorPalette][6];
        this->pattern[2] = colorSets[colorPalette][7];
        this->pattern[1] = colorSets[colorPalette][8];
        this->pattern[0] = colorSets[colorPalette][8];

        this->patternLength = 10;

        for (int i = 0; i < ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES; i++) {
            af_w8v1r[i] = afc_w8v1r[i] > ' ' ? colorSets[colorPalette][afc_w8v1r[i] - '0'] : CRGB::Black;
        }
    }

    ~W8V1ScannerDebrisV1ReverseSprite() {
    }

    boolean UpdateNow() {
      if (millis() - lastUpdateTime >= ACCELERATION_DELAY_OBVIOUSNESS_FACTOR * updateInterval) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }

        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            scanCount = 0;
            currentPixel += 2;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = GetNewScanCountTotal();
            this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            leds[currentPixel + 6] = CRGB::Black;
            leds[currentPixel + 8] = CRGB::Black;
            leds[currentPixel + 9] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel + 10] = CRGB::Black;
        }

        if (! isScanning) {
            // Traveling and continuing to travel.
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            currentPixel -= velocity;

            // Are we nearer the last inflection than the next inflection? If so, speed up. Otherwise, slow down.
            if (currentPixel <= nextInflection - (SCANNER_DELAY_INTERVAL_IN_MS - 1)) {
                updateInterval += 1;
            } else {
                updateInterval -= 1;
            }

            if (updateInterval < 1) {
                updateInterval = 1;
            } else if (updateInterval > SPRITE_STARTING_DELAY_INTERVAL_IN_MS) {
                updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            }

            if (currentPixel <= nextInflection) {
                // Safety. Since I don't trust my math, once we enter scanning mode, ALWAYS go back to the constant speed for scanning
                // regardless of what the math said.
                updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 3;
            }

            if (currentPixel <= -10) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, af_w8v1r + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                SetNextInflection();
            }
        }

        return true;
    }
};


class ScannerSprite : public Sprite {
  private:
    int currentPixel;
    bool isScanning;
    int scanningFrame;
    int nextInflection;
    int scanCount;
    int scanCountTotal;
    int velocity;

    // pattern is two black pixels plus remaining pixels in order of increasing brightness
    CRGB pattern[NUM_COLORS_PER_SET + 1];
    int patternLength = NUM_COLORS_PER_SET + 1;

    void SetNextInflection() {
        nextInflection += random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }

  public:
    ScannerSprite() : Sprite() {
        // Initial state.
        this->currentPixel = -2;  // Both of the first two pixels of the pattern are black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->nextInflection = 0;
        SetNextInflection();
        this->velocity = 2;
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();

        // Choose a random color palette from the palettes available.
        int colorPalette = random(0, NUM_COLORSETS);

        // Set the colors in the pattern.
        this->pattern[0] = this->pattern[1] = colorSets[colorPalette][0];
        for (int i = 1; i < NUM_COLORS_PER_SET; i++) {
            this->pattern[i + 1] = colorSets[colorPalette][i];
        }

        for (int i = 0; i < ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES; i++) {
            animationFrames[i] = animationFramesChars[i] > ' ' ? colorSets[colorPalette][animationFramesChars[i] - '0'] : CRGB::Black;
        }
    }

    ~ScannerSprite() {
    }

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }

        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            scanCount = 0;
            currentPixel += 8;
            SetNextInflection();
            scanCount = GetNewScanCountTotal();
            // leds[currentPixel - 20] = CRGB::White;
            leds[currentPixel - 6] = CRGB::Black;
            leds[currentPixel - 8] = CRGB::Black;
            leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel - 10] = CRGB::Black;
        }

        if (! isScanning) {
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            currentPixel += velocity;

            if (currentPixel >= nextInflection) {
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 8;
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, animationFrames + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                SetNextInflection();
            }
        }

        return true;
    }
};


class W1V1Sprite : public Sprite {
  private:
    int currentPixel;
    CRGB color;

  public:
    W1V1Sprite() {
        this->currentPixel = -1;
        this->color = CRGB::White;
    }

    W1V1Sprite(int startPixel, CRGB startColor) {
        // Minus one because we'll increment it as the first step in Update().
        this->currentPixel = startPixel - 1;
        this->color = startColor;
    }

    ~W1V1Sprite() {
    }

    boolean UpdateNow() {
      if (millis() - lastUpdateTime >= TEST_PATTERN_FRAME_DELAY_IN_MS) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    bool Update() {
      if (this->UpdateNow()) {
        currentPixel++;

        if (currentPixel >= NUM_LEDS) {
            leds[currentPixel - 1] = CRGB::Black;
            this->MarkDone();
        }

        leds[currentPixel] = this->color;
        if (currentPixel > 0) {
            leds[currentPixel - 1] = CRGB::Black;
        }

        return true;
      }

      return false;
    }
};

class SpriteManager {
  private:
    boolean updatedSomething = false;
    SpriteVector* spriteVector;

  public:
    SpriteManager() {
        spriteVector = new SpriteVector(MAXSPRITES);
    }

    ~SpriteManager() {
       // Don't bother. Should never be called.
    }

    int SpriteCount() {
      return spriteVector->Count();
    }

    void Update() {
        updatedSomething = false;

        for (int i = 0; i < this->SpriteCount(); i++) {
            updatedSomething |= spriteVector->Get(i)->Update();
        }

        if (updatedSomething) {
            FastLED.show();
        }

        this->Clean();
    }

    bool Add(Sprite *newSprite) {
      bool x = spriteVector->Add(newSprite);
      return x;
    }

    // Garbage collection. Remove any sprites that have finished their animation
    // from the SpriteVector, in order to make room for others.
    void Clean() {
        for (int i = this->SpriteCount() - 1; i >= 0; i--) {
            if (spriteVector->Get(i)->IsDone()) {
                spriteVector->RemoveAt(i);
            }
        }
    }
};

InfraredSensor *sensor1;
InfraredSensor *sensor2;

SpriteManager *spriteManager;

bool isBooted;
bool testSpritesCreated;

int starttime = millis();

void setup() {
    debug(1);
    createColorsets();
    debug(2);
    createAnimationFrames();
    debug(3);

    isBooted = false;
    testSpritesCreated = false;

    randomSeed(analogRead(0));

    spriteManager = new SpriteManager();

    sensor1 = new InfraredSensor(PIR_SENSOR_1_PIN);
    sensor2 = new InfraredSensor(PIR_SENSOR_2_PIN);

    resetStrip();
}

int counter = 0;
int sensor1LastPollTime = millis();

void loop() {
    if (! isBooted) {
        if (! testSpritesCreated) {
            spriteManager->Add(new W1V1Sprite(10, 0x750787));
            spriteManager->Add(new W1V1Sprite( 8, 0x004dff));
            spriteManager->Add(new W1V1Sprite( 6, 0x008026));
            spriteManager->Add(new W1V1Sprite( 4, 0xffed00));
            spriteManager->Add(new W1V1Sprite( 2, 0xff8c00));
            spriteManager->Add(new W1V1Sprite( 0, 0xe40303));

            testSpritesCreated = true;
        }

        spriteManager->Update();

        if (spriteManager->SpriteCount() == 0) {
            isBooted = true;
        }

        return;
    }

    // (A) JOSH: Remove this when you have the switches working to your heart's content.
/*    if (random(0, 1000) == 0) {
        Sprite *s = new W8V1ScannerDebrisV1Sprite();

        bool added = spriteManager->Add(s);

        if (! added) {
            delete s;
        }
    } */
    // End (A).

    if (sensor1->IsActuated()) {
        Sprite *s1 = new W8V1ScannerDebrisV1Sprite();

        if (! spriteManager->Add(s1)) {
            delete s1;
        }
    }

    if (sensor2->IsActuated()) {
        Sprite *s2 = new W8V1ScannerDebrisV1ReverseSprite();

        if (! spriteManager->Add(s2)) {
            delete s2;
        }
    }

    spriteManager->Update();
}


/* ---Utility functions --- */

void resetStrip() {
    FastLED.addLeds<NEOPIXEL, NEOPIXEL_DATA_PIN>(leds, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }

    FastLED.show();
}

void debug(int number) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_solid(leds, number < NUM_LEDS ? number : NUM_LEDS, 0x202020);
    FastLED.show();
}

void debugNeg(int number) {
    fill_solid(leds + NUM_LEDS - number, number < NUM_LEDS ? number : NUM_LEDS, 0x202020);
}

void debugN(int startPos, int number) {
    fill_solid(leds + startPos, number < (NUM_LEDS - startPos) ? number : NUM_LEDS - startPos, 0x202020);
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

    // This may be negative, which indicates we'd finish writing before we get to the start.
    int actualBytes = actualEndPosition - actualStartPosition;

    if (actualBytes > 0) {
        memcpy(leds + actualStartPosition, (patternStart <= patternSize) ? source + patternStart : source, actualBytes * sizeof(CRGB));
    }
}

void createColorsets() {
// Blue.
    colorSets[0][0] = CRGB::Black;
    colorSets[0][1] = 0x000000;
    colorSets[0][2] = 0x000000;
    colorSets[0][3] = 0x000001;
    colorSets[0][4] = 0x000003;
    colorSets[0][5] = 0x010106;
    colorSets[0][6] = 0x02020C;
    colorSets[0][7] = 0x050519;
    colorSets[0][8] = 0x0A0A33;

// Yellow-green.
#if NUM_COLORSETS > 1
    colorSets[1][0] = 0x000000;
    colorSets[1][1] = 0x000000;
    colorSets[1][2] = 0x000100;
    colorSets[1][3] = 0x010200;
    colorSets[1][4] = 0x030500;
    colorSets[1][5] = 0x060A01;
    colorSets[1][6] = 0x0D1503;
    colorSets[1][7] = 0x1B2A06;
    colorSets[1][8] = 0x36540C;
#endif

#if NUM_COLORSETS > 2
// Amber.
    colorSets[2][0] = 0x000000;
    colorSets[2][1] = 0x000000;
    colorSets[2][2] = 0x000000;
    colorSets[2][3] = 0x010100;
    colorSets[2][4] = 0x030300;
    colorSets[2][5] = 0x060601;
    colorSets[2][6] = 0x0C0C02;
    colorSets[2][7] = 0x191905;
    colorSets[2][8] = 0x33330A;
#endif

#if NUM_COLORSETS > 3
// Reddish-purple.
    colorSets[3][0] = 0x000000;
    colorSets[3][1] = 0x000000;
    colorSets[3][2] = 0x010001;
    colorSets[3][3] = 0x030102;
    colorSets[3][4] = 0x060204;
    colorSets[3][5] = 0x0C0508;
    colorSets[3][6] = 0x180A11;
    colorSets[3][7] = 0x311523;
    colorSets[3][8] = 0x632A47;
#endif

#if NUM_COLORSETS > 4
// Original purple.
    colorSets[4][0] = 0x000000;
    colorSets[4][1] = 0x000000;
    colorSets[4][2] = 0x010001;
    colorSets[4][3] = 0x030102;
    colorSets[4][4] = 0x060305;
    colorSets[4][5] = 0x0C060A;
    colorSets[4][6] = 0x180C14;
    colorSets[4][7] = 0x311828;
    colorSets[4][8] = 0x633051;
#endif
}

void createAnimationFrames() {
    strcpy(animationFramesChars, "          1234   ");
    strcat(animationFramesChars, "           4323  ");
    strcat(animationFramesChars, "         4321  2 ");
    strcat(animationFramesChars, "       4321     1");
    strcat(animationFramesChars, "     4321        ");
    strcat(animationFramesChars, "    3234         ");
    strcat(animationFramesChars, "   2  1234       ");
    strcat(animationFramesChars, "  1     1234     ");

    strcpy(animationFramesCharsReverse, "   4321          ");
    strcat(animationFramesCharsReverse, "  3234           ");
    strcat(animationFramesCharsReverse, " 2  1234         ");
    strcat(animationFramesCharsReverse, "1     1234       ");
    strcat(animationFramesCharsReverse, "        1234     ");
    strcat(animationFramesCharsReverse, "         4323    ");
    strcat(animationFramesCharsReverse, "       4321  2   ");
    strcat(animationFramesCharsReverse, "     4321     1  ");

//                    12345678901234567890123
    strcpy(afc_w8v1, "          123456788    ");
    strcat(afc_w8v1, "           12345688    ");
    strcat(afc_w8v1, "            12348876   ");
    strcat(afc_w8v1, "             128876 4  ");
    strcat(afc_w8v1, "              88765  2 ");
    strcat(afc_w8v1, "             887654   1");
    strcat(afc_w8v1, "            8876543    ");
    strcat(afc_w8v1, "           88765432    ");
    strcat(afc_w8v1, "          887654321    ");
    strcat(afc_w8v1, "         887654321     ");
    strcat(afc_w8v1, "        887654321      ");
    strcat(afc_w8v1, "       887654321       ");
    strcat(afc_w8v1, "      887654321        ");
    strcat(afc_w8v1, "     887654321         ");
    strcat(afc_w8v1, "    887654321          ");
    strcat(afc_w8v1, "    88654321           ");
    strcat(afc_w8v1, "   67884321            ");
    strcat(afc_w8v1, "  4 678821             ");
    strcat(afc_w8v1, " 2  56788              ");
    strcat(afc_w8v1, "1   456788             ");
    strcat(afc_w8v1, "    3456788            ");
    strcat(afc_w8v1, "    23456788           ");
    strcat(afc_w8v1, "    123456788          ");
    strcat(afc_w8v1, "     123456788         ");
    strcat(afc_w8v1, "      123456788        ");
    strcat(afc_w8v1, "       123456788       ");
    strcat(afc_w8v1, "        123456788      ");
    strcat(afc_w8v1, "         123456788     ");


//                     12345678901234567890123
    strcpy(afc_w8v1r, "    887654321          ");
    strcat(afc_w8v1r, "    88654321           ");
    strcat(afc_w8v1r, "   67884321            ");
    strcat(afc_w8v1r, "  4 678821             ");
    strcat(afc_w8v1r, " 2  56788              ");
    strcat(afc_w8v1r, "1   456788             ");
    strcat(afc_w8v1r, "    3456788            ");
    strcat(afc_w8v1r, "    23456788           ");
    strcat(afc_w8v1r, "    123456788          ");
    strcat(afc_w8v1r, "     123456788         ");
    strcat(afc_w8v1r, "      123456788        ");
    strcat(afc_w8v1r, "       123456788       ");
    strcat(afc_w8v1r, "        123456788      ");
    strcat(afc_w8v1r, "         123456788     ");
    strcat(afc_w8v1r, "          123456788    ");
    strcat(afc_w8v1r, "           12345688    ");
    strcat(afc_w8v1r, "            12348876   ");
    strcat(afc_w8v1r, "             128876 4  ");
    strcat(afc_w8v1r, "              88765  2 ");
    strcat(afc_w8v1r, "             887654   1");
    strcat(afc_w8v1r, "            8876543    ");
    strcat(afc_w8v1r, "           88765432    ");
    strcat(afc_w8v1r, "          887654321    ");
    strcat(afc_w8v1r, "         887654321     ");
    strcat(afc_w8v1r, "        887654321      ");
    strcat(afc_w8v1r, "       887654321       ");
    strcat(afc_w8v1r, "      887654321        ");
    strcat(afc_w8v1r, "     887654321         ");

    // TODO: a "reverse direction for animation" animation fragment to play to turn sprites going the other way back once, and gets lined up to play the "forwards" animation
    // (For example, if I want to animate a scanner but the animation loaded into memory starts going in the other direction, you can have the sprite reverse once, then spin around
    //  and execute that animation, which always ends with the sprite in the right position for frame 1 of the scanner animation. Which presumes they all start off in the same position on the same start pixel? Note to self.)

    strcpy(afc_timing_test, "123456788                                                 ");
    strcat(afc_timing_test, "1234567888                                                ");
    strcat(afc_timing_test, "  1234567888                                              ");
    strcat(afc_timing_test, "     1234567888                                           ");
    strcat(afc_timing_test, "       123456677888                                       ");
    strcat(afc_timing_test, "        1223344556667788                                  ");
    strcat(afc_timing_test, "         111122233344556667788                            ");
    strcat(afc_timing_test, "              11111222233344556667788                     ");
    strcat(afc_timing_test, "                   11111222333444555666788                ");
    strcat(afc_timing_test, "                          1111112223344556788             ");
    strcat(afc_timing_test, "                                 11111223456788           ");
    strcat(afc_timing_test, "                                     11123456788          ");
    strcat(afc_timing_test, "                                        1123467831        ");
    strcat(afc_timing_test, "                                            12468531      ");
    strcat(afc_timing_test, "                                             11386421     ");
    strcat(afc_timing_test, "                                              1287543211  ");
    strcat(afc_timing_test, "                                               18865432211");
    strcat(afc_timing_test, "                                               886443211  ");
    strcat(afc_timing_test, "                                              88765431    ");
    strcat(afc_timing_test, "                                            4887654321    ");
    strcat(afc_timing_test, "                                            4886421       ");
    strcat(afc_timing_test, "                                            138831        ");
    strcat(afc_timing_test, "                                            138831        ");
    strcat(afc_timing_test, "                                            138831        ");
    strcat(afc_timing_test, "                                            138831        ");
    strcat(afc_timing_test, "                                            138831        ");

    //                       12345678901234567890123
    strcpy(afc_2_sparkle_a, "123456788              ");
    strcat(afc_2_sparkle_a, " 123456788             ");
    strcat(afc_2_sparkle_a, "  123456788            ");
    strcat(afc_2_sparkle_a, "    12345677           ");
    strcat(afc_2_sparkle_a, "     1234566           ");
    strcat(afc_2_sparkle_a, "       123455          ");
    strcat(afc_2_sparkle_a, "        12344          ");
    strcat(afc_2_sparkle_a, "          1233         ");
    strcat(afc_2_sparkle_a, "           122         ");
    strcat(afc_2_sparkle_a, "             11        ");
    strcat(afc_2_sparkle_a, "                       ");
    strcat(afc_2_sparkle_a, "                1      ");
    strcat(afc_2_sparkle_a, "                1  1   ");
    strcat(afc_2_sparkle_a, "             1  21 2   ");
    strcat(afc_2_sparkle_a, "             1  1  3   ");
    strcat(afc_2_sparkle_a, "             21  1 4   ");
    strcat(afc_2_sparkle_a, "             31 11 5   ");
    strcat(afc_2_sparkle_a, "             42122 6   ");
    strcat(afc_2_sparkle_a, "             51213 7   ");
    strcat(afc_2_sparkle_a, "                       ");
    strcat(afc_2_sparkle_a, "           1 71414 2  1");
    strcat(afc_2_sparkle_a, "           2 51626271 1");
    strcat(afc_2_sparkle_a, "        1  1 42837422 1");
    strcat(afc_2_sparkle_a, "        2  2131428681  ");
    strcat(afc_2_sparkle_a, "          13222813472  ");
    strcat(afc_2_sparkle_a, "        2  2313428623 1");
    strcat(afc_2_sparkle_a, "       1 1 14 2833472 2");
    strcat(afc_2_sparkle_a, "        2   5 1628681 3");
    strcat(afc_2_sparkle_a, "           16  41342  2");
    strcat(afc_2_sparkle_a, "        2 127 122367  1");
    strcat(afc_2_sparkle_a, "        1 23811 4846   ");
    strcat(afc_2_sparkle_a, "       1  1262  6325  1");
    strcat(afc_2_sparkle_a, "       1   183  62 4  2");
    strcat(afc_2_sparkle_a, "        1   641 8123  3");
    strcat(afc_2_sparkle_a, "        2  185143242  4");
    strcat(afc_2_sparkle_a, "            64168161  5");
    strcat(afc_2_sparkle_a, "           185 8314   4");
    strcat(afc_2_sparkle_a, "           274 46 6   5");
    strcat(afc_2_sparkle_a, "           36218614   4");
    strcat(afc_2_sparkle_a, "           254244262  2");
    strcat(afc_2_sparkle_a, "           147382144  1");
    strcat(afc_2_sparkle_a, "           235261262   ");
    strcat(afc_2_sparkle_a, "           327141145   ");
    strcat(afc_2_sparkle_a, "           215 2 122   ");
    strcat(afc_2_sparkle_a, "           1 7   216   ");

//     // Zoomie Intro
//     //                        12345678901234567890123456789012345678901234560123456789012345678901234567890123456789012345601234567890123456789012345678901234567890123456012345678901234567890123456789012345678901234560123456789012
//     strcpy(afc_zoomie_intro, "123456788                                                                                                                                                                                               ");
//     strcat(afc_zoomie_intro, " 123456788                                                                                                                                                                                              ");
//     strcat(afc_zoomie_intro, "  123456788                                                                                                                                                                                             ");
//     strcat(afc_zoomie_intro, "   1 23456788                                                                                                                                                                                           ");
//     strcat(afc_zoomie_intro, "     1 23456788                                                                                                                                                                                         ");
//     strcat(afc_zoomie_intro, "       1 23456788                                                                                                                                                                                       ");
//     strcat(afc_zoomie_intro, "         1 2 3456788                                                                                                                                                                                    ");
//     strcat(afc_zoomie_intro, "           1 2  3456788                                                                                                                                                                                 ");
//     strcat(afc_zoomie_intro, "              1 2  3456788                                                                                                                                                                              ");
//     strcat(afc_zoomie_intro, "                 1 2  3 456788                                                                                                                                                                          ");
//     strcat(afc_zoomie_intro, "                    1 2  3  456788                                                                                                                                                                      ");
//     strcat(afc_zoomie_intro, "                       1 2  3   456788                                                                                                                                                                  ");
//     strcat(afc_zoomie_intro, "                           1 2  3   456788                                                                                                                                                              ");
//     strcat(afc_zoomie_intro, "                               1 2  3   456788                                                                                                                                                          ");
//     strcat(afc_zoomie_intro, "                                   1 2  3   4 56788                                                                                                                                                     ");
//     strcat(afc_zoomie_intro, "                                       1 2  3   4  56788                                                                                                                                                ");
//     strcat(afc_zoomie_intro, "                                           1 2  3   4   56788                                                                                                                                           ");
//     strcat(afc_zoomie_intro, "                                               1 2  3   4    56788                                                                                                                                      ");
//     strcat(afc_zoomie_intro, "                                                    1 2  3   4    5 6788                                                                                                                                ");
//     strcat(afc_zoomie_intro, "                                                         1 2  3   4    5  6788                                                                                                                          ");
//     strcat(afc_zoomie_intro, "                                                              1 2  3   4    5   6788                                                                                                                    ");
//     strcat(afc_zoomie_intro, "                                                                   1 2  3   4    5    6788                                                                                                              ");
//     strcat(afc_zoomie_intro, "                                                                        1 2  3   4    5     6788                                                                                                        ");
//     strcat(afc_zoomie_intro, "                                                                              1 2  3   4    5     6788                                                                                                  ");
//     strcat(afc_zoomie_intro, "                                                                                    1 2  3   4    5     6 788                                                                                           ");
//     strcat(afc_zoomie_intro, "                                                                                          1 2  3   4    5     6  788                                                                                    ");
//     strcat(afc_zoomie_intro, "                                                                                                1 2  3   4    5     6   788                                                                             ");
//     strcat(afc_zoomie_intro, "                                                                                                      1 2  3   4    5     6    788                                                                      ");
//     strcat(afc_zoomie_intro, "                                                                                                            1 2  3   4    5     6     788                                                               ");
//     strcat(afc_zoomie_intro, "                                                                                                                  1 2  3   4    5     6      788                                                        ");
//     strcat(afc_zoomie_intro, "                                                                                                                         1 2  3   4    5     6      7 88                                                ");
//     strcat(afc_zoomie_intro, "                                                                                                                                1 2  3   4    5     6      7  88                                        ");
//     strcat(afc_zoomie_intro, "                                                                                                                                       1 2  3   4    5     6      7   88                                ");
//     strcat(afc_zoomie_intro, "                                                                                                                                              1 2  3   4    5     6      7    88                        ");
//     strcat(afc_zoomie_intro, "                                                                                                                                                     1 2  3   4    5     6      7     88                ");
//     strcat(afc_zoomie_intro, "                                                                                                                                                            1 2  3   4    5     6      7      88        ");
//     strcat(afc_zoomie_intro, "                                                                                                                                                                   1 2  3   4    5     6      7       88");
//
//     // 3_pulse
// //                       123456789012345678901234567890123456789012345678901234567890123456789012345678901234567
//     strcpy(afc_3_pulse, "       88                                                                              ");
//     strcat(afc_3_pulse, "       88                                                                              ");
//     strcat(afc_3_pulse, "       77                                                                              ");
//     strcat(afc_3_pulse, "       66                                                                              ");
//     strcat(afc_3_pulse, "       55                                                                              ");
//     strcat(afc_3_pulse, "       44                                                                              ");
//     strcat(afc_3_pulse, "       44                                                                              ");
//     strcat(afc_3_pulse, "       44                                                                              ");
//     strcat(afc_3_pulse, "       55                                                                              ");
//     strcat(afc_3_pulse, "       66                                                                              ");
//     strcat(afc_3_pulse, "       77                                                                              ");
//     strcat(afc_3_pulse, "       88                                                                              ");
//     strcat(afc_3_pulse, "       88                                                                              ");
//     strcat(afc_3_pulse, "       788                                                                             ");
//     strcat(afc_3_pulse, "       788                                                                             ");
//     strcat(afc_3_pulse, "       788                                                                             ");
//     strcat(afc_3_pulse, "       6788                                                                            ");
//     strcat(afc_3_pulse, "       6788                                                                            ");
//     strcat(afc_3_pulse, "       56788                                                                           ");
//     strcat(afc_3_pulse, "       456788                                                                          ");
//     strcat(afc_3_pulse, "       23456788                                                                        ");
//     strcat(afc_3_pulse, "        123456788                                                                      ");
//     strcat(afc_3_pulse, "           123456788                                                                   ");
//     strcat(afc_3_pulse, "              123456788                                                                ");
//     strcat(afc_3_pulse, "                  123456788                                                            ");
//     strcat(afc_3_pulse, "                      123456788                                                        ");
//     strcat(afc_3_pulse, "                        123456788                                                      ");
//     strcat(afc_3_pulse, "                         123456788                                                     ");
//     strcat(afc_3_pulse, "                          123456788                                                    ");
//     strcat(afc_3_pulse, "                           12345688                                                    ");
//     strcat(afc_3_pulse, "                            1234588                                                    ");
//     strcat(afc_3_pulse, "                             123488                                                    ");
//     strcat(afc_3_pulse, "                              12388                                                    ");
//     strcat(afc_3_pulse, "                               1277                                                    ");
//     strcat(afc_3_pulse, "                                166                                                    ");
//     strcat(afc_3_pulse, "                                 55                                                    ");
//     strcat(afc_3_pulse, "                                 44                                                    ");
//     strcat(afc_3_pulse, "                                 44                                                    ");
//     strcat(afc_3_pulse, "                                 44                                                    ");
//     strcat(afc_3_pulse, "                                 55                                                    ");
//     strcat(afc_3_pulse, "                                 66                                                    ");
//     strcat(afc_3_pulse, "                                 77                                                    ");
//     strcat(afc_3_pulse, "                                 88                                                    ");
//     strcat(afc_3_pulse, "                                 88                                                    ");
//     strcat(afc_3_pulse, "                                 88                                                    ");
//     strcat(afc_3_pulse, "                                 77                                                    ");
//     strcat(afc_3_pulse, "                                 66                                                    ");
//     strcat(afc_3_pulse, "                                 55                                                    ");
//     strcat(afc_3_pulse, "                                 44                                                    ");
//     strcat(afc_3_pulse, "                                 44                                                    ");
//     strcat(afc_3_pulse, "                                 44                                                    ");
//     strcat(afc_3_pulse, "                                 55                                                    ");
//     strcat(afc_3_pulse, "                                 66                                                    ");
//     strcat(afc_3_pulse, "                                 77                                                    ");
//     strcat(afc_3_pulse, "                                 88                                                    ");
//     strcat(afc_3_pulse, "                                 88                                                    ");
//     strcat(afc_3_pulse, "                                887                                                    ");
//     strcat(afc_3_pulse, "                                887                                                    ");
//     strcat(afc_3_pulse, "                               8876                                                    ");
//     strcat(afc_3_pulse, "                              88765                                                    ");
//     strcat(afc_3_pulse, "                            8876543                                                    ");
//     strcat(afc_3_pulse, "                         887654321                                                     ");
//     strcat(afc_3_pulse, "                     887654321                                                         ");
//     strcat(afc_3_pulse, "                   887654321                                                           ");
//     strcat(afc_3_pulse, "                  887654321                                                            ");
//     strcat(afc_3_pulse, "                  88654321                                                             ");
//     strcat(afc_3_pulse, "                  8854321                                                              ");
//     strcat(afc_3_pulse, "                  774321                                                               ");
//     strcat(afc_3_pulse, "                  66321                                                                ");
//     strcat(afc_3_pulse, "                  5521                                                                 ");
//     strcat(afc_3_pulse, "                  441                                                                  ");
//     strcat(afc_3_pulse, "                  44                                                                   ");
//     strcat(afc_3_pulse, "                  44                                                                   ");
//     strcat(afc_3_pulse, "                  55                                                                   ");
//     strcat(afc_3_pulse, "                  66                                                                   ");
//     strcat(afc_3_pulse, "                  77                                                                   ");
//     strcat(afc_3_pulse, "                  88                                                                   ");
//     strcat(afc_3_pulse, "                  88                                                                   ");
//     strcat(afc_3_pulse, "                  88                                                                   ");
//     strcat(afc_3_pulse, "                  788                                                                  ");
//     strcat(afc_3_pulse, "                  788                                                                  ");
//     strcat(afc_3_pulse, "                  6788                                                                 ");
//     strcat(afc_3_pulse, "                  456788                                                               ");
//     strcat(afc_3_pulse, "                  123456788                                                            ");
//     strcat(afc_3_pulse, "                      123456788                                                        ");
//     strcat(afc_3_pulse, "                          123456788                                                    ");
//     strcat(afc_3_pulse, "                              123456788                                                ");
//     strcat(afc_3_pulse, "                                 123456788                                             ");
//     strcat(afc_3_pulse, "                                   123456788                                           ");
//     strcat(afc_3_pulse, "                                    123456788                                          ");
//     strcat(afc_3_pulse, "                                     12345688                                          ");
//     strcat(afc_3_pulse, "                                      1234588                                          ");
//     strcat(afc_3_pulse, "                                       123477                                          ");
//     strcat(afc_3_pulse, "                                        12366                                          ");
//     strcat(afc_3_pulse, "                                         1255                                          ");
//     strcat(afc_3_pulse, "                                          144                                          ");
//     strcat(afc_3_pulse, "                                           44                                          ");
//     strcat(afc_3_pulse, "                                           44                                          ");
//     strcat(afc_3_pulse, "                                           55                                          ");
//     strcat(afc_3_pulse, "                                           66                                          ");
//     strcat(afc_3_pulse, "                                           77                                          ");
//     strcat(afc_3_pulse, "                                           88                                          ");
//     strcat(afc_3_pulse, "                                           88                                          ");
//     strcat(afc_3_pulse, "                                           88                                          ");
//     strcat(afc_3_pulse, "                                           77                                          ");
//     strcat(afc_3_pulse, "                                           66                                          ");
//     strcat(afc_3_pulse, "                                           55                                          ");
//     strcat(afc_3_pulse, "                                           44                                          ");
//     strcat(afc_3_pulse, "                                           44                                          ");
//     strcat(afc_3_pulse, "                                           44                                          ");
//     strcat(afc_3_pulse, "                                           55                                          ");
//     strcat(afc_3_pulse, "                                           66                                          ");
//     strcat(afc_3_pulse, "                                           77                                          ");
//     strcat(afc_3_pulse, "                                           88                                          ");
//     strcat(afc_3_pulse, "                                           88                                          ");
//     strcat(afc_3_pulse, "                                           88                                          ");
//     strcat(afc_3_pulse, "                                          887                                          ");
//     strcat(afc_3_pulse, "                                        88765                                          ");
//     strcat(afc_3_pulse, "                                     88765432                                          ");
//     strcat(afc_3_pulse, "                                 887654321                                             ");
//     strcat(afc_3_pulse, "                              887654321                                                ");
//     strcat(afc_3_pulse, "                             887654321                                                 ");
//     strcat(afc_3_pulse, "                            887654321                                                  ");
//     strcat(afc_3_pulse, "                            88654321                                                   ");
//     strcat(afc_3_pulse, "                            8854321                                                    ");
//     strcat(afc_3_pulse, "                            774321                                                     ");
//     strcat(afc_3_pulse, "                            66321                                                      ");
//     strcat(afc_3_pulse, "                            5521                                                       ");
//     strcat(afc_3_pulse, "                            441                                                        ");
//     strcat(afc_3_pulse, "                            33                                                         ");
//     strcat(afc_3_pulse, "                           1441                                                        ");
//     strcat(afc_3_pulse, "                           2552                                                        ");
//     strcat(afc_3_pulse, "                           4664                                                        ");
//     strcat(afc_3_pulse, "                          157751                                                       ");
//     strcat(afc_3_pulse, "                          268862                                                       ");
//     strcat(afc_3_pulse, "                          368863                                                       ");
//     strcat(afc_3_pulse, "                         13688631                                                      ");
//     strcat(afc_3_pulse, "                         13688631                                                      ");
//     strcat(afc_3_pulse, "                          368863                                                       ");
//     strcat(afc_3_pulse, "                          268862                                                       ");
//     strcat(afc_3_pulse, "                          157751                                                       ");
//     strcat(afc_3_pulse, "                           4664                                                        ");
//     strcat(afc_3_pulse, "                           2552                                                        ");
//     strcat(afc_3_pulse, "                           1441                                                        ");
//     strcat(afc_3_pulse, "                            44                                                         ");
//     strcat(afc_3_pulse, "                           1441                                                        ");
//     strcat(afc_3_pulse, "                           2552                                                        ");
//     strcat(afc_3_pulse, "                           4664                                                        ");
//     strcat(afc_3_pulse, "                          157751                                                       ");
//     strcat(afc_3_pulse, "                          268862                                                       ");
//     strcat(afc_3_pulse, "                          368863                                                       ");
//     strcat(afc_3_pulse, "                         13688631                                                      ");
//     strcat(afc_3_pulse, "                         13688631                                                      ");
//     strcat(afc_3_pulse, "                          368863                                                       ");
//     strcat(afc_3_pulse, "                          268862                                                       ");
//     strcat(afc_3_pulse, "                          157751                                                       ");
//     strcat(afc_3_pulse, "                           4664                                                        ");
//     strcat(afc_3_pulse, "                           2552                                                        ");
//     strcat(afc_3_pulse, "                           2552                                                        ");
//     strcat(afc_3_pulse, "                           1661                                                        ");
//     strcat(afc_3_pulse, "                            77                                                         ");
//     strcat(afc_3_pulse, "                            88                                                         ");
//     strcat(afc_3_pulse, "                            88                                                         ");
//     strcat(afc_3_pulse, "                            88                                                         ");
//     strcat(afc_3_pulse, "                            788                                                        ");
//     strcat(afc_3_pulse, "                            788                                                        ");
//     strcat(afc_3_pulse, "                            788                                                        ");
//     strcat(afc_3_pulse, "                            6788                                                       ");
//     strcat(afc_3_pulse, "                            6788                                                       ");
//     strcat(afc_3_pulse, "                            56788                                                      ");
//     strcat(afc_3_pulse, "                            456788                                                     ");
//     strcat(afc_3_pulse, "                            23456788                                                   ");
//     strcat(afc_3_pulse, "                             123456788                                                 ");
//     strcat(afc_3_pulse, "                                123456788                                              ");
//     strcat(afc_3_pulse, "                                   123456788                                           ");
//     strcat(afc_3_pulse, "                                       123456788                                       ");
//     strcat(afc_3_pulse, "                                            123456788                                  ");
//     strcat(afc_3_pulse, "                                                  123456788                            ");
//     strcat(afc_3_pulse, "                                                        123456788                      ");
//     strcat(afc_3_pulse, "                                                              123456788                ");
//     strcat(afc_3_pulse, "                                                                   123456788           ");
//     strcat(afc_3_pulse, "                                                                       123456788       ");
//     strcat(afc_3_pulse, "                                                                          123456788    ");
//     strcat(afc_3_pulse, "                                                                            123456788  ");
//     strcat(afc_3_pulse, "                                                                             123456788 ");
//     strcat(afc_3_pulse, "                                                                              12345688 ");
//     strcat(afc_3_pulse, "                                                                                1234588");
//     strcat(afc_3_pulse, "                                                                                 123488");
//     strcat(afc_3_pulse, "                                                                                  12388");
//     strcat(afc_3_pulse, "                                                                                   1277");
//     strcat(afc_3_pulse, "                                                                                    166");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     77");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     77");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     77");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     77");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     33");
//     strcat(afc_3_pulse, "                                                                                     11");
//     strcat(afc_3_pulse, "                                                                                     33");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     77");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     88");
//     strcat(afc_3_pulse, "                                                                                     77");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     44");
//     strcat(afc_3_pulse, "                                                                                     55");
//     strcat(afc_3_pulse, "                                                                                     66");
//     strcat(afc_3_pulse, "                                                                                     77");
    // strcat(afc_3_pulse, "                                                                                     88");
    // strcat(afc_3_pulse, "                                                                                     88");
    // strcat(afc_3_pulse, "                                                                                     88");
    // strcat(afc_3_pulse, "                                                                                    887");
    // strcat(afc_3_pulse, "                                                                                    887");
    // strcat(afc_3_pulse, "                                                                                    887");
    // strcat(afc_3_pulse, "                                                                                   8876");
    // strcat(afc_3_pulse, "                                                                                   8876");
    // strcat(afc_3_pulse, "                                                                                  88765");
    // strcat(afc_3_pulse, "                                                                                 887654");
    // strcat(afc_3_pulse, "                                                                              887654321");
    // strcat(afc_3_pulse, "                                                                           887654321   ");
    // strcat(afc_3_pulse, "                                                                       887654321       ");
    // strcat(afc_3_pulse, "                                                                  887654321            ");
    // strcat(afc_3_pulse, "                                                            887654321                  ");
    // strcat(afc_3_pulse, "                                                     887654321                         ");
    // strcat(afc_3_pulse, "                                               8876654321                              ");
    // strcat(afc_3_pulse, "                                         887654321                                     ");
    // strcat(afc_3_pulse, "                                   887654321                                           ");
    // strcat(afc_3_pulse, "                              887654321                                                ");
    // strcat(afc_3_pulse, "                         887654321                                                     ");
    // strcat(afc_3_pulse, "                    887654321                                                          ");
    // strcat(afc_3_pulse, "                887654321                                                              ");
    // strcat(afc_3_pulse, "             887654321                                                                 ");
    // strcat(afc_3_pulse, "           887654321                                                                   ");
    // strcat(afc_3_pulse, "          887654321                                                                    ");
    // strcat(afc_3_pulse, "         887654321                                                                     ");
    // strcat(afc_3_pulse, "         88654321                                                                      ");
    // strcat(afc_3_pulse, "        8854321                                                                        ");
    // strcat(afc_3_pulse, "        884321                                                                         ");
    // strcat(afc_3_pulse, "        88321                                                                          ");
    // strcat(afc_3_pulse, "        8821                                                                           ");
    // strcat(afc_3_pulse, "       881                                                                             ");
    // strcat(afc_3_pulse, "       88                                                                              ");


}
