#include <FastLED.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define NUM_LEDS            750
#define MAXSPRITES            20

#define NUM_COLORSETS         5
#define NUM_COLORS_PER_SET    9

#define PIR_SENSOR_1_PIN     3
#define PIR_SENSOR_2_PIN     4
#define NEOPIXEL_DATA_PIN    6                // Pin for neopixels

#define INFRARED_SENSOR_TIMEOUT_IN_MS   1000  // Ten seconds.

#define SCANNER_SPRITE_FRAME_DELAY_IN_MS     1
#define TEST_PATTERN_FRAME_DELAY_IN_MS       1

#define SCANNER_MIN_SCANS    6
#define SCANNER_MAX_SCANS    8

// lurker sprite constants

#define BLINK_SPRITE_MIN_LIFETIME		3
#define BLINK_SPRITE_MAX_LIFETIME		12		// # of blinks before dying

#define BLINK_SPRITE_MAX_BLINK_SPEED	5		// ms interval between updates, lower is faster
#define BLINK_SPRITE_MIN_BLINK_SPEED	45

#define BLINK_MIN_COUNT		1
#define BLINK_MAX_COUNT		4			

#define LURKER_BLINK_MIN_FREQUENCY	2500
#define LURKER_BLINK_MAX_FREQUENCY	6000

#define LURKER_MIN_PIXEL_1  100
#define LURKER_MAX_PIXEL_1  150
#define LURKER_MIN_PIXEL_2  260
#define LURKER_MAX_PIXEL_2  310
#define LURKER_MIN_PIXEL_3	400
#define LURKER_MAX_PIXEL_3	450

// TreeSprite locations
#define TREE_FADE_PIXEL_1	315
#define TREE_START_1		323
#define TREE_END_1			334

#define TREE_FADE_PIXEL_2	315
#define TREE_START_2		323
#define TREE_END_2			334

#define TREE_FADE_PIXEL_3	315
#define TREE_START_3		323
#define TREE_END_3			334

#define TREE_FADE_PIXEL_4	315
#define TREE_START_4		323
#define TREE_END_4			334

#define TREE_FADE_PIXEL_5	315
#define TREE_START_5		323
#define TREE_END_5			334



// currently set this to be consistent for animation design
#define SCANNER_MIN_STOP_DISTANCE    35   // This probably shouldn't be smaller than 40. If it is scanners may get stuck in place if they don't have enough "exit velocity". // 40
#define SCANNER_MAX_STOP_DISTANCE    60   // 120

#define SPRITE_STARTING_DELAY_INTERVAL_IN_MS   40 // 40
#define SCANNER_DELAY_INTERVAL_IN_MS           20

// For testing use only. In production, set this equal to 1. Use this to exaggerate the acceleration effects. 10-20 is good for testing.
#define ACCELERATION_DELAY_OBVIOUSNESS_FACTOR        1

#define ANIMATION_FRAME_WIDTH     23
#define ANIMATION_FRAMES          28

// this is obviously a temporary hack, we don't want to have to specify constants for every single animation at the start, do we?

#define SPARKLE_ANIMATION_FRAME_WIDTH 23
#define SPARKLE_ANIMATION_FRAMES      46

#define TIMINGTEST_ANIMATION_FRAME_WIDTH  58
#define TIMINGTEST_ANIMATION_FRAMES       26

#define afc_f_slow_stop_ANIMATION_FRAME_WIDTH   17
#define afc_f_slow_stop_ANIMATION_FRAMES        65

#define afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH 25
#define afc_f_slow_stop_c_ANIMATION_FRAMES      82

#define afc_l_pulsar_a_ANIMATION_FRAME_WIDTH    24
#define afc_l_pulsar_a_ANIMATION_FRAMES         22

#define afc_l_mother_ANIMATION_FRAME_WIDTH      3
#define afc_l_mother_ANIMATION_FRAMES           10

#define afc_f_eye_full_a_ANIMATION_FRAME_WIDTH 	21
#define afc_f_eye_full_a_ANIMATION_FRAMES 		58

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

// char animationFramesChars[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
// CRGB animationFrames[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

// char animationFramesCharsReverse[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
// CRGB animationFramesReverse[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

// char afc_w8v1[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
// CRGB af_w8v1[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

char afc_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB af_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

// char afc_timing_test[TIMINGTEST_ANIMATION_FRAME_WIDTH * TIMINGTEST_ANIMATION_FRAMES];
// CRGB af_timing_test[TIMINGTEST_ANIMATION_FRAME_WIDTH * TIMINGTEST_ANIMATION_FRAMES];

// char afc_2_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];
// CRGB af_2_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];

