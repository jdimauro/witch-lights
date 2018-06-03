#ifndef SPRITE_H_
#define SPRITE_H_

#include <Arduino.h>

#include "FastLED.h"

#include "animation.h"
#include "colorsets.h"
#include "hardware.h"

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

#define FAERIE_MIN_IDLE							1
#define FAERIE_MAX_IDLE							4

#define FAERIE_FLIT_MIN_DISTANCE		5
#define FAERIE_FLIT_MAX_DISTANCE		15

#define FAERIE_FLIT_MIN_START_INTERVAL	25
#define FAERIE_FLIT_MAX_START_INTERVAL	35

#define FAERIE_MIN_SPEED						1
#define FAERIE_MAX_SPEED 						10

#define FAERIE_MIN_BRAKE						96
#define FAERIE_MAX_BRAKE						112

#define FAERIE_MIN_WAIT							3
#define FAERIE_MAX_WAIT							9

#define FAERIE_MIN_TRAIL_LENGTH			80	// The lower the value, the longer the trail generated, but also the more FastLED functions get called per update per sprite. 
#define FAERIE_MAX_TRAIL_LENGTH			33

// currently set this to be consistent for animation design
#define FAERIE_MIN_LONG_TRAVEL			50	// This range is dead center in my first "no idle" zone, so any problems with the no-idle functions will show up quickly
#define FAERIE_MAX_LONG_TRAVEL			75	// 

#define SPRITE_STARTING_DELAY_INTERVAL_IN_MS	 50 // 40

#define NO_IDLE_LOOP_COUNT					7			// number of no-idle zones in the array


const bool randomInflection = false;	// Randomly makes faerie sprite dance back and forth, instead of mainly going "forwards". 


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

		bool UpdateNow() {
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
		bool done;
};

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
	int Blink();
	
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

		bool Update();
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

		bool Update();
};

class FaerieSprite : public Sprite {
private:
	int updateInterval;
	int minInterval;
	int maxInterval;
	int currentPixel;
	int currentLocation;
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
		
		// TODO when direction (1 or -1) is added as a sprite param on create, use that to make the sprite "advance" in the sprite's general direction(?)
		if (randomInflection && currentPixel > 60) {
			nextInflection += travelDistance * TravelDirectionSwitch();
			return;
		} else {
			nextInflection += travelDistance;
		}
	}

	int ReturnClosestPixel(int pixel, int minpix, int maxpix) {
		int minDistance = abs(pixel - minpix);
		int maxDistance = abs(pixel - maxpix);
		return (minDistance - maxDistance < 0) ? (minpix - random(3,6)) : (maxpix + random(3,6));
	}

	int CoerceTargetPixel(int targetPixel) {
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
			return .12;
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
	
	void StartTravel();

	bool UpdateTravel();

	bool StartIdle();

	bool UpdateIdle();
	
	bool StartWaiting() {
		isWaiting = true;
		waitCount = 0;
		waitCountTotal = SetWaitCount();
	}
	
	bool UpdateWaiting() {
		
	}

public:
	FaerieSprite() : Sprite() {
		// Initial state.
		this->currentPixel = -3;
		this->idlingFrame = 0;
		this->isIdling = false;
		this->isWaiting = false;
		this->lastInflection = 0;
		this->nextInflection = 0;
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

	int ReportLocation() {
		// return location to SpriteManager for collision detection
		// TODO: if drawDirection = -1, return the pixel that it's being "drawn" on; subtract currentPixel from NUM_LEDS
		
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

		bool Update();
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

		bool Update();
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

#endif