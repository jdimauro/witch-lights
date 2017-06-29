#include <FastLED.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define NUM_LEDS             750
#define MAXSPRITES           10

#define NUM_COLORSETS         1
#define NUM_COLORS_PER_SET    9

#define PIR_SENSOR_1_PIN     2
#define PIR_SENSOR_2_PIN     3
#define PUSHBUTTON_PIN       13
#define NEOPIXEL_DATA_PIN    6                // Pin for neopixels

#define DEFAULT_COLOR        0x633051 

#define INFRARED_SENSOR_TIMEOUT_IN_MS   500

#define SCANNER_SPRITE_FRAME_DELAY_IN_MS  1

#define SCANNER_MIN_SCANS    2
#define SCANNER_MAX_SCANS    5

#define SCANNER_MIN_STOP_DISTANCE    20
#define SCANNER_MAX_STOP_DISTANCE    80

#define ANIMATION_FRAME_WIDTH     23
#define ANIMATION_FRAMES          28

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

// Function prototypes.
void resetStrip(void);
void debug(int);
void debugNeg(int);
void debugN(int, int);
void stripcpy(CRGB *, CRGB *, int, int, int);
void createColorsets(void);
void createAnimationFrames(void);


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
      if (millis() - lastUpdateTime >= SCANNER_SPRITE_FRAME_DELAY_IN_MS) {
        lastUpdateTime = millis();
        return true;
      } else {
        return false;
      }
    }

    void MarkDone() {
        this->done = true;
        // garbageCollector.ScheduleClean();
    }

    bool IsDone() {
        return this->done;
    } 

  protected:
    uint32_t lastUpdateTime;
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
                sprites[j] = NULL;
            }

            --this->count;

            return true;
        }
};

class W8V1ScannerDebrisV1Sprite : public Sprite {
  private:
    int currentPixel;
    bool isScanning;
    int scanningFrame;
    int nextInflection;
    int scanCount;
    int scanCountTotal;
    int velocity;

    // pattern is one black pixel plus remaining pixels in order of increasing brightness with brightest pixel doubled.
    CRGB pattern[NUM_COLORS_PER_SET + 1];
    int patternLength = NUM_COLORS_PER_SET + 1;

    void SetNextInflection() {
        nextInflection += random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }
    
  public:
    W8V1ScannerDebrisV1Sprite() : Sprite() {
        // Initial state.
        this->currentPixel = -1;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->nextInflection = 0;
        SetNextInflection();
        this->velocity = 1;
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();

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

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
               // garbageCollector->ScheduleCleaning();
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
    int currentPixel;
    bool isScanning;
    int scanningFrame;
    int nextInflection;
    int scanCount;
    int scanCountTotal;
    int velocity;

    // pattern is one black pixel plus remaining pixels in order of increasing brightness with brightest pixel doubled.
    CRGB pattern[NUM_COLORS_PER_SET + 1];
    int patternLength = NUM_COLORS_PER_SET + 1;

    void SetNextInflection() {
        nextInflection -= random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }
    
  public:
    W8V1ScannerDebrisV1ReverseSprite() : Sprite() {
        // Initial state.
        this->currentPixel = NUM_LEDS;  // The first pixel of the pattern is black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->nextInflection = NUM_LEDS;
        SetNextInflection();
        this->velocity = 1;
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();

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

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }

        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            scanCount = 0;
            currentPixel += 2;
            SetNextInflection();
            scanCount = GetNewScanCountTotal();
            leds[currentPixel + 6] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
        }

