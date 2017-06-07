#include <FastLED.h>
#define NUM_LEDS	30
#define DATA_PIN	6

CRGB leds[NUM_LEDS];
LinearSprite *linearSprite;

class Sprite {
public:
	Sprite() {
	}
	
	virtual bool Update() = 0;
}

class LinearSprite : public Sprite {
protected:
	uint32_t lastUpdateTime;
	
	int currentPixel;
    CRGB color;	

public:
    virtual bool Update() {
		if (millis() - lastUpdateTime > 30) {
			lastUpdateTime = millis();
			FastLED.show();

	    	leds[currentPixel++] = CRGB::Black;
			leds[currentPixel] = color;	
		}
    }	
}

/* ***************** */

void setup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
    	leds[i] = CRGB::Black;
    }

    FastLED.show();
		
	sprite = new LinearSprite();
}

void loop() {
    sprite->Update();
}