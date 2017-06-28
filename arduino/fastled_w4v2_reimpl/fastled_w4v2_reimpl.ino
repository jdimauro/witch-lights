#include <FastLED.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define NUM_LEDS             750
#define MAXSPRITES           2

#define PIR_SENSOR_1_PIN     2
#define PIR_SENSOR_2_PIN     3
#define PUSHBUTTON_PIN       13
#define NEOPIXEL_DATA_PIN    6                // Pin for neopixels

#define DEFAULT_COLOR        0x633051 

#define INFRARED_SENSOR_TIMEOUT_IN_MS   500

#define SCANNER_MIN_SCANS    2
#define SCANNER_MAX_SCANS    5

#define SCANNER_MIN_STOP_DISTANCE    6
#define SCANNER_MAX_STOP_DISTANCE    14

#define ANIMATION_FRAME_WIDTH     17
#define ANIMATION_FRAMES          8

// ...TO HERE.

// Array representing the strip.
CRGB leds[NUM_LEDS];
// -------
// By defining a single element in memory which SHOULD BE immediately before the large pixel array,
// I've effectively created the (-1)th element of the array. This frees me from having to check
// that I'm writing pixels to that area and will unknowingly blow something up, and saves a costly
// comparison (which is false ALMOST always) each time through each Sprite's Update() method.
// TODO: Do this properly with malloc()/calloc().

CRGB pattern[6];
int patternLength = 6;

