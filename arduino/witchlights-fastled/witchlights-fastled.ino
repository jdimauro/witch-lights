#include <FastLED.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif
 
// BEGIN BASIC OTA INCLUDES AND CONFIG

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "wifiauth.h"
 
// END BASIC OTA INCLUDES AND CONFIG


// TODO: fix the stupid random interval, that worked on an 8-bit arduino, this is an esp32.

// debug or animation modes
// TODO: set this with a jumper to an input pin
bool debugMode = false;					// turns on debug() statements
bool spawnLurkers = false;			// Not ready for Firefly 2018
bool randomInflection = false;	// Randomly makes faerie sprite dance back and forth, instead of mainly going "forwards". 
bool spawnFaeries = true;			// TODO Spawn a new faerie randomly; helpful to keep a constant background of sprite animation for evaluation
bool placeLurkers = false;			// TODO Dimly lights up range of pixels where lurkers are "allowed" to spawn, for install time
bool placeTrees = false;				// TODO Dimly lights up range of pixels green where trees are defined, also for installs
bool placeNoIdle = false;				// TODO same, for specifying zones where faeries will not stop to idle

// FastLED constants
#define NUM_LEDS							300 // 150 per 5-meter strip
#define MAXSPRITES						3

#define NUM_COLORSETS					6  // 6
#define NUM_COLORS_PER_SET		9

// Data pins
#define PIR_SENSOR_1_PIN		 10
#define PIR_SENSOR_2_PIN		 9
#define NEOPIXEL_DATA_PIN		 23								// Pin for neopixels (6 on Due, 23 on wireless stick)

// Sensor time-out (in production, set to 10000)
#define INFRARED_SENSOR_TIMEOUT_IN_MS		8000	// in milliseconds

#define SCANNER_SPRITE_FRAME_DELAY_IN_MS			1
#define TEST_PATTERN_FRAME_DELAY_IN_MS				24

#define FAERIE_MIN_IDLE							1
#define FAERIE_MAX_IDLE							6

#define FAERIE_FLIT_MIN_DISTANCE		5
#define FAERIE_FLIT_MAX_DISTANCE		25

#define FAERIE_FLIT_MIN_START_INTERVAL	20
#define FAERIE_FLIT_MAX_START_INTERVAL	45

#define FAERIE_MIN_SPEED						 8		// sets minInterval range
#define FAERIE_MAX_SPEED 						 20    // the larger this is the slower the maximum speed can be

#define FAERIE_MIN_BRAKE						60		// 96 for 50-60 px // sets maxInterval
#define FAERIE_MAX_BRAKE						116		// orig 112 or 86

#define FAERIE_MIN_WAIT							2
#define FAERIE_MAX_WAIT							6

#define FAERIE_MIN_TRAIL_LENGTH			 60	// The lower the value, the longer the trail generated, but also the more FastLED functions get called per update per sprite. 
#define FAERIE_MAX_TRAIL_LENGTH			 26

// currently set this to be consistent for animation design
#define FAERIE_MIN_LONG_TRAVEL			110	//
#define FAERIE_MAX_LONG_TRAVEL			180	// 

#define SPRITE_STARTING_DELAY_INTERVAL_IN_MS	 40 // 40

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
#define LURKER_MAX_PIXEL_1	149
#define LURKER_MIN_PIXEL_2	151
#define LURKER_MAX_PIXEL_2	280
#define LURKER_MIN_PIXEL_3	400
#define LURKER_MAX_PIXEL_3	450

/*
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
*/

// For testing use only. In production, set this equal to 1. Use this to exaggerate the acceleration effects. 10-20 is good for testing.
#define ACCELERATION_DELAY_OBVIOUSNESS_FACTOR				 1


#define ANIMATION_FRAME_WIDTH			1 //23
#define ANIMATION_FRAMES					1 //28




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


// Function prototypes.
void resetStrip(void);
void debug(int);
void debugNeg(int);
void debugN(int, int);
void stripcpy(CRGB *, CRGB *, int, int, int);
void createColorsets(void);

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
		return random(4,5 +1);
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

