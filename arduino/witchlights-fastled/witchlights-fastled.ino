#include <FastLED.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

// debug or animation modes
// TODO: set this with a jumper to an input pin
bool debugMode = true;					// turns on debug() statements
bool spawnLurkers = false;			// Not ready for Firefly 2018
bool randomInflection = false;	// Randomly makes faerie sprite dance back and forth, instead of mainly going "forwards". 
bool spawnFaeries = false;			// TODO Spawn a new faerie randomly; helpful to keep a constant background of sprite animation for evaluation
bool placeLurkers = false;			// TODO Dimly lights up range of pixels where lurkers are "allowed" to spawn, for install time
bool placeTrees = false;				// TODO Dimly lights up range of pixels green where trees are defined, also for installs
bool placeNoIdle = false;				// TODO same, for specifying zones where faeries will not stop to idle

// FastLED constants
#define NUM_LEDS							600 // 750 or 600 in production
#define MAXSPRITES						20

#define NUM_COLORSETS					5
#define NUM_COLORS_PER_SET		9

// Data pins
#define PIR_SENSOR_1_PIN		 3
#define PIR_SENSOR_2_PIN		 4
#define NEOPIXEL_DATA_PIN		 6								// Pin for neopixels

// Sensor time-out (in production, set to 10000)
#define INFRARED_SENSOR_TIMEOUT_IN_MS		5000	// in milliseconds

#define SCANNER_SPRITE_FRAME_DELAY_IN_MS			1
#define TEST_PATTERN_FRAME_DELAY_IN_MS				1

#define FAERIE_MIN_IDLE							1
#define FAERIE_MAX_IDLE							4

#define FAERIE_FLIT_MIN_DISTANCE		5
#define FAERIE_FLIT_MAX_DISTANCE		25

#define FAERIE_FLIT_MIN_START_INTERVAL	25
#define FAERIE_FLIT_MAX_START_INTERVAL	35

#define FAERIE_MIN_SPEED						1
#define FAERIE_MAX_SPEED 						10

#define FAERIE_MIN_BRAKE						32		// 96 for 50-60 px
#define FAERIE_MAX_BRAKE						64		// 112

#define FAERIE_MIN_WAIT							1
#define FAERIE_MAX_WAIT							5

#define FAERIE_MIN_TRAIL_LENGTH			80	// The lower the value, the longer the trail generated, but also the more FastLED functions get called per update per sprite. 
#define FAERIE_MAX_TRAIL_LENGTH			33

// currently set this to be consistent for animation design
#define FAERIE_MIN_LONG_TRAVEL			180	// This range is dead center in my first "no idle" zone, so any problems with the no-idle functions will show up quickly
#define FAERIE_MAX_LONG_TRAVEL			300	// 

#define SPRITE_STARTING_DELAY_INTERVAL_IN_MS	 50 // 40
#define SCANNER_DELAY_INTERVAL_IN_MS					 20

#define NO_IDLE_LOOP_COUNT					7			// number of no-idle zones in the array

// lurker sprite constants

#define BLINK_SPRITE_MIN_LIFETIME		3
#define BLINK_SPRITE_MAX_LIFETIME		12		// # of blinks before dying

#define BLINK_SPRITE_MAX_BLINK_SPEED	5		// ms interval between updates, lower is faster
#define BLINK_SPRITE_MIN_BLINK_SPEED	45

#define BLINK_MIN_COUNT		1
#define BLINK_MAX_COUNT		4

#define LURKER_BLINK_MIN_FREQUENCY	2500
#define LURKER_BLINK_MAX_FREQUENCY	6000

#define LURKER_BLINK_SHUT_MIN_TIMING	50
#define LURKER_BLINK_SHUT_MAX_TIMING	500

#define LURKER_MIN_PIXEL_1	40
#define LURKER_MAX_PIXEL_1	75
#define LURKER_MIN_PIXEL_2	120
#define LURKER_MAX_PIXEL_2	148
#define LURKER_MIN_PIXEL_3	400
#define LURKER_MAX_PIXEL_3	450

// TreeSprite locations
#define TREE_FADE_PIXEL_1 315
#define TREE_START_1		323
#define TREE_END_1			334

#define TREE_FADE_PIXEL_2 315
#define TREE_START_2		323
#define TREE_END_2			334

#define TREE_FADE_PIXEL_3 315
#define TREE_START_3		323
#define TREE_END_3			334

#define TREE_FADE_PIXEL_4 315
#define TREE_START_4		323
#define TREE_END_4			334

#define TREE_FADE_PIXEL_5 315
#define TREE_START_5		323
#define TREE_END_5			334

// For testing use only. In production, set this equal to 1. Use this to exaggerate the acceleration effects. 10-20 is good for testing.
#define ACCELERATION_DELAY_OBVIOUSNESS_FACTOR				 1


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

char afc_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB af_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

// char afc_2_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];
// CRGB af_2_sparkle_a[SPARKLE_ANIMATION_FRAME_WIDTH * SPARKLE_ANIMATION_FRAMES];

char afc_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];
CRGB af_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];

char afc_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];
CRGB af_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];