char afc_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];
CRGB af_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];

char afc_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];
CRGB af_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];

char afc_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];
CRGB af_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];


char afc_l_mother[afc_l_mother_ANIMATION_FRAME_WIDTH * afc_l_mother_ANIMATION_FRAMES];
CRGB af_l_mother[afc_l_mother_ANIMATION_FRAME_WIDTH * afc_l_mother_ANIMATION_FRAMES];

char afc_f_eye_full_a[afc_f_eye_full_a_ANIMATION_FRAME_WIDTH * afc_f_eye_full_a_ANIMATION_FRAMES];
CRGB af_f_eye_full_a[afc_f_eye_full_a_ANIMATION_FRAME_WIDTH * afc_f_eye_full_a_ANIMATION_FRAMES];

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

    bool allowCreation() {
        return true;    // Always true, no reason to veto this one.
    }

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

// TODO: sprite creation

// I want to declare a BlinkingEyesSprite and declare which animation fragments it uses, what its "loop" animation is, what the sprite animation is, its starting interval and its target interval when it reaches a destination with an UpdateTravel move. Also its travel fadeinterval and animation fadeinterval. 
// Use ifdef to call animation char and CRGB structs into memory at compile time


// TODO: Animation Experiments!

// Animation experiment classes start here.

// First working animation test class. 
/*
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
        this->scanCountTotal = 1;
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

        for (int i = 0; i < TIMINGTEST_ANIMATION_FRAME_WIDTH * TIMINGTEST_ANIMATION_FRAMES; i++) {
            af_timing_test[i] = afc_timing_test[i] > ' ' ? colorSets[colorPalette][afc_timing_test[i] - '0'] : CRGB::Black;
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
            currentPixel += TIMINGTEST_ANIMATION_FRAME_WIDTH; // 8
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = 1;
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
                currentPixel -= 0; // -8 normally
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, af_timing_test + TIMINGTEST_ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, TIMINGTEST_ANIMATION_FRAME_WIDTH, TIMINGTEST_ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == TIMINGTEST_ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                // SetNextInflection();
            }
        }

        return true;
    }
};
*/



// TODO: State machine blink sprites that spawn in defined areas when TravelSprites pass through them. "Spirits awaken" 

// What I'm going for here: I want to put these in shadowed places, where our primate brain will be looking for glowing eyes in the night. I want them to awaken, and blink, and maybe shift a little, and blink. 
class LurkerSprite : public Sprite {
private:
    int updateInterval;
    int currentPixel;
	int lifetimeBlinks; // # of eyeblinks before sprite vanishes
	int blinkCount;
	int blinkFrequency;
	int eyeWidth;	// spacing between eyes, minimum is 1, set this higher the further away you are locating the sprite from the viewer
	int blinkDirection;	// if in blink mode, are we closing or opening? -1 for closing, +1 for opening, 0 for staring
	int eyeColor;			// entry in color array
	int eyeMaxColor;		// how bright should we go?
	int blinkMaxCount;		// how many blinks in a "set" of blinks?
	int blinkTiming;		// how many ms between each blink in a "set"? 
	unsigned long lastBlinkTime;	// time when we last blinked
	int colorPalette;

	// We're defining a set of either 2, 4, or 6 pixels, depending on how far apart you want the eyes to be
	// ...so, should this be set to eyewidth + 1?
	CRGB eyes[2];
	int eyeLength = 2;
	
	int SetLifeSpan() {
		// debug(2);
		return random(BLINK_SPRITE_MIN_LIFETIME, BLINK_SPRITE_MAX_LIFETIME + 1);
	}
	
	int SetInitialBlinkSpeed() {
		// debug(3);
		return random(BLINK_SPRITE_MAX_BLINK_SPEED, BLINK_SPRITE_MIN_BLINK_SPEED + 1);
	}
	
	int	SetBlinkMaxCount() {
		// debug(4);
		return random(BLINK_MIN_COUNT,BLINK_MAX_COUNT + 1);

	}
	
	int SetBlinkTiming() {
		// debug(5);
		return random(200,1000); // ms; testing to see what looks good, these are rough guesses
	}
	

	
	int SetBlinkFrequency() {
		return random(LURKER_BLINK_MIN_FREQUENCY, LURKER_BLINK_MAX_FREQUENCY +1);
	}
	
	void SpawnBlinkChild() {
		// debug(6);
		// random chance to spawn a new blink sprite with a reduced lifespan
	}
	
