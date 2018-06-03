#include "colorsets.h"
#include "hardware.h"
#include "sprite.h"
#include "spritemanager.h"
#include "spritevector.h"
#include "infraredsensor.h"

#include "FastLED.h"

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

// debug or animation modes
// TODO: set this with a jumper to an input pin
bool spawnLurkers = true;				// IMPORTANT: set to FALSE for all public video before Firefly 2018!
bool spawnFaeries = true;				// TODO Spawn a new faerie randomly; helpful to keep a constant background of sprite animation for evaluation
bool placeLurkers = false;			// TODO Dimly lights up range of pixels where lurkers are "allowed" to spawn, for install time
bool placeTrees = false;				// TODO Dimly lights up range of pixels green where trees are defined, also for installs
bool placeNoIdle = false;				// TODO same, for specifying zones where faeries will not stop to idle

// Data pins
#define PIR_SENSOR_1_PIN		 3
#define PIR_SENSOR_2_PIN		 4
#define NEOPIXEL_DATA_PIN		 6								// Pin for neopixels

// 1st 5: ban idling across the very ends of LED strips, it looks weird
#define NO_IDLE_MIN_1								145
#define NO_IDLE_MAX_1								155

#define NO_IDLE_MIN_2								295
#define NO_IDLE_MAX_2								305

#define NO_IDLE_MIN_3								445
#define NO_IDLE_MAX_3								455

#define NO_IDLE_MIN_4								595
#define NO_IDLE_MAX_4								605

#define NO_IDLE_MIN_5								745
#define NO_IDLE_MAX_5								755


// Set these values to define areas (such as behind trees) where faeries should not idle
#define NO_IDLE_MIN_6								80
#define NO_IDLE_MAX_6								111

#define NO_IDLE_MIN_7								0
#define NO_IDLE_MAX_7								42

#define NO_IDLE_MIN_8								718
#define NO_IDLE_MAX_8								750

#define NO_IDLE_MIN_9								-1
#define NO_IDLE_MAX_9								-2

#define NO_IDLE_MIN_10							-3
#define NO_IDLE_MAX_10							-4

/*
// Set starting range of any no_idle zones here
int minNoIdle[] = { 
	0, 
	80,
	145,
	295,
	445,
	595,
	718
};
*/

/*
// Set end range of no_idle zones here
int maxNoIdle[] = { 
	42, 
	111,
	155,
	305,
	455,
	605,
	750
};
*/


// Function prototypes.
void resetStrip(void);
void createColorsets(void);
void createAnimationFrames(void);

// TODO: State machine blink sprites that spawn in defined areas when TravelSprites pass through them. "Spirits awaken" 

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
					Sprite *s1 = new FaerieSprite();
				}

				return;
		}

		// Spawn lurkers randomly
		if (random(0,1000) == 0 && spawnLurkers) {
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
		
		if (random(0,1000) == 0 && spawnFaeries) {
			Sprite *s1 = new FaerieSprite(); 
		
			if (! spriteManager->Add(s1)) {
				delete s1;
			}
			
		} else {
			// debug(1);
		}

		if (sensor1->IsActuated()) {
			debug(1);
			Sprite *s1 = new FaerieSprite();

			if (! spriteManager->Add(s1)) {
					delete s1;
			}
		}

		if (sensor2->IsActuated()) {
				//Sprite *s2 = new W8V1ScannerDebrisV1ReverseSprite();
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