// The major sprite for Witch Lights animation 
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
	int minNoIdle[3] = {  1, 145, 295 };
	int maxNoIdle[3] = {  6, 155, 305 };

	CRGB pattern[3];
	int patternLength = 3;

	// use updateInterval and map() to make tails longer when going faster (reduce the fade factor) and shorter when going slower (increase fade factor)
	int SetDimFactor(int interval) {
		return map(interval, 0, SPRITE_STARTING_DELAY_INTERVAL_IN_MS, trailLength, 192); // 192 is a max dimfactor?
	}

	int SetTrailLength() {
		return random(FAERIE_MAX_TRAIL_LENGTH, FAERIE_MIN_TRAIL_LENGTH) + 1;
	}

	// TODO define global constants for brake and acceleration factor ranges
	// higher numbers = sharper braking
	float SetBrakeFactor(bool waiting) {
		if (!waiting) {
			return (random(500,1200) + 1) / 100; // originally 500, 900, +1
		} else {
			return (random(9000,17000) + 1) / 100; // 9000, 19000
		}
	}
	
	// higher numbers = steeper acceleration curve
	float SetAccelerationFactor(bool waiting) {
		if (!waiting) {
			return (random(120,290) + 1) / 100; // to slow acceleration, decrease this a little? orig 100,350 +1
		} else {
			return (random(400,1100) + 1) / 100; // orig 400,1000
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
		if (random(10) +1 < 6) {
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
			return .07;	// .12 worked for 50-75 pixel moves, too big for 160 pixel moves; .06 from Witch Lights
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
		// nextInflection = AvoidNoIdle(nextInflection);
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
			/*  // a chance to change colors?
			if (random(5) == 0) {
				// random color palette
				this->colorPalette = random(0, NUM_COLORSETS);
			} */
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

// BEGIN OTA SETUP

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with its md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

// END OTA SETUP



		createColorsets();
		// createAnimationFrames();
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

	// BEGIN OTA LOOP

  ArduinoOTA.handle();

	// END OTA LOOP

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
		/*if (random(0,1000) == 0 && spawnLurkers) {
			// debug(3);
			int lurkerSpawnPixel = random(40,225); // TODO - create array of lurker zones to use instead of the constants?
			Sprite *s1 = new LurkerSprite(lurkerSpawnPixel,1); 
			// TODO: check to see if another lurker already exists at this pixel, despawn if so
		
			if (! spriteManager->Add(s1)) {
				delete s1;
			} 
		} else {
			// debug(1);
		} */
		
		// 3200000 = perfect for esp32 in production
		// 1960000 = almost twice as frequent for animation analysis?
		if (random(0,696000) == 0 && spawnFaeries) {

			for (int i = 0; i <= random(MAXSPRITES) + 1; i++) {
				Sprite *s1 = new FaerieSprite(1, -3); 
			
				if (! spriteManager->Add(s1)) {
					delete s1;
				}
			}
			
		} else if (random(0,696000) == 0 && spawnFaeries) {
			for (int i = 0; i <= random(MAXSPRITES) + 1; i++) {
				Sprite *s2 = new FaerieSprite(-1, NUM_LEDS + 3); 
			
				if (! spriteManager->Add(s2)) {
					delete s2;
				}
				// debug(1);
			}
		}

		/*
		PHASE Lights aren't running sensors for FF2023

		if (sensor1->IsActuated()) {
			Sprite *s1 = new FaerieSprite(1, -3); // production

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
		*/

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
// Blue 2.0
		colorSets[0][0] = 0x000000;
		colorSets[0][1] = 0x000000;
		colorSets[0][2] = 0x000000;
		colorSets[0][3] = 0x000101;
		colorSets[0][4] = 0x000203;
		colorSets[0][5] = 0x010507; // 02070A
		colorSets[0][6] = 0x030A0F;
		colorSets[0][7] = 0x06151E;
		colorSets[0][8] = 0x0C2A3C;

// Blue 2.5
#if NUM_COLORSETS > 1
		colorSets[1][0] = 0x000000;
		colorSets[1][1] = 0x000000;
		colorSets[1][2] = 0x000000;
		colorSets[1][3] = 0x000001;
		colorSets[1][4] = 0x000003;
		colorSets[1][5] = 0x010106; // 02070A
		colorSets[1][6] = 0x02020C;
		colorSets[1][7] = 0x050519;
		colorSets[1][8] = 0x0A0A33;

#endif

// Yellow-green.
#if NUM_COLORSETS > 2
		colorSets[2][0] = 0x000000;
		colorSets[2][1] = 0x000000;
		colorSets[2][2] = 0x000100;
		colorSets[2][3] = 0x010200;
		colorSets[2][4] = 0x030500;
		colorSets[2][5] = 0x060A01;
		colorSets[2][6] = 0x0D1503;
		colorSets[2][7] = 0x1B2A06;
		colorSets[2][8] = 0x36540C;
#endif

#if NUM_COLORSETS > 3
// Amber.
		colorSets[3][0] = 0x000000;
		colorSets[3][1] = 0x000000;
		colorSets[3][2] = 0x000000;
		colorSets[3][3] = 0x010100;
		colorSets[3][4] = 0x030300;
		colorSets[3][5] = 0x060601;
		colorSets[3][6] = 0x0C0C02;
		colorSets[3][7] = 0x191905;
		colorSets[3][8] = 0x33330A;
#endif

#if NUM_COLORSETS > 4
// Reddish-purple.
		colorSets[4][0] = 0x000000;
		colorSets[4][1] = 0x000000;
		colorSets[4][2] = 0x010001;
		colorSets[4][3] = 0x030102;
		colorSets[4][4] = 0x060204;
		colorSets[4][5] = 0x0C0508;
		colorSets[4][6] = 0x180A11;
		colorSets[4][7] = 0x311523;
		colorSets[4][8] = 0x632A47;
#endif

#if NUM_COLORSETS > 5
// Original purple.
		colorSets[5][0] = 0x000000;
		colorSets[5][1] = 0x000000;
		colorSets[5][2] = 0x010001;
		colorSets[5][3] = 0x030102;
		colorSets[5][4] = 0x060305;
		colorSets[5][5] = 0x0C060A;
		colorSets[5][6] = 0x180C14;
		colorSets[5][7] = 0x311828;
		colorSets[5][8] = 0x633051;
#endif
}