	// TODO: blink in multiple sets
	int SetBlinkDirection() {
		// if 0:
		// check to see if eyecolor = ??maxeyecolor??; if so, we're starting a blink.
			// Check to see if millis - lastBlinkTime is over the blinkFrequency. 
			// if so, return -1, else 0
		// check to see if eyecolor = 0, if so, we're mid-blink.
		// check if millis - lastBlinkTime >= blinkTiming 
			// if so, return +1, else 0
		
		if (blinkDirection == 0) {
			if (eyeColor == eyeMaxColor) {
				// we are staring.
				// check to see if we need to blink
				if (millis() - lastBlinkTime >= blinkFrequency) {
					lastBlinkTime = millis();
					blinkFrequency = SetBlinkFrequency();	// reset blinkFrequency after every blink?
					return -1;
					// make this a random chance that gets more likely over time, based on blinkFrequency?
				} else {
					return 0;
				}
			} else if (eyeColor == 0) {
				// we are mid-blink
				// check to see if our lifespan is up, and if so, mark done and exit
				if (blinkCount >= lifetimeBlinks) {
					this->MarkDone();
					SpawnBlinkChild();
					return 0;
				}
				// check to see if it's time to open our eyes
				if (millis() - lastBlinkTime >= blinkTiming) {
					// eyes have been closed long enough
					return 1;
				} else {
					// eyes are staying closed a little longer
					return 0;
				}
			}
		}
		
		// if -1:
		// Check to see if eyeColor = 0
		// if eyecolor > 0, return -1
		
		if (blinkDirection == -1) {
			// have we reached the end of the blink?
			if (eyeColor <= 0) {
				// if eyeColor went negative, fix that
				eyeColor = 0;
				return 0;
			} else {
				// if not, keep blinking
				return -1;
			}
		}
		
		// if 1:
		// Check if eyecolor == max, if so, return 0
		// 
		if (blinkDirection == 1) {
			if (eyeColor >= eyeMaxColor) {
				// we have just completed a single blink
				// add a blink to our lifetime count
				blinkCount ++;
				// and record the time as the end of the last blink
				lastBlinkTime = millis();
				// and stop blinking
				return 0;
			} else {
				return 1;
			}
		} else {
			debug(7);
			return 0; // if blinkDirection's value gets messed up somehow and is not -1, 0, or 1, just set it to 0 for now. 
		}
	}
	
public:
    LurkerSprite(int spawnPixel, int eyew) : Sprite() {
        // Initial state.
        this->currentPixel = spawnPixel;  // OK, so I want to set this to a random between factors (RBF) value based on the value of currentPixel in a passing TravelSprite when it passes through areas where BlinkSprites can "awaken". So... I set a method here, right? AwakenAtPixel()? I'm assuming we can hand a BlinkSprite off the value of currentPixel at the time of spawn from the sprite that "woke" it? 
        this->updateInterval = SetInitialBlinkSpeed();
		this->lifetimeBlinks = SetLifeSpan(); 
		this->blinkCount = -1;							// When first opening the eyes, it "counts" as a blink, and setting to -1 means we don't count it against lifespan
		this->blinkFrequency = SetBlinkFrequency(); 	// decent test with 4000 ms, trying a random set
        this->eyeWidth = eyew;							// can we set this on spawn? Make it semi-random within params? 
		this->blinkDirection = 1;						// want to start with eyes closed and open them
		this->eyeColor = 0;								// eyes closed = 0
		this->eyeMaxColor = 5;							// up to 5 in the color set
		this->blinkMaxCount = SetBlinkMaxCount();
		this->blinkTiming = SetBlinkTiming();
		this->lastBlinkTime = millis();
		
		// int colorPalette = random(0, NUM_COLORSETS);
		int colorPalette = 2; 							// yellow to start

		// TODO: fix eye color lookup; it's not drawing because it's not pulling correctly from colorsets? 

		// this->eyes[0] = colorSets[colorPalette][eyeColor];
// 		this->eyes[eyeWidth] = colorSets[colorPalette][eyeColor];

		this->eyes[0] = colorSets[2][eyeColor];
		this->eyes[eyeWidth] = colorSets[2][eyeColor];
		
        this->eyeLength = 2;
    }

    ~LurkerSprite() {
    }
	
    bool allowCreation() {
        return (currentPixel >= LURKER_MIN_PIXEL_1 && currentPixel <= LURKER_MAX_PIXEL_1) || (currentPixel >= LURKER_MIN_PIXEL_2 && currentPixel <= LURKER_MAX_PIXEL_2) || (currentPixel >= LURKER_MIN_PIXEL_3 && currentPixel <= LURKER_MAX_PIXEL_3);
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
		// debug(eyeColor);
        if (! this->UpdateNow()) {
            return false;
        }
		
		// Decide if we're going to blink, and set the value to do it
		blinkDirection = SetBlinkDirection();
		
		// close or open the eyes a step by adding blinkDirection to eye color
		eyeColor += blinkDirection;
		
		// so when we use colorPalette to call the color set, we don't get a value returned?
		// TODO: figure this out
		// this->eyes[0] = 0x020202; // colorSets[colorPalette][eyeColor];
		this->eyes[0] = colorSets[2][eyeColor];
		this->eyes[eyeWidth] = colorSets[2][eyeColor]; //colorSets[colorPalette][eyeColor];
		
		stripcpy(leds, eyes, currentPixel, eyeLength, eyeLength);
		return true;
	}
};