CRGB c[5];
char animationFramesChars[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB animationFrames[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];


// Function prototypes.
void resetStrip(void);
void debug(void);
void stripcpy(CRGB *, CRGB *, int, int, int);

class InputDevice {
  public:
    virtual bool IsActuated() = 0;
    uint32_t lastPollTime;
    
protected:
    int _pinNumber;
};

class InfraredSensor : public InputDevice {
public:
    InfraredSensor(int pinNumber) {
        this->_pinNumber = pinNumber;
    }

    // Put sensor read code here. Return true if triggered, false otherwise.
    bool IsActuated() {
        if (millis() - this->lastPollTime < INFRARED_SENSOR_TIMEOUT_IN_MS) {
            return false;
        }

        // Josh: put whatever sensor check you need in the "if" condition here.
        // TODO Should this be analogRead?
        if (digitalRead(this->_pinNumber) == HIGH) {
            // Test pixel to indicate when the button's been pressed. Feel free to remove this when you like.
            leds[29] = CRGB::Red;

            

            // Make sure these stay at the end of the sensor "if" block. This will set the last polling time to 
            // ensure that the sensor is properly "debounced".
            this->lastPollTime = millis();
            return true;
        }        

        return false;
    }
};

class Pushbutton : public InputDevice {
  public:
    Pushbutton(int pinNumber) {
        this->_pinNumber = pinNumber;
    }

    bool IsActuated() {
        if (millis() - this->lastPollTime < 1000) {
            return false;
        }

        if (digitalRead(this->_pinNumber) == HIGH) {
            leds[12] = CRGB::Red;

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
      if (millis() - lastUpdateTime >= 0) {
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
    boolean done;
};

/*
class LinkedList {
    public:
        LinkedListNode* first;

        bool Add(Sprite *sprite) {
            // Order doesn't so much matter for us here, so it'll just be easier to put it FIRST.
            LinkedListNode *node = new LinkedListNode(sprite, this->first);   
        }

        bool Remove(Sprite *sprite) {
            // Traverse the list until we find it.

            LinkedListNode *i;

            while (i->next != NULL) {
                if (i->content == sprite) {
                    // Found it.
                    i->
                }
            }
        }
}

class LinkedListNode {
    public: 
        Sprite *content;
        LinkedListNode* next;

        LinkedListNode(Sprite *ptr, LinkedListNode *nxt) {
            this->content = ptr;
            this->next = nxt;
        }
}
*/

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
                sprites[j] = NULL;
            }

            --this->count;

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

    void SetNextInflection() {
        nextInflection += random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }
    
  public:
    ScannerSprite() : Sprite() {
        this->currentPixel = 0;
        this->scanningFrame = 0;
        this->isScanning = false;
        this->nextInflection = 0;
        this->velocity = 2;
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();
        SetNextInflection();
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
            leds[currentPixel - 6] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
        }

        if (! isScanning) {
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            currentPixel += velocity;

            if (currentPixel >= nextInflection) {
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 8;
            }
        } else {
            stripcpy(leds, animationFrames + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
            }
        }

        return true;
    }
};

class W4V1ScannerSprite : public Sprite {
  private:
    int currentPixel;
    bool scanning;
    bool hasScanned;
    int scannerCount;

    CRGB regularFlow[4] = { 0x400000, 0x800000, 0xc00000, 0xff0000 };

    const CRGB red0 = 0x000000;
    const CRGB red4 = 0x400000;
    const CRGB red8 = 0x800000;
    const CRGB redC = 0xC00000;
    const CRGB redF = 0xFF0000;

    CRGB debris[78] = { red0, red0, red0, red0, red0, red0, red0, redF, redC, red8, redC, red0, red0,    // frame 10
                        red0, red0, red0, red0, red0, redF, redC, red8, red4, red0, red4, red8, red0,    // frame 11
                        red0, red0, red0, redF, redC, red8, red4, red0, red0, red0, red0, red0, red4,    //       12
                        red0, red0, redC, red8, redC, redF, red0, red0, red0, red0, red0, red0, red0,    //       13
                        red0, red8, red4, red0, red4, red8, redC, redF, red0, red0, red0, red0, red0,    //       14
                        red4, red0, red0, red0, red0, red0, red4, red8, redC, redF, red0, red0, red0     //       15
                      };


  public:
    W4V1ScannerSprite() : Sprite() {
      this->currentPixel = 0;
      this->scanning = false;
      this->hasScanned = false;
      this->scannerCount = 0;
    }

    ~W4V1ScannerSprite() {
    }

    bool Update() {
      if (this->UpdateNow()) {
        if (this->currentPixel >= NUM_LEDS + 3 || this->currentPixel == 0) {
            currentPixel = 0;
            scannerCount = 0;

            memset(leds, 0x00, NUM_LEDS * sizeof(CRGB));
        }

        if (scanning) {
            if (scannerCount >= 6 * 5) {
                memset(leds + 8, 0x00, 13 * sizeof(CRGB));
                scanning = false;
                currentPixel = 18;
            }
        }

        if (! scanning) {
            if (currentPixel >= 0 && currentPixel <= 3) {
                memcpy(leds, regularFlow + (3 - currentPixel), (currentPixel + 1) * sizeof(CRGB));
            } else if (scannerCount == 0 && (currentPixel == 15 || currentPixel == 16)) {
                memcpy(leds + (currentPixel - 3), regularFlow, 4 * sizeof(CRGB));
                leds[currentPixel - 4] = CRGB::Black;
                leds[currentPixel - 5] = CRGB::Black;
                scanning = true;                
            } else if (this->currentPixel >= 4 && this->currentPixel < NUM_LEDS) {
                memcpy(leds + (currentPixel - 3), regularFlow, 4 * sizeof(CRGB));
                leds[currentPixel - 4] = CRGB::Black;
                leds[currentPixel - 5] = CRGB::Black;
            } else if (currentPixel >= NUM_LEDS) {
                memcpy(leds + (currentPixel - 3), regularFlow, (33 - currentPixel) * sizeof(CRGB));
                leds[currentPixel - 4] = CRGB::Black;
                leds[currentPixel - 5] = CRGB::Black;
            }

            currentPixel += 2;
        } else {
            memcpy(leds + 8, debris + (13 * (scannerCount % 6)), 13 * sizeof(CRGB));
            ++scannerCount;
        }

        return true;
      } else {
        return false;
      }
    }
};

class W1V1Sprite : public Sprite {
  private:
    int currentPixel;
    CRGB color;

  public:
    W1V1Sprite() {
        this->currentPixel = -1;
        this->color = DEFAULT_COLOR;
    }

    W1V1Sprite(int startPixel, CRGB startColor) {
        // Minus one because we'll increment it as the first step in Update().
        this->currentPixel = startPixel - 1;  
        this->color = startColor;
    }

    ~W1V1Sprite() {
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
        
            // No need to clean unless something got updated.
            this->Clean();
        }
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
Pushbutton *pushbutton;

SpriteManager *spriteManager;
bool isBooted;
bool testSpritesCreated;

int starttime = millis();

void setup() {
    pattern[0] = pattern[1] = c[0] = CRGB::Black;
    pattern[2] = c[1] = 0x330000;
    pattern[3] = c[2] = 0x770000;
    pattern[4] = c[3] = 0xbb0000;
    pattern[5] = c[4] = 0xff0000;

    strcpy(animationFramesChars, "          1234   ");
    strcat(animationFramesChars, "           4323  ");
    strcat(animationFramesChars, "         4321  2 ");
    strcat(animationFramesChars, "       4321     1");
    strcat(animationFramesChars, "     4321        ");
    strcat(animationFramesChars, "    3234         ");
    strcat(animationFramesChars, "   2  1234       ");
    strcat(animationFramesChars, "  1     1234     ");

    for (int i = 0; i < ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES; i++) {
        animationFrames[i] = animationFramesChars[i] > ' ' ? c[animationFramesChars[i] - '0'] : CRGB::Black;
    }
  
    isBooted = false;
    testSpritesCreated = false;
  
    randomSeed(analogRead(0));
   
    spriteManager = new SpriteManager();
    
    sensor1 = new InfraredSensor(PIR_SENSOR_1_PIN);
    sensor2 = new InfraredSensor(PIR_SENSOR_2_PIN);
    pushbutton = new Pushbutton(PUSHBUTTON_PIN);
  
    resetStrip();
}

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
    if (random(0, 5000) == 0) {
        spriteManager->Add(new ScannerSprite());
    }
    // End (A).

    if (sensor1->IsActuated()) {
        // Add sprite.
        spriteManager->Add(new W1V1Sprite());
    }

    if (sensor2->IsActuated()) {
        // Add sprite.
        spriteManager->Add(new W1V1Sprite());
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
    fill_solid(leds, number < NUM_LEDS ? number : NUM_LEDS, CRGB::White);
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