char afc_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];
CRGB af_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];


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
				return true;		// Always true, no reason to veto this one.
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
				nextInflection += random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL + 1);
		}

		int GetNewScanCountTotal() {
				return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE + 1);
		}

	public:
		AnimationTestSprite() : Sprite() {
				// Initial state.
				this->currentPixel = -8;	// The first pixel of the pattern is black.
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
						leds[currentPixel - 9] = CRGB::Black;	 // I hate this. One-off to get rid of the straggler when coming out of scan mode.
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
	int eyeWidth; // spacing between eyes, minimum is 1, set this higher the further away you are locating the sprite from the viewer
	int blinkDirection; // if in blink mode, are we closing or opening? -1 for closing, +1 for opening, 0 for staring
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
	
	// TODO: make eye dimness slightly variable on spawn
	int SetMaxEyeColor() {
		// random 3-5
		return random(3,5 +1);
	}
	
	int SetBlinkMaxCount() {
		// debug(4);
		return random(BLINK_MIN_COUNT,BLINK_MAX_COUNT + 1);

	}
	
	int SetBlinkTiming() {
		// debug(5);
		return random(LURKER_BLINK_SHUT_MIN_TIMING,LURKER_BLINK_SHUT_MAX_TIMING + 1); // ms; testing to see what looks good, these are rough guesses
	}
	

	
	int SetBlinkFrequency() {
		return random(LURKER_BLINK_MIN_FREQUENCY, LURKER_BLINK_MAX_FREQUENCY +1);
	}
	
	void SpawnBlinkChild() {
		// debug(6);
		// random chance to spawn a new blink sprite with a reduced lifespan
	}
	
	// TODO: blink in multiple sets
	int Blink() {
		if (blinkDirection == 0) {
			if (eyeColor == eyeMaxColor) {
				// we are staring.
				// check to see if we need to blink
				if (millis() - lastBlinkTime >= blinkFrequency) {
					lastBlinkTime = millis();
					blinkFrequency = SetBlinkFrequency(); // reset blinkFrequency after every blink?
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
		this->currentPixel = spawnPixel;
		this->updateInterval = SetInitialBlinkSpeed();
		this->lifetimeBlinks = SetLifeSpan(); 
		this->blinkCount = -1;							// When first opening the eyes, it "counts" as a blink, and setting to -1 means we don't count it against lifespan
		this->blinkFrequency = SetBlinkFrequency();		// decent test with 4000 ms, trying a random set
		this->eyeWidth = eyew;							// can we set this on spawn? Make it semi-random within params? 
		this->blinkDirection = 1;						// want to start with eyes closed and open them
		this->eyeColor = 0;								// eyes closed = 0
		this->eyeMaxColor = SetMaxEyeColor();			// up to 5 in the color set
		this->blinkMaxCount = SetBlinkMaxCount();
		this->blinkTiming = SetBlinkTiming();
		this->lastBlinkTime = millis();
		
		// this->colorPalette = random(0, NUM_COLORSETS);
		this->colorPalette = 2;								// yellow to start


		this->eyes[0] = colorSets[colorPalette][eyeColor];
		this->eyes[eyeWidth] = colorSets[colorPalette][eyeColor];
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
		
			if (! this->allowCreation()) {
				this->MarkDone();
				return false;
			}
		
			// Decide if we're going to blink, and set the value to do it
			blinkDirection = Blink();
		
			// close or open the eyes a step by adding blinkDirection to eye color
			eyeColor += blinkDirection;
		
			this->eyes[0] = colorSets[colorPalette][eyeColor]; // colorSets[2][eyeColor];
			this->eyes[eyeWidth] = colorSets[colorPalette][eyeColor]; // colorSets[2][eyeColor]; 
		
			stripcpy(leds, eyes, currentPixel, eyeLength, eyeLength);
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
				nextInflection += random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL + 1);
		}

		int GetNewScanCountTotal() {
				return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE + 1);
		}

	public:
		LoopTestSprite() : Sprite() {
				// Initial state.
				this->currentPixel = -8;	// The first pixel of the pattern is black.
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
						leds[currentPixel - 9] = CRGB::Black;	 // I hate this. One-off to get rid of the straggler when coming out of scan mode.
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

class FaerieSprite : public Sprite {
private:
	int updateInterval;
	int minInterval;
	int maxInterval;
	int currentPixel;
	int currentLocation;
	int travelDirection;
	bool isIdling;
	int idlingFrame;
	bool isWaiting;
	int waitCount;
	int waitCountTotal;
	bool isBraking;
	int brakeDistance;
	int brakePixel;
	int lastInflection;
	int nextInflection;
	int travelDistance;
	int idleCount;
	int idleCountTotal;
	float accelerationFactor;	// make these semi-randomly set parameters on create; range from 0.5 to 2? 
	float brakeFactor;
	int dimFactor;
	int trailLength;					// range from 32 to 80
	int idleFrameCount;				// how many frames the idle algorithm uses
	int pixelA;
	int pixelB;
	int pixelC;
	int fadeSteps;
	int colorInertia;
	int colorPalette;
	bool idleToTravel;
	int totalTravelDistance;
	int currentDistance;
	float brakePercentage;
	int minNoIdle[7] = { 1, 80, 145, 295, 445, 595, 718 };
	int maxNoIdle[7] = { 42, 111, 155, 305, 455, 605, 749 };

	CRGB pattern[3];
	int patternLength = 3;

	// use updateInterval and map() to make tails longer when going faster (reduce the fade factor) and shorter when going slower (increase fade factor)
	int SetDimFactor(int interval) {
		return map(interval, 0, SPRITE_STARTING_DELAY_INTERVAL_IN_MS, trailLength, 192);
	}

	int SetTrailLength() {
		return random(FAERIE_MAX_TRAIL_LENGTH, FAERIE_MIN_TRAIL_LENGTH) + 1;
	}

	// TODO define global constants for brake and acceleration factor ranges
	float SetBrakeFactor(bool waiting) {
		if (!waiting) {
			return (random(500,900) + 1) / 100;
		} else {
			return (random(9000,19000) + 1) / 100;
		}
	}
	
	float SetAccelerationFactor(bool waiting) {
		if (!waiting) {
			return (random(100,350) + 1) / 100;
		} else {
			return (random(400,1000) + 1) / 100;
		}
	}

	void SetNextInflection() {
		lastInflection = nextInflection;
		
		int travelDistance = random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL) + 1;
		
		if (randomInflection && currentPixel > 60) {
			nextInflection += travelDistance * TravelDirectionSwitch();
			return;
		} else {
			// travelDirection is either 1 or -1
			nextInflection += travelDistance * travelDirection;
		}
		// if (nextInflection < 150) debug(nextInflection);
		// delay(1000);
	}

	int ReturnClosestPixel(int pixel, int minpix, int maxpix) {
		int minDistance = abs(pixel - minpix);
		int maxDistance = abs(pixel - maxpix);
		return (minDistance - maxDistance < 0) ? (minpix - minDistance) : (maxpix + maxDistance);
	}


	// for (const string &text : texts) {}
	// for (Sprite *otherSprite : faerieVector)  ???

	int AvoidNoIdle(int targetPixel) {
		for (int i = 0; i <= NO_IDLE_LOOP_COUNT - 1; i++) {
			if (targetPixel >= this->minNoIdle[i] && targetPixel <= this->maxNoIdle[i]) {
				return ReturnClosestPixel(targetPixel, minNoIdle[i], maxNoIdle[i]);
			}
		}
		return targetPixel;
	}
	
	void SetNextWaitTravelTarget() {
		// TODO use collision detection when setting next inflection, so that faeries are never traveling TO the same target pixel (or within a range of that pixel, like +- 3)
		
		lastInflection = nextInflection;
		int travelDistance = (random(FAERIE_FLIT_MIN_DISTANCE, FAERIE_FLIT_MAX_DISTANCE) + 1) * TravelDirectionSwitch();
		nextInflection += travelDistance;
	}

	int TravelDirectionSwitch() {
		// returns -1 or 1 randomly
		if (random(0,2) == 0) {
			return -1;
		} else {
			return 1;
		}
	}

	int GetNewidleCountTotal() {
		return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE) + 1;
	}

	void FadeToColor() {
		// https://gist.github.com/kriegsman/d0a5ed3c8f38c64adcb4837dafb6e690
		// https://gist.github.com/kriegsman/1f7ccbbfa492a73c015e
		// want to have the sprite's two bright pixels fade from one color set to another
	}
	
	// add int direction and make tailpixel += direction, and when direction is "backwards", offset the dimtrail by 3 pixels to prevent dimming the sprite
	void DimTrail(int tailPixel, int dimFactor, int direction) {
		if (tailPixel < 0) return;
		if (tailPixel > NUM_LEDS) return;
		if (! leds[tailPixel]) return;
		
		leds[tailPixel].fadeToBlackBy(dimFactor);
		tailPixel += direction;
		DimTrail(tailPixel, dimFactor, direction);
	}
	
	void FadeOutTrail(int tailPixel, int dimFactor, int direction) {
		if (tailPixel < 0) return;
		if (tailPixel > NUM_LEDS) return;
		if (! leds[tailPixel]) return;
		
		// Recursively run DimTrail() at tailPixel+1, so trail fades from the dim end
		DimTrail(tailPixel, dimFactor, direction);
		FadeOutTrail(tailPixel + direction, dimFactor, direction);
	}

	int TravelDirection() {
		// return + for moving in a positive direction, - for moving backwards
		
		if (nextInflection > currentPixel) {
			return 1;
		} else if (nextInflection == currentPixel) {
			return 0;
		} else {
			return -1;
		}
	}
	
	void UpdatePattern() {
		this->pattern[0] = colorSets[colorPalette][pixelA];
		this->pattern[1] = colorSets[colorPalette][pixelB];
		this->pattern[2] = colorSets[colorPalette][pixelC];
	}
	
	void UpdatePatternWithSparkles() {
		int randomA = random(0,pixelA) + 1;
		int randomB = random(0,pixelB) + 1;
		int randomC = random(0,pixelC) + 1;
		
		this->pattern[0] = colorSets[colorPalette][randomA];
		this->pattern[1] = colorSets[colorPalette][randomB];
		this->pattern[2] = colorSets[colorPalette][randomC];
	}
	
	int DistanceFromDestination() {
		return abs(currentPixel - nextInflection);
	}

	int SetMaxSpeed() {
		return random(FAERIE_MIN_SPEED, FAERIE_MAX_SPEED) + 1;
	}
	
	int SetMaxBrake() {
		return random(FAERIE_MIN_BRAKE, FAERIE_MAX_BRAKE) + 1;
	}
	
	float SetBrakePercentage() {
		// TODO: map distance of move to brake percentage, brakefactor, accelerationfactor
		if (!isWaiting) {
			return .06;	//  TODO - set this with a constant // .12 worked for 50-75 pixel moves, too big for 160 pixel moves
		} else {
			return .45;
		}
	}
	
	int SetWaitCount() {
		return random(FAERIE_MIN_WAIT,FAERIE_MAX_WAIT) + 1;
	}
	
	int SetWaitInterval() {
		return random(FAERIE_FLIT_MIN_START_INTERVAL, FAERIE_FLIT_MAX_START_INTERVAL) + 1;
	}
	
	int EffectiveFrame(int frame) {
		return frame % idleFrameCount; 
	}
	
	int IdlePixelA(int frame) {
		return 8 - abs(EffectiveFrame(frame) -8);
	}
	
	int IdlePixelB(int frame) {
		//Named variables after the shapes I'm folding the line into with the abs
		//function
		int Vee = abs(EffectiveFrame(frame) - 8);
		int W = abs(Vee - 4);
		return 8 - W;
	}

	int IdlePixelC(int frame) {
		return abs(EffectiveFrame(frame) -8);
	}
	
	// TODO: fix this
	int AccelerateIdle(int frame) {
		if (EffectiveFrame(frame) == 9 || EffectiveFrame(frame) == 0) {
			updateInterval = 0;
		}
		return (frame % 8) * 1.8;
	}
	
	int AccelerateTravel() {
		// Decide whether braking or accelerating
		if (currentDistance < totalTravelDistance * brakePercentage) {
			if (!isBraking) {
				isBraking = true;
				brakeDistance = abs(currentDistance - totalTravelDistance);
				brakePixel = currentPixel;
			}
		} else {
			isBraking = false;
		}
		
		// When waiting (short moves) we use a different acceleration and braking factor than we do with longer moves. You need exaggerated acceleration and braking over shorter distances.
		SetBrakeFactor(isWaiting);
		SetAccelerationFactor(isWaiting);
		
		// Accelerate or brake by returning positive or negative values to subtract from updateInterval
		if (! isBraking) {
		int x = abs(currentDistance - totalTravelDistance);
			return round(sqrt(x) * accelerationFactor);
		} else if (isBraking) {
		int x = abs(currentPixel - brakePixel);
			return -1 * round(sqrt(x) * brakeFactor); 
		}
	}

	
	int ConstrainIntervalToMaxMin(int interval) {
		if (interval < minInterval) {
			return minInterval;
		} 
		
		if (interval > maxInterval) {
			return maxInterval;
		}

		return interval;
	}
	

	bool CheckForTermination(int direction) {
		if (direction >= 1) {
			return this->currentPixel > NUM_LEDS ? true : false;
		} else if (direction <= -1) {
			// debug(6);
			return this->currentPixel < 0 ? true : false;
		} else {
			// something is very wrong if direction = 0
			debug(10);
			return true;
		}
	}

	/*	
	void JitterLocation() {

		bool moveIt = leds[currentPixel - 1];
		if (!moveIt) {
			moveIt = leds[currentPixel + 3];
		}
		
		if (EffectiveFrame(idlingFrame) == 0 && random(0,3) == 0 && !moveIt) { // pixelA == 8
			currentPixel --;
			updateInterval = 18; // updateInterval * 2;
		} else if (EffectiveFrame(idlingFrame) == 8 && random(0,3) == 0 && !moveIt) { // pixelC == 8
			currentPixel ++;
			updateInterval = 18; // updateInterval * 2;
		}

	}
	*/
	
	void StartTravel() {
		// Transition from loop mode to travel mode
		isIdling = false;
		currentPixel += 0;
		isWaiting ? SetNextWaitTravelTarget() : SetNextInflection();
		// debug(1);
		// delay(500);
		// if (nextInflection < 150) debug(nextInflection);
		// delay(1000);
		nextInflection = AvoidNoIdle(nextInflection);
		// debug(2);
		// delay(500);
		// if (nextInflection < 150) debug(nextInflection);
		// delay(1000);
		totalTravelDistance = DistanceFromDestination();
		this->idleCount = 0;
		this->idleCountTotal = GetNewidleCountTotal(); // set to 1 for fragments

		isWaiting ? this->updateInterval = SetWaitInterval() : this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
		this->brakePercentage = SetBrakePercentage();

		// debug(updateInterval);
		// need to set a bool here so that updateTravel() knows to fade the values towards 848?
		// idleToTravel
		this->pixelA = 6;
		this->pixelB = 8;
		this->pixelC = 6;
		UpdatePattern();
	}

	bool UpdateTravel() {
		// First, write current CRGB pattern to leds
		stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
		
		// set dim factor
		dimFactor = SetDimFactor(updateInterval);
		
		// step backwards and fade the trail
		DimTrail(currentPixel, dimFactor, -1);
		DimTrail(currentPixel+3, dimFactor, 1);

		// Next up, prepare for the next UpdateTravel() by moving currentPixel, and setting the next updateInterval value

		currentPixel += TravelDirection();
		currentDistance = DistanceFromDestination();

		updateInterval -= AccelerateTravel();
		updateInterval = ConstrainIntervalToMaxMin(updateInterval);
		
		// If we have reached the destination pixel, our next stop is running the idle animation
		if (currentDistance == 0) {
			StartIdle();
		}
		
		// Terminate if we go off the end of the strip		
		if (CheckForTermination(travelDirection)) {
			// TODO - make the trails fade out after the FaerieSprite goes off-strip (github issue)
			
			FadeOutTrail(NUM_LEDS - 1, 255, -1);
			FadeOutTrail(0, 255, 1);
			// debug(2);
			this->MarkDone();
		}
		
		/*
		if (currentPixel > NUM_LEDS) {
			FadeOutTrail(NUM_LEDS - 1, 255, -1); 
			// FadeOutTrail(0, 255, 1);
			this->MarkDone();
		}
		*/

		return true;
	}

	bool StartIdle() {
		// If we're in wait mode, we'll count one instance of running an idle animation as one "wait" cycle, which is a series of small moves, in random directions, and idle cycles. 
		if (isWaiting) {
			waitCount++;
		} else {
			isWaiting = true;
		}
		
		dimFactor = 128;
		
		// Transition from travel mode to loop mode
		updateInterval = 1;
		isIdling = true;
		idlingFrame = 0;
		
		currentPixel -= 0; // pretty sure we don't need this unless we're doing char animation
		
		fadeSteps = 0;
		
		return true;
	}

	bool UpdateIdle() {
		if (fadeSteps >= colorInertia) {
			pixelA = IdlePixelA(idlingFrame);
			pixelB = IdlePixelB(idlingFrame);
			pixelC = IdlePixelC(idlingFrame);
		
			updateInterval += AccelerateIdle(idlingFrame);
		
			fadeSteps = 0;
			idlingFrame++;
		} else {
			fadeSteps++;
		}

		UpdatePattern();
		// UpdatePatternWithSparkles();

		stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
	
		FadeOutTrail(currentPixel, dimFactor, -1);
		FadeOutTrail(currentPixel + 3, dimFactor, 1);
	
		if (EffectiveFrame(idlingFrame) == 0) {
			++idleCount;
		}
		
		return true;
	}
	
	bool StartWaiting() {
		isWaiting = true;
		waitCount = 0;
		waitCountTotal = SetWaitCount();
	}
	
	bool UpdateWaiting() {
		
	}

public:
	FaerieSprite(int direction, int start) : Sprite() {
		// Initial state.
		this->currentPixel = start; // DONE - set currentPixel as a parameter
		this->travelDirection = direction;
		this->idlingFrame = 0;
		this->isIdling = false;
		this->isWaiting = false;
		this->lastInflection = 0;
		this->nextInflection = start;
		SetNextInflection();
		this->idleCount = 0;
		this->idleCountTotal = GetNewidleCountTotal();
		this->waitCount = 0;
		this->waitCountTotal = SetWaitCount();
		this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
		this->minInterval = SetMaxSpeed();
		this->maxInterval = SetMaxBrake();
		this->brakePixel;
		this->trailLength = SetTrailLength();
		this->dimFactor = SetDimFactor(updateInterval);
		this->idleFrameCount = 16;
		this->fadeSteps = 0;
		this->colorInertia = 1;
		this->idleToTravel = true;
		this->totalTravelDistance = DistanceFromDestination();
		this->currentDistance = totalTravelDistance;
		this->isBraking = false;
		this->brakeDistance;
		this->minNoIdle;
		this->maxNoIdle;
		
		//To be set semi-randomly for all sprites, making them move just a bit differently so that they never mirror each other
		this->brakePercentage = SetBrakePercentage(); // .15 originally
		this->accelerationFactor = SetAccelerationFactor(isWaiting); // 0.75
		this->brakeFactor = SetBrakeFactor(isWaiting); // 8
		
		// Choose a random color palette from the palettes available.
		this->colorPalette = random(0, NUM_COLORSETS);
		this->pixelA = 6;
		this->pixelB = 8;
		this->pixelC = 6;

		// Set the colors in the pattern.
		UpdatePattern();

		this->patternLength = 3;

		// Required for pixel animation:
		/*
		for (int i = 0; i < afc_l_mother_ANIMATION_FRAME_WIDTH * afc_l_mother_ANIMATION_FRAMES; i++) {
			af_l_mother[i] = afc_l_mother[i] > ' ' ? colorSets[colorPalette][afc_l_mother[i] - '0'] : CRGB::Black;
		}
		*/
	}

	~FaerieSprite() {
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

		// if (travelDirection == -1) debug(6); // this test passed 2018-06-24

		// Going from scanning to travel mode.
		if (isIdling && idleCount == idleCountTotal) {
		// need to put in a transition animation from idle to travel here?

			if (isWaiting && waitCount >= waitCountTotal) {
				isWaiting = false;
				waitCount = 0;
				waitCountTotal = SetWaitCount();
			}

			StartTravel();
		}

		if (! isIdling) {
			// Traveling and continuing to travel.
			UpdateTravel();
		} else {
			UpdateIdle();
		}

		return true;
	}

	int CurrentPixel() {
		// return location to SpriteManager for collision detection		
		return currentPixel;
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
				nextInflection += random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL + 1);
		}

		int GetNewScanCountTotal() {
				return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE + 1);
		}

	public:
		FragmentTestSprite() : Sprite() {
				// Initial state.
				this->currentPixel = -8;	// The first pixel of the pattern is black.
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
						leds[currentPixel - 9] = CRGB::Black;	 // I hate this. One-off to get rid of the straggler when coming out of scan mode.
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
				nextInflection += random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL + 1);
		}

		int GetNewScanCountTotal() {
				return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE + 1);
		}

	public:
		W8V1ScannerDebrisV1Sprite() : Sprite() {
				// Initial state.
				this->currentPixel = -8;	// The first pixel of the pattern is black.
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
						leds[currentPixel - 9] = CRGB::Black;	 // I hate this. One-off to get rid of the straggler when coming out of scan mode.
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
				nextInflection -= random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL + 1);
		}

		int GetNewScanCountTotal() {
				return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE + 1);
		}

	public:
		W8V1ScannerDebrisV1ReverseSprite() : Sprite() {
				// Initial state.
				this->currentPixel = NUM_LEDS + 7;	// The first pixel of the pattern is black.
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
						leds[currentPixel + 9] = CRGB::Black;	 // I hate this. One-off to get rid of the straggler when coming out of scan mode.
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
				nextInflection += random(FAERIE_MIN_LONG_TRAVEL, FAERIE_MAX_LONG_TRAVEL + 1);
		}

		int GetNewScanCountTotal() {
				return random(FAERIE_MIN_IDLE, FAERIE_MAX_IDLE + 1);
		}

	public:
		ScannerSprite() : Sprite() {
				// Initial state.
				this->currentPixel = -2;	// Both of the first two pixels of the pattern are black.
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
						leds[currentPixel - 9] = CRGB::Black;	 // I hate this. One-off to get rid of the straggler when coming out of scan mode.
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

		// TODO: define pins for these booleans as pullup input pins, for jumper to ground activation
		/*
		debugMode = true;					// turns on debug() statements
		bool spawnLurkers = true;			// IMPORTANT: set to FALSE for all public video before Firefly 2018!
		bool randomInflection = false;	// Randomly makes faerie sprite dance back and forth, instead of mainly going "forwards". 
		bool spawnFaeries = true;				// TODO Spawn a new faerie randomly; helpful to keep a constant background of sprite animation for evaluation
		bool placeLurkers = false;			// TODO Dimly lights up range of pixels where lurkers are "allowed" to spawn, for install time
		bool placeTrees = false;				// TODO Dimly lights up range of pixels green where trees are defined, also for installs
		bool placeNoIdle;
		bool tuneInterval;
		bool burnNight;									// TODO Sprites go from 4 to 25 slowly over the course of the night, and motion sensor timeout is reduced to 1 second. 
		*/

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
		if (random(0,1000) == 0 && spawnLurkers) {
			// debug(3);
			int lurkerSpawnPixel = random(40,149); // TODO - create array of lurker zones to use instead of the constants?
			Sprite *s1 = new LurkerSprite(lurkerSpawnPixel,1); 
			// TODO: check to see if another lurker already exists at this pixel, despawn if so
		
			if (! spriteManager->Add(s1)) {
				delete s1;
			} 
		} else {
			// debug(1);
		}
		
		if (random(0,1000) == 0 && spawnFaeries) {
			Sprite *s1 = new FaerieSprite(1, -3); 
		
			if (! spriteManager->Add(s1)) {
				delete s1;
			}
			
		} else {
			// debug(1);
		}

		if (sensor1->IsActuated()) {
			// debug(1);
			Sprite *s1 = new FaerieSprite(1, -3); // production
			// Sprite *s1 = new FaerieSprite(-1, 153); // For testing

			if (! spriteManager->Add(s1)) {
					delete s1;
			}
		}

		if (sensor2->IsActuated()) {
			Sprite *s2 = new FaerieSprite(-1, NUM_LEDS + 3);
			
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
		fill_solid(leds + startPos, number < (NUM_LEDS - startPos) ? number : NUM_LEDS - startPos, 0x202020);
}

// TODO: faeries go both ways
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
		strcpy(animationFramesChars, "					1234	 ");
		strcat(animationFramesChars, "					 4323	 ");
		strcat(animationFramesChars, "				 4321	 2 ");
		strcat(animationFramesChars, "			 4321			1");
		strcat(animationFramesChars, "		 4321				 ");
		strcat(animationFramesChars, "		3234				 ");
		strcat(animationFramesChars, "	 2	1234			 ");
		strcat(animationFramesChars, "	1			1234		 ");

		strcpy(animationFramesCharsReverse, "		4321					");
		strcat(animationFramesCharsReverse, "	 3234						");
		strcat(animationFramesCharsReverse, " 2	 1234					");
		strcat(animationFramesCharsReverse, "1		 1234				");
		strcat(animationFramesCharsReverse, "				 1234			");
		strcat(animationFramesCharsReverse, "					4323		");
		strcat(animationFramesCharsReverse, "				4321	2		");
		strcat(animationFramesCharsReverse, "			4321		 1	");

//										12345678901234567890123
		strcpy(afc_w8v1, "					123456788		 ");
		strcat(afc_w8v1, "					 12345688		 ");
		strcat(afc_w8v1, "						12348876	 ");
		strcat(afc_w8v1, "						 128876 4	 ");
		strcat(afc_w8v1, "							88765	 2 ");
		strcat(afc_w8v1, "						 887654		1");
		strcat(afc_w8v1, "						8876543		 ");
		strcat(afc_w8v1, "					 88765432		 ");
		strcat(afc_w8v1, "					887654321		 ");
		strcat(afc_w8v1, "				 887654321		 ");
		strcat(afc_w8v1, "				887654321			 ");
		strcat(afc_w8v1, "			 887654321			 ");
		strcat(afc_w8v1, "			887654321				 ");
		strcat(afc_w8v1, "		 887654321				 ");
		strcat(afc_w8v1, "		887654321					 ");
		strcat(afc_w8v1, "		88654321					 ");
		strcat(afc_w8v1, "	 67884321						 ");
		strcat(afc_w8v1, "	4 678821						 ");
		strcat(afc_w8v1, " 2	56788							 ");
		strcat(afc_w8v1, "1		456788						 ");
		strcat(afc_w8v1, "		3456788						 ");
		strcat(afc_w8v1, "		23456788					 ");
		strcat(afc_w8v1, "		123456788					 ");
		strcat(afc_w8v1, "		 123456788				 ");
		strcat(afc_w8v1, "			123456788				 ");
		strcat(afc_w8v1, "			 123456788			 ");
		strcat(afc_w8v1, "				123456788			 ");
		strcat(afc_w8v1, "				 123456788		 ");


//										 12345678901234567890123
		strcpy(afc_w8v1r, "		 887654321					");
		strcat(afc_w8v1r, "		 88654321						");
		strcat(afc_w8v1r, "		67884321						");
		strcat(afc_w8v1r, "	 4 678821							");
		strcat(afc_w8v1r, " 2	 56788							");
		strcat(afc_w8v1r, "1	 456788							");
		strcat(afc_w8v1r, "		 3456788						");
		strcat(afc_w8v1r, "		 23456788						");
		strcat(afc_w8v1r, "		 123456788					");
		strcat(afc_w8v1r, "			123456788					");
		strcat(afc_w8v1r, "			 123456788				");
		strcat(afc_w8v1r, "				123456788				");
		strcat(afc_w8v1r, "				 123456788			");
		strcat(afc_w8v1r, "					123456788			");
		strcat(afc_w8v1r, "					 123456788		");
		strcat(afc_w8v1r, "						12345688		");
		strcat(afc_w8v1r, "						 12348876		");
		strcat(afc_w8v1r, "							128876 4	");
		strcat(afc_w8v1r, "							 88765	2 ");
		strcat(afc_w8v1r, "							887654	 1");
		strcat(afc_w8v1r, "						 8876543		");
		strcat(afc_w8v1r, "						88765432		");
		strcat(afc_w8v1r, "					 887654321		");
		strcat(afc_w8v1r, "					887654321			");
		strcat(afc_w8v1r, "				 887654321			");
		strcat(afc_w8v1r, "				887654321				");
		strcat(afc_w8v1r, "			 887654321				");
		strcat(afc_w8v1r, "			887654321					");

		//											 12345678901234567890123
		strcpy(afc_2_sparkle_a, "123456788							");
		strcat(afc_2_sparkle_a, " 123456788							");
		strcat(afc_2_sparkle_a, "	 123456788						");
		strcat(afc_2_sparkle_a, "		 12345677						");
		strcat(afc_2_sparkle_a, "			1234566						");
		strcat(afc_2_sparkle_a, "				123455					");
		strcat(afc_2_sparkle_a, "				 12344					");
		strcat(afc_2_sparkle_a, "					 1233					");
		strcat(afc_2_sparkle_a, "						122					");
		strcat(afc_2_sparkle_a, "							11				");
		strcat(afc_2_sparkle_a, "												");
		strcat(afc_2_sparkle_a, "								 1			");
		strcat(afc_2_sparkle_a, "								 1	1		");
		strcat(afc_2_sparkle_a, "							1	 21 2		");
		strcat(afc_2_sparkle_a, "							1	 1	3		");
		strcat(afc_2_sparkle_a, "							21	1 4		");
		strcat(afc_2_sparkle_a, "							31 11 5		");
		strcat(afc_2_sparkle_a, "							42122 6		");
		strcat(afc_2_sparkle_a, "							51213 7		");
		strcat(afc_2_sparkle_a, "												");
		strcat(afc_2_sparkle_a, "						1 71414 2	 1");
		strcat(afc_2_sparkle_a, "						2 51626271 1");
		strcat(afc_2_sparkle_a, "				 1	1 42837422 1");
		strcat(afc_2_sparkle_a, "				 2	2131428681	");
		strcat(afc_2_sparkle_a, "					 13222813472	");
		strcat(afc_2_sparkle_a, "				 2	2313428623 1");
		strcat(afc_2_sparkle_a, "				1 1 14 2833472 2");
		strcat(afc_2_sparkle_a, "				 2	 5 1628681 3");
		strcat(afc_2_sparkle_a, "						16	41342	 2");
		strcat(afc_2_sparkle_a, "				 2 127 122367	 1");
		strcat(afc_2_sparkle_a, "				 1 23811 4846		");
		strcat(afc_2_sparkle_a, "				1	 1262	 6325	 1");
		strcat(afc_2_sparkle_a, "				1		183	 62 4	 2");
		strcat(afc_2_sparkle_a, "				 1	 641 8123	 3");
		strcat(afc_2_sparkle_a, "				 2	185143242	 4");
		strcat(afc_2_sparkle_a, "						 64168161	 5");
		strcat(afc_2_sparkle_a, "						185 8314	 4");
		strcat(afc_2_sparkle_a, "						274 46 6	 5");
		strcat(afc_2_sparkle_a, "						36218614	 4");
		strcat(afc_2_sparkle_a, "						254244262	 2");
		strcat(afc_2_sparkle_a, "						147382144	 1");
		strcat(afc_2_sparkle_a, "						235261262		");
		strcat(afc_2_sparkle_a, "						327141145		");
		strcat(afc_2_sparkle_a, "						215 2 122		");
		strcat(afc_2_sparkle_a, "						1 7		216		");
*/	
		
		// The L animations are loops. 
		//
		// These always start with pixels 11 and 12 (counting from 0) at 8, and return to the same position. 
		
		//											123456789012345678901234
		strcpy(afc_l_pulsar_a, "					 88						");
		strcat(afc_l_pulsar_a, "					1881					");
		strcat(afc_l_pulsar_a, "					2882					");
		strcat(afc_l_pulsar_a, "				 138831					");
		strcat(afc_l_pulsar_a, "				 248842					");
		strcat(afc_l_pulsar_a, "				13588531				");
		strcat(afc_l_pulsar_a, "			 1246886421				");
		strcat(afc_l_pulsar_a, "			123578875321			");
		strcat(afc_l_pulsar_a, "		 12346888864321			");
		strcat(afc_l_pulsar_a, "		1234578888754321		");
		strcat(afc_l_pulsar_a, "	 123456888888654321		");
		strcat(afc_l_pulsar_a, "	12345677888877654321	");
		strcat(afc_l_pulsar_a, " 1234567868888687654321 ");
		strcat(afc_l_pulsar_a, "123456787578875787654321");
		strcat(afc_l_pulsar_a, "234567876468864678765432");
		strcat(afc_l_pulsar_a, "123456765358853567654321");
		strcat(afc_l_pulsar_a, " 1234565424884245654321 ");
		strcat(afc_l_pulsar_a, "	12345431388313454321	");
		strcat(afc_l_pulsar_a, "	 123432 2882 2343210	");
		strcat(afc_l_pulsar_a, "		12321 1881 123210		");
		strcat(afc_l_pulsar_a, "		 121	1881	1210		");
		strcat(afc_l_pulsar_a, "			1		 88		 1			");
		
		
		// The F animations are fragments.
		
		// The plan is to cut them into "intro" and "outro" animations, with loop animations inserted in between
		// Hopefully we can have enough SRAM for multiple "intro" and "outro" animations, so that the motion doesn't become predictable. 
		// Also, incorporating auto-trails and controllable acceleration factors in UpdateTravel methods means that we reduce the number
		// of frames of animation that we have to render into RAM. 

		//											 12345678901234567
		strcpy(afc_f_slow_stop, "123456788				");
		strcat(afc_f_slow_stop, " 123456778				");
		strcat(afc_f_slow_stop, "	 123456678			");
		strcat(afc_f_slow_stop, "		12345568			");
		strcat(afc_f_slow_stop, "		 12344578			");
		strcat(afc_f_slow_stop, "			1233468			");
		strcat(afc_f_slow_stop, "			 122357			");
		strcat(afc_f_slow_stop, "				112468		");
		strcat(afc_f_slow_stop, "					1358		");
		strcat(afc_f_slow_stop, "					 248		");
		strcat(afc_f_slow_stop, "					 158		");
		strcat(afc_f_slow_stop, "						68		");
		strcat(afc_f_slow_stop, "					 178		");
		strcat(afc_f_slow_stop, "					 288		");
		strcat(afc_f_slow_stop, "					 388		");
		strcat(afc_f_slow_stop, "					 488		");
		strcat(afc_f_slow_stop, "					 588		");
		strcat(afc_f_slow_stop, "					 688		");
		strcat(afc_f_slow_stop, "					 788		");
		strcat(afc_f_slow_stop, "					 887		");
		strcat(afc_f_slow_stop, "					 886		");
		strcat(afc_f_slow_stop, "					 885		");
		strcat(afc_f_slow_stop, "					 884		");
		strcat(afc_f_slow_stop, "					 883		");
		strcat(afc_f_slow_stop, "					2882		");
		strcat(afc_f_slow_stop, "				 23871		");
		strcat(afc_f_slow_stop, "				 3486			");
		strcat(afc_f_slow_stop, "				 4585			");
		strcat(afc_f_slow_stop, "				 5684			");
		strcat(afc_f_slow_stop, "				 6783			");
		strcat(afc_f_slow_stop, "				 7882			");
		strcat(afc_f_slow_stop, "				48871			");
		strcat(afc_f_slow_stop, "				5876			");
		strcat(afc_f_slow_stop, "				6865			");
		strcat(afc_f_slow_stop, "				7854			");
		strcat(afc_f_slow_stop, "			 48843			");
		strcat(afc_f_slow_stop, "			 58832			");
		strcat(afc_f_slow_stop, "			 68821			");
		strcat(afc_f_slow_stop, "			 7881				");
		strcat(afc_f_slow_stop, "			5888				");
		strcat(afc_f_slow_stop, "			6887				");
		strcat(afc_f_slow_stop, "			7876				");
		strcat(afc_f_slow_stop, "			8865				");
		strcat(afc_f_slow_stop, "			8854				");
		strcat(afc_f_slow_stop, "			7883				");
		strcat(afc_f_slow_stop, "			6882				");
		strcat(afc_f_slow_stop, "			5788				");
		strcat(afc_f_slow_stop, "			4688				");
		strcat(afc_f_slow_stop, "			3588				");
		strcat(afc_f_slow_stop, "			24788				");
		strcat(afc_f_slow_stop, "			13688				");
		strcat(afc_f_slow_stop, "			 2578				");
		strcat(afc_f_slow_stop, "			 14688			");
		strcat(afc_f_slow_stop, "				3588			");
		strcat(afc_f_slow_stop, "				2488			");
		strcat(afc_f_slow_stop, "				13788			");
		strcat(afc_f_slow_stop, "				 2688			");
		strcat(afc_f_slow_stop, "				 1588			");
		strcat(afc_f_slow_stop, "				 14788		");
		strcat(afc_f_slow_stop, "				 13688		");
		strcat(afc_f_slow_stop, "				 125788		");
		strcat(afc_f_slow_stop, "				 1246788	");
		strcat(afc_f_slow_stop, "				 12356788 ");
		strcat(afc_f_slow_stop, "				 123456788");
		strcat(afc_f_slow_stop, "									");
		
		//												 1234567890123456789012345
		strcpy(afc_f_slow_stop_c, "123456788								");
		strcat(afc_f_slow_stop_c, " 1234567882							");
		strcat(afc_f_slow_stop_c, "	 1234567782							");
		strcat(afc_f_slow_stop_c, "		1234566782						");
		strcat(afc_f_slow_stop_c, "		 123455684						");
		strcat(afc_f_slow_stop_c, "			123445782						");
		strcat(afc_f_slow_stop_c, "			 12334684						");
		strcat(afc_f_slow_stop_c, "				1223586						");
		strcat(afc_f_slow_stop_c, "				 1124782					");
		strcat(afc_f_slow_stop_c, "					 13684					");
		strcat(afc_f_slow_stop_c, "						2586					");
		strcat(afc_f_slow_stop_c, "						1487					");
		strcat(afc_f_slow_stop_c, "						 378					");
		strcat(afc_f_slow_stop_c, "						 268					");
		strcat(afc_f_slow_stop_c, "						 158					");
		strcat(afc_f_slow_stop_c, "							48					");
		strcat(afc_f_slow_stop_c, "							38					");
		strcat(afc_f_slow_stop_c, "							28					");
		strcat(afc_f_slow_stop_c, "							18					");
		strcat(afc_f_slow_stop_c, "							18					");
		strcat(afc_f_slow_stop_c, "							38					");
		strcat(afc_f_slow_stop_c, "							58					");
		strcat(afc_f_slow_stop_c, "							78					");
		strcat(afc_f_slow_stop_c, "							88					");
		strcat(afc_f_slow_stop_c, "							88					");
		strcat(afc_f_slow_stop_c, "						 288					");
		strcat(afc_f_slow_stop_c, "						 488					");
		strcat(afc_f_slow_stop_c, "						 688					");
		strcat(afc_f_slow_stop_c, "						 788					");
		strcat(afc_f_slow_stop_c, "						 887					");
		strcat(afc_f_slow_stop_c, "						 886					");
		strcat(afc_f_slow_stop_c, "						2885					");
		strcat(afc_f_slow_stop_c, "						4884					");
		strcat(afc_f_slow_stop_c, "						6884					");
		strcat(afc_f_slow_stop_c, "						7883					");
		strcat(afc_f_slow_stop_c, "						8873					");
		strcat(afc_f_slow_stop_c, "						8862					");
		strcat(afc_f_slow_stop_c, "						8852					");
		strcat(afc_f_slow_stop_c, "					 28841					");
		strcat(afc_f_slow_stop_c, "					 48841					");
		strcat(afc_f_slow_stop_c, "					 6883						");
		strcat(afc_f_slow_stop_c, "					 7873						");
		strcat(afc_f_slow_stop_c, "					 8862						");
		strcat(afc_f_slow_stop_c, "					28852						");
		strcat(afc_f_slow_stop_c, "					48841						");
		strcat(afc_f_slow_stop_c, "					68841						");
		strcat(afc_f_slow_stop_c, "					7883						");
		strcat(afc_f_slow_stop_c, "				 28873						");
		strcat(afc_f_slow_stop_c, "				 48862						");
		strcat(afc_f_slow_stop_c, "				 68852						");
		strcat(afc_f_slow_stop_c, "				 78841						");
		strcat(afc_f_slow_stop_c, "				388741						");
		strcat(afc_f_slow_stop_c, "				58863							");
		strcat(afc_f_slow_stop_c, "				78853							");
		strcat(afc_f_slow_stop_c, "				88742							");
		strcat(afc_f_slow_stop_c, "				88642							");
		strcat(afc_f_slow_stop_c, "				87531							");
		strcat(afc_f_slow_stop_c, "				86431							");
		strcat(afc_f_slow_stop_c, "				8542							");
		strcat(afc_f_slow_stop_c, "				8432							");
		strcat(afc_f_slow_stop_c, "				8321							");
		strcat(afc_f_slow_stop_c, "				821								");
		strcat(afc_f_slow_stop_c, "				81								");
		strcat(afc_f_slow_stop_c, "				8									");
		strcat(afc_f_slow_stop_c, "				82								");
		strcat(afc_f_slow_stop_c, "				84								");
		strcat(afc_f_slow_stop_c, "				86								");
		strcat(afc_f_slow_stop_c, "				88								");
		strcat(afc_f_slow_stop_c, "				788								");
		strcat(afc_f_slow_stop_c, "				688								");
		strcat(afc_f_slow_stop_c, "				5788							");
		strcat(afc_f_slow_stop_c, "				46788							");
		strcat(afc_f_slow_stop_c, "				356788						");
		strcat(afc_f_slow_stop_c, "				24567788					");
		strcat(afc_f_slow_stop_c, "				1345667788				");
		strcat(afc_f_slow_stop_c, "				123455667788			");
		strcat(afc_f_slow_stop_c, "				 123445566788			");
		strcat(afc_f_slow_stop_c, "					1233445567788		");
		strcat(afc_f_slow_stop_c, "					 1223344566788	");
		strcat(afc_f_slow_stop_c, "							12334456788 ");
		strcat(afc_f_slow_stop_c, "								 123456788");
		strcat(afc_f_slow_stop_c, "													");
}