// Traveling Eye Sprite Test

class TravelEyeTestSprite : public Sprite {
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
    TravelEyeTestSprite() : Sprite() {
        // Initial state.
        this->currentPixel = -8;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->lastInflection = 0;
        this->nextInflection = 0;
        SetNextInflection();
        this->scanCount = 0;
        // this->scanCountTotal = GetNewScanCountTotal();
		this->scanCountTotal = 1;
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

        for (int i = 0; i < afc_f_eye_full_a_ANIMATION_FRAME_WIDTH * afc_f_eye_full_a_ANIMATION_FRAMES; i++) {
            af_f_eye_full_a[i] = afc_f_eye_full_a[i] > ' ' ? colorSets[colorPalette][afc_f_eye_full_a[i] - '0'] : CRGB::Black;
        }
    }

    ~TravelEyeTestSprite() {
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
		
		// debug(nextInflection);
        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            currentPixel += afc_f_eye_full_a_ANIMATION_FRAME_WIDTH;
			currentPixel -= 8;
            SetNextInflection();
            this->scanCount = 0;
            // this->scanCountTotal = GetNewScanCountTotal(); // set to 1 for fragments
			this->scanCountTotal = 1;
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
                // updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
				updateInterval = 60;
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 0;
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, af_f_eye_full_a + afc_f_eye_full_a_ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, afc_f_eye_full_a_ANIMATION_FRAME_WIDTH, afc_f_eye_full_a_ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == afc_f_eye_full_a_ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                // SetNextInflection();
            }
        }

        return true;
    }
};

// Loop test class
// Travel, pause, play loop 2-5 times, move on

class LoopTestSprite : public Sprite {
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
    LoopTestSprite() : Sprite() {
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

        for (int i = 0; i < afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES; i++) {
            af_l_pulsar_a[i] = afc_l_pulsar_a[i] > ' ' ? colorSets[colorPalette][afc_l_pulsar_a[i] - '0'] : CRGB::Black;
        }
    }

    ~LoopTestSprite() {
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
		
		// debug(nextInflection);
        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            currentPixel += 8;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = GetNewScanCountTotal(); // set to 1 for fragments
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
            stripcpy(leds, af_l_pulsar_a + afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == afc_l_pulsar_a_ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                // SetNextInflection();
            }
        }

        return true;
    }
};

class MotherSprite : public Sprite {
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
    MotherSprite() : Sprite() {
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

        for (int i = 0; i < afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES; i++) {
            af_l_pulsar_a[i] = afc_l_pulsar_a[i] > ' ' ? colorSets[colorPalette][afc_l_pulsar_a[i] - '0'] : CRGB::Black;
        }
    }

    ~MotherSprite() {
    }

    boolean UpdateNow() {
      if (millis() - lastUpdateTime >= ACCELERATION_DELAY_OBVIOUSNESS_FACTOR * updateInterval) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    bool DimColor() {
        // leds[i].fadeToBlackBy( 128 ); // Fade completely to black = 256; 128/256 = .5, so this fades by half

    }

    bool MoveToLocation(int dest, int accel) {
        // move to a destination with an acceleration factor
    }

    bool StartTravel() {
        // Transition from loop mode to travel mode

    }

    bool UpdateTravel() {
        // In travel mode, move forwards
    }

    bool StartLoop() {
        // Transition from travel mode to loop mode
    }

    bool UpdateLoop() {

    }

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }
        
        // debug(nextInflection);
        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            currentPixel += 8;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = GetNewScanCountTotal(); // set to 1 for fragments
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
            stripcpy(leds, af_l_pulsar_a + afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == afc_l_pulsar_a_ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                // SetNextInflection();
            }
        }

        return true;
    }
};