        if (! isScanning) {
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            currentPixel -= velocity;

            if (currentPixel <= nextInflection) {
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 3;
            }

            if (currentPixel <= -6) {
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


class ReverseScannerSprite : public Sprite {
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
        nextInflection -= random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }
    
  public:
    ReverseScannerSprite() : Sprite() {
        // Initial state.
        this->currentPixel = NUM_LEDS + 1;  // Both of the first two pixels of the pattern are black.
        this->scanningFrame = 0;
        this->isScanning = false;
        this->nextInflection = NUM_LEDS + 1;
        SetNextInflection();
        this->velocity = 2;
        this->scanCount = 0;
        this->scanCountTotal = GetNewScanCountTotal();

        // Choose a random color palette from the palettes available.
        int colorPalette = random(0, NUM_COLORSETS);

        // Set the colors in the pattern.
        this->pattern[5] = this->pattern[4] = colorSets[colorPalette][0];
        this->pattern[3] = colorSets[colorPalette][1];
        this->pattern[2] = colorSets[colorPalette][2];
        this->pattern[1] = colorSets[colorPalette][3];
        this->pattern[0] = colorSets[colorPalette][4];

        for (int i = 0; i < ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES; i++) {
            animationFramesReverse[i] = animationFramesCharsReverse[i] > ' ' ? colorSets[colorPalette][animationFramesChars[i] - '0'] : CRGB::Black;
        }
    }

    ~ReverseScannerSprite() {
    }

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }
        
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            scanCount = 0;
            currentPixel += 2;
            SetNextInflection();
            scanCount = GetNewScanCountTotal();
            leds[currentPixel + 6] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel + 12] = CRGB::Black; // I hate this too. Why is this necessary when going backwards but not forwards?
        }

        if (! isScanning) {
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            currentPixel -= velocity;

            if (currentPixel <= nextInflection) {
                isScanning = true;
                scanningFrame = 0;
                currentPixel -= 3;
            }

            if (currentPixel <= -6) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, animationFramesReverse + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
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

class GarbageCollectionNotifier {
  private:
    SpriteManager *manager;
    
  public:
    bool cleaningToDo;

    GarbageCollectionNotifier(SpriteManager *spriteManager) {
        this->manager = spriteManager;
    }

    // Sprites should schedule cleaning, but not clean themselves.
    void ScheduleCleaning() {
        cleaningToDo = true;
    }

        // Garbage collection. Remove any sprites that have finished their animation
    // from the SpriteVector, in order to make room for others.
    void Clean() {
        if (cleaningToDo) {
            manager->Clean();
            cleaningToDo = false;
        }
    }

};

InfraredSensor *sensor1;
InfraredSensor *sensor2;
Pushbutton *pushbutton;

GarbageCollectionNotifier *garbageCollector;
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
    garbageCollector = new GarbageCollectionNotifier(spriteManager);
    
    sensor1 = new InfraredSensor(PIR_SENSOR_1_PIN);
    sensor2 = new InfraredSensor(PIR_SENSOR_2_PIN);
    pushbutton = new Pushbutton(PUSHBUTTON_PIN);
  
    resetStrip();
}

int counter = 0;

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
    // if (random(0, 500) == 0) {
    //     spriteManager->Add(new W8V1ScannerDebrisV1ReverseSprite());
    // }
    // End (A).

    if (sensor1->IsActuated()) {
        spriteManager->Add(new W8V1ScannerDebrisV1Sprite());
    }

    if (sensor2->IsActuated()) {
        spriteManager->Add(new W8V1ScannerDebrisV1ReverseSprite());
    }

    spriteManager->Update();
    spriteManager->Clean();
    // garbageCollector->Clean();
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

void debugNeg(int number) {
    fill_solid(leds + NUM_LEDS - number, number < NUM_LEDS ? number : NUM_LEDS, CRGB::White);
}

void debugN(int startPos, int number) {
    fill_solid(leds + startPos, number < (NUM_LEDS - startPos) ? number : NUM_LEDS - startPos, CRGB::White);
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
/*    colorSets[0][0] = CRGB::Black;
    colorSets[0][1] = 0x180c14;
    colorSets[0][2] = 0x311828;
    colorSets[0][3] = 0x49243c;
    colorSets[0][4] = 0x633051;*/

    colorSets[0][0] = CRGB::Black;
    colorSets[0][1] = 0x010101;
    colorSets[0][2] = 0x020207;
    colorSets[0][3] = 0x03030e;
    colorSets[0][4] = 0x050517;
    colorSets[0][5] = 0x06061e;
    colorSets[0][6] = 0x080827;
    colorSets[0][7] = 0x09092e;
    colorSets[0][8] = 0x0a0a33;
//  colorSets[0][8] = 0x00ff00;


#if NUM_COLORSETS > 1
    colorSets[1][0] = CRGB::Black;
    colorSets[1][1] = 0x0d1503;
    colorSets[1][2] = 0x1b2a06;
    colorSets[1][3] = 0x294009;
    colorSets[1][4] = 0x36540c;
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

    
}

