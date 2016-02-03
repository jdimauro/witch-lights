#include <Adafruit_NeoPixel.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy (???)
#endif

#define PIN    6                // Pin for neopixels
#define N_LEDS 750              // 5 5-meter reels hooked up so far    

int ledPin = 13;                // choose the pin for the LED
int loc0Pin = 3;                // choose the input pin (for PIR sensor)
int loc1Pin = 4;
int pir0State = LOW;            // we start, assuming no motion detected
int pir1State = LOW;
int loc0val = 0;                // variable for reading the pin status
int loc1val = 1;

int hoverTime;				          // how long the sprite hovers for
int hoverLowerTime = 5;         // shortest possible hoverTime
int hoverUpperTime = 30;        // longest possible hoverTime
int hoverLowerDistance = 120;   // shortest possible pause distance
int hoverUpperDistance = 240;   // longest possible pause distance
                                // TODO: make hover distance adjustable via a knob

int hoverRandom;
int dir = 1;					          // +1 to move forwards, -1 to move backwards

long interval = 35;             // delay between pixel draws -- 45 is slower than you can walk away from
long minInterval = 0;           // 
      
boolean debug = false;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup()                    // special function, runs on boot
{
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(loc0Pin, INPUT);     	// declare sensor as input
  pinMode(loc1Pin, INPUT);
	digitalWrite(ledPin, LOW);
  randomSeed(analogRead(0));
  
  
  strip.begin();
  strip.show();
  if (debug) {
    Serial.begin(9600);
    Serial.println("Started!");
  }
}
 
void loop()                      // special function
{
	// unsigned long currentMillis = millis();
	
  loc0val = digitalRead(loc0Pin);  	// read input loc0value
  
  if (loc0val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    if (pir0State == LOW) {
      if (debug) { Serial.println("moving to 150"); }
      dir = 1;
      // we have just turned on
      chase(0x633051);
      // We only want to print on the output change, not state
      pir0State = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); 	// turn LED OFF
    if (pir0State == HIGH){
      
      // we have just turned off
      // We only want to print on the output change, not state
      pir0State = LOW;
    }
  }
	
  loc1val = digitalRead(loc1Pin);  	// read input loc0value
  if (loc1val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  		// turn LED ON
    if (pir1State == LOW) {
      if (debug) { Serial.println("moving to 0"); }
      dir = -1;
      // we have just turned on
      chase(0x552255);
      // We only want to print on the output change, not state
      pir1State = HIGH;
      dir = 1;
    }
  } else {
    digitalWrite(ledPin, LOW); 			// turn LED OFF
    if (pir1State == HIGH){
      
      // we have just turned off
      // We only want to print on the output change, not state
      pir1State = LOW;
    }
  }
	
}


void chase(uint32_t c) 
{
    if (debug) { Serial.println("Starting the chase!"); }
    int locPos = (dir==1) ? 0 : N_LEDS;
	  int destPixel = locPos + (random(hoverLowerDistance,hoverUpperDistance) + 3) * dir;
    uint16_t locEnd = (dir==1) ? N_LEDS : 0;
        
	  strip.show();
	
	  while (locPos != locEnd) {              // WARNING: comparison between signed and unsigned integer expressions 
          if (debug) {
            Serial.print("locPos ");
            Serial.print(locPos);
            Serial.print(" locEnd ");
            Serial.println(locEnd);
          }
        moveFromToLocation(locPos, destPixel, c);
        locPos = destPixel;
 
  	    hoverRandom = random(hoverLowerDistance,hoverUpperDistance);
  	    destPixel = locPos + hoverRandom * dir;
        destPixel = (destPixel < 0) ? 0 : destPixel;
        destPixel = (destPixel > N_LEDS) ? N_LEDS : destPixel;
        hoverTime = random(hoverLowerTime,hoverUpperTime);

  	    for (int j=0; j<hoverTime; j++) 
        {  //do 10 cycles of chasing
          // I really want the sparkle to move back and forth while waiting
            for (int q=0; q < 3; q++) 
            {
              setPixelUsingExistingColor(locPos-1*dir);
              setPixelUsingExistingColor(locPos-2*dir);
              setPixelUsingExistingColor(locPos-3*dir);
              setPixelUsingExistingColor(locPos-4*dir);
              setPixelUsingExistingColor(locPos-5*dir);
              setPixelUsingExistingColor(locPos-6*dir);
              setPixelUsingExistingColor(locPos-7*dir);
              setPixelUsingExistingColor(locPos-8*dir);

      		    for (int i=locPos; i < locPos+3; i=i+3) 
              {
      		      setPixelColor(i+q*dir, c);    //turn every third pixel on
      		    }
      		    strip.show();
   
      		    delay(random(5,interval));
   
      		    for (int i=locPos; i < locPos+3; i=i+3) {
      		      setPixelColor(i+q*dir, 0x111111);        //turn every third pixel off
      		    }
  	       }
  	     }
       locPos = (locPos <= 0) ? 0 : locPos; 
       locPos = (locPos > N_LEDS) ? N_LEDS : locPos;
       if (debug) { 
         Serial.print("Finished moving new position ");
         Serial.print(locPos);
         Serial.print(" moving to ");
         Serial.println(locEnd);
       }
	}  
}


void moveFromToLocation(uint16_t locStart, uint16_t locEnd, uint32_t color) 
{
  long currentInterval = interval;
  uint16_t locPos = locStart;
  while(locPos != locEnd) {
    setPixelColor(locPos, color); // Draw new pixel
    setPixelUsingExistingColor(locPos-1*dir);
    setPixelUsingExistingColor(locPos-2*dir);
    setPixelUsingExistingColor(locPos-3*dir);
    setPixelUsingExistingColor(locPos-4*dir);
    setPixelUsingExistingColor(locPos-5*dir);
    setPixelUsingExistingColor(locPos-6*dir);
    setPixelUsingExistingColor(locPos-7*dir);
    
    strip.show();
    delay(currentInterval);
    if(currentInterval > minInterval)
    {
      currentInterval -= 1;
    }
    locPos += dir;
  }
}

void setPixelUsingExistingColor(uint16_t loc)
{
  if (loc > 0 && loc < N_LEDS) {
    setPixelColor(loc, DimColor(strip.getPixelColor(loc)));
  }
}

void setPixelColor(uint16_t loc, uint32_t color)
{
  if (loc > 0 && loc < N_LEDS) {
    if (debug) { Serial.println(loc); }
    strip.setPixelColor(loc, color);
  }
}

// Calculate 50% dimmed version of a color (used by ScannerUpdate)
uint32_t DimColor(uint32_t color)
{
    // Shift R, G and B components one bit to the right
    uint32_t dimColor = strip.Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
    return dimColor;
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color)
{
    return color & 0xFF;
}