// Test intro and outro fragments together until I like them, then split into two halves and adjust code to play one, a loop, and then the other. 
class FragmentTestSprite : public Sprite {
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
    FragmentTestSprite() : Sprite() {
        // Initial state.
        this->currentPixel = -8;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->lastInflection = 0;
        this->nextInflection = 0;
        SetNextInflection();
        this->scanCount = 0;
        this->scanCountTotal = 1;
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
		

        // Read all frames of animation at once into the af_f_slow_stop[i] CRGB struct, which is then used to write to the FastLED leds buffer
        // If we want to automatically create trails, we need to have made the correct changes to the char afc_f_slow_stop's contents to create
        // the fading trail.
        for (int i = 0; i < afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES; i++) {
            af_l_pulsar_a[i] = afc_l_pulsar_a[i] > ' ' ? colorSets[colorPalette][afc_l_pulsar_a[i] - '0'] : CRGB::Black;
        }
    }

    ~FragmentTestSprite() {
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
		
		// debug(2);
        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) { // >= ?
            isScanning = false;
            currentPixel += afc_l_pulsar_a_ANIMATION_FRAME_WIDTH; // 8
            currentPixel -= 8;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = GetNewScanCountTotal();
            this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            leds[currentPixel - 6] = CRGB::Black;
            leds[currentPixel - 8] = CRGB::Black;
            leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel - 10] = CRGB::Black;
			// debug(3);
        }

        if (! isScanning) {
            // Traveling and continuing to travel.
			// debug(4);
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            ++currentPixel;

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
				// debug(5);
                // Safety. Since I don't trust my math, once we enter scanning mode, ALWAYS go back to the constant speed for scanning
                // regardless of what the math said.
                updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
                isScanning = true;
                scanningFrame = 0;
                currentPixel += 1; // -8 normally
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
			// debug(6);
            stripcpy(leds, af_l_pulsar_a + afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == afc_l_pulsar_a_ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                // SetNextInflection();
            }
        }

        return true;
    }
};

// Animation sprites from last year
/*
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
                // SetNextInflection();
            }
        }

        return true;
    }
};
*/

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
                // SetNextInflection();
            }
        }

        return true;
    }
};

/*
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
                // SetNextInflection();
            }
        }

        return true;
    }
};
*/

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

    createColorsets();

    createAnimationFrames();


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

	// Spawn lurkers randomly
	if (random(0,1000) == 0) {
		// debug(3);
		int lurkerSpawnPixel = random(40,149);
		Sprite *s1 = new LurkerSprite(lurkerSpawnPixel,1); 
		// TODO: check to see if another lurker already exists at this pixel, despawn if so
		
        if (! spriteManager->Add(s1)) {
            delete s1;
        } 
	} else {
		// debug(1);
	}

    if (sensor1->IsActuated()) {
        debug(1);
        // Sprite *s1 = new W8V1ScannerDebrisV1Sprite();
        // Sprite *s1 = new AnimationTestSprite();
        // Sprite *s1 = new FragmentTestSprite();
        Sprite *s1 = new LoopTestSprite();
		


        if (! spriteManager->Add(s1)) {
            delete s1;
        }
    }

    if (sensor2->IsActuated()) {
        //Sprite *s2 = new W8V1ScannerDebrisV1ReverseSprite();
        // debugNeg(42);
        /*
        if (! spriteManager->Add(s2)) {
            delete s2;
        }
        */
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
  
/*
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
*/  
    
    // The L animations are loops. 
    //
    // These always start with pixels 11 and 12 (counting from 0) at 8, and return to the same position. 
    
    //                      123456789012345678901234
    strcpy(afc_l_pulsar_a, "           88           ");
    strcat(afc_l_pulsar_a, "          1881          ");
    strcat(afc_l_pulsar_a, "          2882          ");
    strcat(afc_l_pulsar_a, "         138831         ");
    strcat(afc_l_pulsar_a, "         248842         ");
    strcat(afc_l_pulsar_a, "        13588531        ");
    strcat(afc_l_pulsar_a, "       1246886421       ");
    strcat(afc_l_pulsar_a, "      123578875321      ");
    strcat(afc_l_pulsar_a, "     12346888864321     ");
    strcat(afc_l_pulsar_a, "    1234578888754321    ");
    strcat(afc_l_pulsar_a, "   123456888888654321   ");
    strcat(afc_l_pulsar_a, "  12345677888877654321  ");
    strcat(afc_l_pulsar_a, " 1234567868888687654321 ");
    strcat(afc_l_pulsar_a, "123456787578875787654321");
    strcat(afc_l_pulsar_a, "234567876468864678765432");
    strcat(afc_l_pulsar_a, "123456765358853567654321");
    strcat(afc_l_pulsar_a, " 1234565424884245654321 ");
    strcat(afc_l_pulsar_a, "  12345431388313454321  ");
    strcat(afc_l_pulsar_a, "   123432 2882 2343210  ");
    strcat(afc_l_pulsar_a, "    12321 1881 123210   ");
    strcat(afc_l_pulsar_a, "     121  1881  1210    ");
    strcat(afc_l_pulsar_a, "      1    88    1      ");
    
    
    // The F animations are fragments.
    
    // The plan is to cut them into "intro" and "outro" animations, with loop animations inserted in between
    // Hopefully we can have enough SRAM for multiple "intro" and "outro" animations, so that the motion doesn't become predictable. 
    // Also, incorporating auto-trails and controllable acceleration factors in UpdateTravel methods means that we reduce the number
    // of frames of animation that we have to render into RAM. 

    //                       12345678901234567
    strcpy(afc_f_slow_stop, "123456788        ");
    strcat(afc_f_slow_stop, " 123456778       ");
    strcat(afc_f_slow_stop, "  123456678      ");
    strcat(afc_f_slow_stop, "   12345568      ");
    strcat(afc_f_slow_stop, "    12344578     ");
    strcat(afc_f_slow_stop, "     1233468     ");
    strcat(afc_f_slow_stop, "      122357     ");
    strcat(afc_f_slow_stop, "       112468    ");
    strcat(afc_f_slow_stop, "         1358    ");
    strcat(afc_f_slow_stop, "          248    ");
    strcat(afc_f_slow_stop, "          158    ");
    strcat(afc_f_slow_stop, "           68    ");
    strcat(afc_f_slow_stop, "          178    ");
    strcat(afc_f_slow_stop, "          288    ");
    strcat(afc_f_slow_stop, "          388    ");
    strcat(afc_f_slow_stop, "          488    ");
    strcat(afc_f_slow_stop, "          588    ");
    strcat(afc_f_slow_stop, "          688    ");
    strcat(afc_f_slow_stop, "          788    ");
    strcat(afc_f_slow_stop, "          887    ");
    strcat(afc_f_slow_stop, "          886    ");
    strcat(afc_f_slow_stop, "          885    ");
    strcat(afc_f_slow_stop, "          884    ");
    strcat(afc_f_slow_stop, "          883    ");
    strcat(afc_f_slow_stop, "         2882    ");
    strcat(afc_f_slow_stop, "        23871    ");
    strcat(afc_f_slow_stop, "        3486     ");
    strcat(afc_f_slow_stop, "        4585     ");
    strcat(afc_f_slow_stop, "        5684     ");
    strcat(afc_f_slow_stop, "        6783     ");
    strcat(afc_f_slow_stop, "        7882     ");
    strcat(afc_f_slow_stop, "       48871     ");
    strcat(afc_f_slow_stop, "       5876      ");
    strcat(afc_f_slow_stop, "       6865      ");
    strcat(afc_f_slow_stop, "       7854      ");
    strcat(afc_f_slow_stop, "      48843      ");
    strcat(afc_f_slow_stop, "      58832      ");
    strcat(afc_f_slow_stop, "      68821      ");
    strcat(afc_f_slow_stop, "      7881       ");
    strcat(afc_f_slow_stop, "     5888        ");
    strcat(afc_f_slow_stop, "     6887        ");
    strcat(afc_f_slow_stop, "     7876        ");
    strcat(afc_f_slow_stop, "     8865        ");
    strcat(afc_f_slow_stop, "     8854        ");
    strcat(afc_f_slow_stop, "     7883        ");
    strcat(afc_f_slow_stop, "     6882        ");
    strcat(afc_f_slow_stop, "     5788        ");
    strcat(afc_f_slow_stop, "     4688        ");
    strcat(afc_f_slow_stop, "     3588        ");
    strcat(afc_f_slow_stop, "     24788       ");
    strcat(afc_f_slow_stop, "     13688       ");
    strcat(afc_f_slow_stop, "      2578       ");
    strcat(afc_f_slow_stop, "      14688      ");
    strcat(afc_f_slow_stop, "       3588      ");
    strcat(afc_f_slow_stop, "       2488      ");
    strcat(afc_f_slow_stop, "       13788     ");
    strcat(afc_f_slow_stop, "        2688     ");
    strcat(afc_f_slow_stop, "        1588     ");
    strcat(afc_f_slow_stop, "        14788    ");
    strcat(afc_f_slow_stop, "        13688    ");
    strcat(afc_f_slow_stop, "        125788   ");
    strcat(afc_f_slow_stop, "        1246788  ");
    strcat(afc_f_slow_stop, "        12356788 ");
    strcat(afc_f_slow_stop, "        123456788");
    strcat(afc_f_slow_stop, "                 ");
    
    //                         1234567890123456789012345
    strcpy(afc_f_slow_stop_c, "123456788                ");
    strcat(afc_f_slow_stop_c, " 1234567882              ");
    strcat(afc_f_slow_stop_c, "  1234567782             ");
    strcat(afc_f_slow_stop_c, "   1234566782            ");
    strcat(afc_f_slow_stop_c, "    123455684            ");
    strcat(afc_f_slow_stop_c, "     123445782           ");
    strcat(afc_f_slow_stop_c, "      12334684           ");
    strcat(afc_f_slow_stop_c, "       1223586           ");
    strcat(afc_f_slow_stop_c, "        1124782          ");
    strcat(afc_f_slow_stop_c, "          13684          ");
    strcat(afc_f_slow_stop_c, "           2586          ");
    strcat(afc_f_slow_stop_c, "           1487          ");
    strcat(afc_f_slow_stop_c, "            378          ");
    strcat(afc_f_slow_stop_c, "            268          ");
    strcat(afc_f_slow_stop_c, "            158          ");
    strcat(afc_f_slow_stop_c, "             48          ");
    strcat(afc_f_slow_stop_c, "             38          ");
    strcat(afc_f_slow_stop_c, "             28          ");
    strcat(afc_f_slow_stop_c, "             18          ");
    strcat(afc_f_slow_stop_c, "             18          ");
    strcat(afc_f_slow_stop_c, "             38          ");
    strcat(afc_f_slow_stop_c, "             58          ");
    strcat(afc_f_slow_stop_c, "             78          ");
    strcat(afc_f_slow_stop_c, "             88          ");
    strcat(afc_f_slow_stop_c, "             88          ");
    strcat(afc_f_slow_stop_c, "            288          ");
    strcat(afc_f_slow_stop_c, "            488          ");
    strcat(afc_f_slow_stop_c, "            688          ");
    strcat(afc_f_slow_stop_c, "            788          ");
    strcat(afc_f_slow_stop_c, "            887          ");
    strcat(afc_f_slow_stop_c, "            886          ");
    strcat(afc_f_slow_stop_c, "           2885          ");
    strcat(afc_f_slow_stop_c, "           4884          ");
    strcat(afc_f_slow_stop_c, "           6884          ");
    strcat(afc_f_slow_stop_c, "           7883          ");
    strcat(afc_f_slow_stop_c, "           8873          ");
    strcat(afc_f_slow_stop_c, "           8862          ");
    strcat(afc_f_slow_stop_c, "           8852          ");
    strcat(afc_f_slow_stop_c, "          28841          ");
    strcat(afc_f_slow_stop_c, "          48841          ");
    strcat(afc_f_slow_stop_c, "          6883           ");
    strcat(afc_f_slow_stop_c, "          7873           ");
    strcat(afc_f_slow_stop_c, "          8862           ");
    strcat(afc_f_slow_stop_c, "         28852           ");
    strcat(afc_f_slow_stop_c, "         48841           ");
    strcat(afc_f_slow_stop_c, "         68841           ");
    strcat(afc_f_slow_stop_c, "         7883            ");
    strcat(afc_f_slow_stop_c, "        28873            ");
    strcat(afc_f_slow_stop_c, "        48862            ");
    strcat(afc_f_slow_stop_c, "        68852            ");
    strcat(afc_f_slow_stop_c, "        78841            ");
    strcat(afc_f_slow_stop_c, "       388741            ");
    strcat(afc_f_slow_stop_c, "       58863             ");
    strcat(afc_f_slow_stop_c, "       78853             ");
    strcat(afc_f_slow_stop_c, "       88742             ");
    strcat(afc_f_slow_stop_c, "       88642             ");
    strcat(afc_f_slow_stop_c, "       87531             ");
    strcat(afc_f_slow_stop_c, "       86431             ");
    strcat(afc_f_slow_stop_c, "       8542              ");
    strcat(afc_f_slow_stop_c, "       8432              ");
    strcat(afc_f_slow_stop_c, "       8321              ");
    strcat(afc_f_slow_stop_c, "       821               ");
    strcat(afc_f_slow_stop_c, "       81                ");
    strcat(afc_f_slow_stop_c, "       8                 ");
    strcat(afc_f_slow_stop_c, "       82                ");
    strcat(afc_f_slow_stop_c, "       84                ");
    strcat(afc_f_slow_stop_c, "       86                ");
    strcat(afc_f_slow_stop_c, "       88                ");
    strcat(afc_f_slow_stop_c, "       788               ");
    strcat(afc_f_slow_stop_c, "       688               ");
    strcat(afc_f_slow_stop_c, "       5788              ");
    strcat(afc_f_slow_stop_c, "       46788             ");
    strcat(afc_f_slow_stop_c, "       356788            ");
    strcat(afc_f_slow_stop_c, "       24567788          ");
    strcat(afc_f_slow_stop_c, "       1345667788        ");
    strcat(afc_f_slow_stop_c, "       123455667788      ");
    strcat(afc_f_slow_stop_c, "        123445566788     ");
    strcat(afc_f_slow_stop_c, "         1233445567788   ");
    strcat(afc_f_slow_stop_c, "          1223344566788  ");
    strcat(afc_f_slow_stop_c, "             12334456788 ");
    strcat(afc_f_slow_stop_c, "                123456788");
    strcat(afc_f_slow_stop_c, "                         ");
//                        123
    strcpy(afc_l_mother, "84 ");
    strcat(afc_l_mother, "751");
    strcat(afc_l_mother, "662");
    strcat(afc_l_mother, "573");
    strcat(afc_l_mother, "484");
    strcat(afc_l_mother, "375");
    strcat(afc_l_mother, "266");
    strcat(afc_l_mother, "157");
    strcat(afc_l_mother, " 48");
 	
//                             123456789012345678901
     strcpy(afc_f_eye_full_a, "123456785            ");
     strcat(afc_f_eye_full_a, " 123456785           ");
     strcat(afc_f_eye_full_a, "  12345685           ");
     strcat(afc_f_eye_full_a, "   12345785          ");
     strcat(afc_f_eye_full_a, "    1234685          ");
     strcat(afc_f_eye_full_a, "     123585          ");
     strcat(afc_f_eye_full_a, "      124785         ");
     strcat(afc_f_eye_full_a, "       13685         ");
     strcat(afc_f_eye_full_a, "        2585         ");
     strcat(afc_f_eye_full_a, "        1485         ");
     strcat(afc_f_eye_full_a, "         3785        ");
     strcat(afc_f_eye_full_a, "         2685        ");
     strcat(afc_f_eye_full_a, "         1585        ");
     strcat(afc_f_eye_full_a, "          485        ");
     strcat(afc_f_eye_full_a, "          385        ");
     strcat(afc_f_eye_full_a, "          285        ");
     strcat(afc_f_eye_full_a, "          185        ");
     strcat(afc_f_eye_full_a, "           74        ");
     strcat(afc_f_eye_full_a, "           63        ");
     strcat(afc_f_eye_full_a, "           52        ");
     strcat(afc_f_eye_full_a, "           55        ");
     strcat(afc_f_eye_full_a, "          5 6        ");
     strcat(afc_f_eye_full_a, "         5  7        ");
     strcat(afc_f_eye_full_a, "        5  5         ");
     strcat(afc_f_eye_full_a, "       5  5          ");
     strcat(afc_f_eye_full_a, "      5  5           ");
     strcat(afc_f_eye_full_a, "     5  5            ");
     strcat(afc_f_eye_full_a, "    5  5             ");
     strcat(afc_f_eye_full_a, "   5  5              ");
     strcat(afc_f_eye_full_a, "  5  5               ");
     strcat(afc_f_eye_full_a, "  4 5                ");
     strcat(afc_f_eye_full_a, "  35                 ");
     strcat(afc_f_eye_full_a, "  25                 ");
     strcat(afc_f_eye_full_a, "  35                 ");
     strcat(afc_f_eye_full_a, "  4 5                ");
     strcat(afc_f_eye_full_a, "  5  5               ");
     strcat(afc_f_eye_full_a, "   5  5              ");
     strcat(afc_f_eye_full_a, "    5  5             ");
     strcat(afc_f_eye_full_a, "     5  5            ");
     strcat(afc_f_eye_full_a, "      5  5           ");
     strcat(afc_f_eye_full_a, "       5  5          ");
     strcat(afc_f_eye_full_a, "        5  5         ");
     strcat(afc_f_eye_full_a, "         5  4        ");
     strcat(afc_f_eye_full_a, "          5 3        ");
     strcat(afc_f_eye_full_a, "           52        ");
     strcat(afc_f_eye_full_a, "           62        ");
     strcat(afc_f_eye_full_a, "           74        ");
     strcat(afc_f_eye_full_a, "           85        ");
     strcat(afc_f_eye_full_a, "           785       ");
     strcat(afc_f_eye_full_a, "           6785      ");
     strcat(afc_f_eye_full_a, "           56786     ");
     strcat(afc_f_eye_full_a, "           456786    ");
     strcat(afc_f_eye_full_a, "           3456787   ");
     strcat(afc_f_eye_full_a, "           23456787  ");
     strcat(afc_f_eye_full_a, "           123456788 ");
     strcat(afc_f_eye_full_a, "            123456788");
	 strcat(afc_f_eye_full_a, "                     ");

}
