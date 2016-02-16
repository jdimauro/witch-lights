#include <utility.h>
#include <unwind-cxx.h>
#include <StandardCplusplus.h>
#include <vector>
#include <system_configuration.h>

using namespace std;

#include <MemoryFree.h>

#include <Adafruit_NeoPixel.h>

// JOSH: FEEL FREE TO EDIT CONSTANT VALUES HERE...

// Hardware configuration.
#define NEOPIXEL_DATA_PIN    6                // Pin for neopixels
#define PIR_SENSOR_1_PIN     3
#define PIR_SENSOR_2_PIN     4
#define PUSHBUTTON_PIN       2

#define N_LEDS              30               // 30/meter test reel    

// Sensor timeouts.
#define INFRARED_SENSOR_TIMEOUT_IN_MS  2000
#define PUSHBUTTON_TIMEOUT_IN_MS  1000

// The maximum number of sprites run simultaneously.
#define MAXSPRITES           7

// "Framerate" minimum and maximum period for animations.
#define MIN_UPDATE_INTERVAL_IN_MS  15
#define MAX_UPDATE_INTERVAL_IN_MS  40

// ScannerSprite parameters.
#define MIN_PIVOT_COUNT 2                    // Josh: reset to 5
#define MAX_PIVOT_COUNT 10                    // Josh: reset to 40
#define MIN_LEAP_DISTANCE_BETWEEN_PIVOTS 5   // Josh: reset to 20
#define MAX_LEAP_DISTANCE_BETWEEN_PIVOTS 15   // Josh: reset to 80
#define SCANNER_WIDTH  9        // Must be an odd number.
#define MIN_SCANNER_WIDTH  7    // Must be an odd number.
#define MAX_SCANNER_WIDTH  13   // Must be an odd number.

// ...TO HERE.

#define DARK    0x000000
#define WHITE   0xffffff

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);

class IDSequence {
public:
    IDSequence() {
        this->nextValue = 1;
    }
    
    int GetNextValue() {
        return this->nextValue;
    }
    
private:
    int nextValue;
};

class InputDevice {
public:
    virtual bool IsActuated() = 0;

public:
    uint32_t lastPollTime;
    
protected:
    int _pinNumber;
};

IDSequence sequence;

class InfraredSensor : public InputDevice {
public:
    bool IsActuated() {
        // Put sensor read code here. Return true if triggered, false otherwise.
        if (millis() - this->lastPollTime < INFRARED_SENSOR_TIMEOUT_IN_MS) {
            return false;
        }

        if (digitalRead(this->_pinNumber) == HIGH) {
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
        
        pinMode(this->_pinNumber, INPUT);     // declare sensor as input
    }

    bool IsActuated() {
        if (millis() - this->lastPollTime < PUSHBUTTON_TIMEOUT_IN_MS) {
            return false;
        }
    
        if (digitalRead(this->_pinNumber) == HIGH) {
            this->lastPollTime = millis(); 
            return true;
        }

        return false;    
    }
};

class Sprite {
public:
    Sprite() {
        this->id = sequence.GetNextValue();
        MarkUpdated();
        done = false;
        SetNextUpdateTime();
    }
    
    virtual void Update() = 0;

    boolean UpdateNow() {
        return (millis() - lastUpdateTime >= GetNextUpdateTime());
    }
    
    void MarkUpdated() {
        lastUpdateTime = millis();
        SetNextUpdateTime();
    }

    uint32_t GetNextUpdateTime() {
        return nextUpdateTime;
    }

    virtual void SetNextUpdateTime() {
        nextUpdateTime = random(MIN_UPDATE_INTERVAL_IN_MS, MAX_UPDATE_INTERVAL_IN_MS);  
    }
    
    boolean IsDone() {
        return done;
    }
    
    void MarkDone() {
        done = true; 
    }
    
    int getID() {
        return sequence.GetNextValue(); 
    }
    
protected:
    uint32_t lastUpdateTime;
    boolean done;
    uint16_t id;
    uint32_t nextUpdateTime;
};

class ForwardScannerSprite : public Sprite {
private:
    uint32_t pixelColor;
    int currentPixel;
    int targetPivot;
    int pivotCount;
    bool pivoting;
    int generatedPivotTotal = MIN_PIVOT_COUNT;
    int scannerWidth;

    // The number of the pixel for the purposes of the scanner. A PIVOT_WIDTH wide scanner
    // should cause this number to range from 1 to (PIVOT_WIDTH - 1) * 2.
    int pivotPixel;
    
    int GetNextPivotPosition(int currentPivot) {
        return currentPivot + random(MIN_LEAP_DISTANCE_BETWEEN_PIVOTS, MAX_LEAP_DISTANCE_BETWEEN_PIVOTS) + 3;
    }

    int getScannerPixelPosition(int _targetPivot, int _pivotWidth, int _pivotPixel) {
        // Examples:
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 1
        //         ..........====T*===........... returns 16
        
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 4
        //         ..........====T===*........... returns 19
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 5
        //         ..........====T==*=........... returns 18
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 11
        //         ..........=*==T====........... returns 12
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 12
        //         ..........*===T====........... returns 11
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 16
        //         ..........====*====........... returns 15
    
        int farSide = (_pivotWidth - 1) / 2;
        int nearSide = farSide + _pivotWidth - 1;
    
        if (_pivotPixel <= farSide) {
            return _targetPivot + _pivotPixel;
        } else if (_pivotPixel > farSide && _pivotPixel <= nearSide) {
            return _targetPivot + 2 * farSide - _pivotPixel;
        } else {
            return _targetPivot - (2 * (_pivotWidth - 1) - _pivotPixel); 
       }
    }
    
public:
    ForwardScannerSprite(uint32_t _color) {
        pixelColor = _color;
        currentPixel = 0;
        targetPivot = GetNextPivotPosition(currentPixel);
        pivoting = false;
        pivotCount = 0;
        generatedPivotTotal = random(MIN_PIVOT_COUNT, MAX_PIVOT_COUNT);
        scannerWidth = MIN_SCANNER_WIDTH + 2 * random(0, 1 + (MAX_SCANNER_WIDTH - MIN_SCANNER_WIDTH) / 2);
        
        pivotPixel = 0;
    }
    
    ~ForwardScannerSprite() {
    }
    
    virtual void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }

        if (currentPixel >= strip.numPixels() + 1) {
            MarkDone();
            return;
        }

        if (! (pivoting) && currentPixel < targetPivot) {
            strip.setPixelColor(currentPixel, pixelColor);
            strip.setPixelColor(currentPixel - 1, DimColor(strip.getPixelColor(currentPixel)));
            strip.setPixelColor(currentPixel - 2, DimColor(strip.getPixelColor(currentPixel - 1)));
            strip.setPixelColor(currentPixel - 3, DimColor(strip.getPixelColor(currentPixel - 2)));
            strip.setPixelColor(currentPixel - 4, DARK);
            currentPixel++;
        } else if (! (pivoting) && currentPixel == targetPivot) {
            // Reached the next pivot!
            pivoting = true;
            pivotPixel = 1;
            strip.setPixelColor(targetPivot, pixelColor);
            strip.setPixelColor(targetPivot - 1, DimColor(strip.getPixelColor(targetPivot)));
            strip.setPixelColor(targetPivot - 2, DimColor(strip.getPixelColor(targetPivot - 1)));
            strip.setPixelColor(targetPivot - 3, DimColor(strip.getPixelColor(targetPivot - 2)));
            strip.setPixelColor(targetPivot - 4, DARK);
        } else if (pivoting && pivotPixel < (2 * (scannerWidth - 1))) {
            // Pivoting.
            int thisPos = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel);
            int lastPos1 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 1);
            int lastPos2 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 2);
            int lastPos3 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 3);
            int lastPos4 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 4);
            
            strip.setPixelColor(thisPos, pixelColor);
            strip.setPixelColor(lastPos1, DimColor(strip.getPixelColor(thisPos)));
            strip.setPixelColor(lastPos2, DimColor(strip.getPixelColor(lastPos1)));
            strip.setPixelColor(lastPos3, DimColor(strip.getPixelColor(lastPos2)));
            strip.setPixelColor(lastPos4, DARK);

            pivotPixel++;
        } else if (pivoting && pivotPixel >= (2 * (scannerWidth - 1))) {
            pivotCount++;

            strip.setPixelColor(currentPixel, pixelColor);
            strip.setPixelColor(currentPixel - 1, DimColor(strip.getPixelColor(currentPixel)));
            strip.setPixelColor(currentPixel - 2, DimColor(strip.getPixelColor(currentPixel - 1)));
            strip.setPixelColor(currentPixel - 3, DimColor(strip.getPixelColor(currentPixel - 2)));
            strip.setPixelColor(currentPixel - 4, DARK);
            
            if (pivotCount > MAX_PIVOT_COUNT) {
                // Transitioning out of pivot and back into chase -- maybe.
                pivoting = false;
                currentPixel = targetPivot + 1;
                targetPivot = GetNextPivotPosition(targetPivot);
                pivotCount = 0;
            } else {
                // Pivot again!
                pivotCount++;
                pivotPixel = 0;
            }
        }
        
        MarkUpdated();
    }  
};

class BackwardScannerSprite : public Sprite {
private:
    uint16_t indexPixel = strip.numPixels() - 1;             // start pixel for animations
    uint16_t destPixel = strip.numPixels() - 10;             // destination pixel for animations

    uint32_t pixelColor;
    int16_t currentPixel;
    uint16_t targetPivot;
    uint16_t pivotCount;
    bool pivoting;
    uint16_t generatedPivotTotal = random(MIN_PIVOT_COUNT, MAX_PIVOT_COUNT);
    uint16_t scannerWidth = MIN_SCANNER_WIDTH + 2 * random(0, 1 + (MAX_SCANNER_WIDTH - MIN_SCANNER_WIDTH) / 2);

    // The number of the pixel for the purposes of the scanner. A PIVOT_WIDTH wide scanner
    // should cause this number to range from 1 to (PIVOT_WIDTH - 1) * 2.
    int pivotPixel;
    
    int GetNextPivotPosition(int currentPivot) {
        return currentPivot - random(MIN_LEAP_DISTANCE_BETWEEN_PIVOTS, MAX_LEAP_DISTANCE_BETWEEN_PIVOTS) - 3;
    }

    int getScannerPixelPosition(int _targetPivot, int _pivotWidth, int _pivotPixel) {
        // Examples:
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 1
        //         ..........===*T====........... returns 14
        
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 4
        //         ..........*===T====........... returns 11
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 5
        //         ..........=*==T====........... returns 12
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 11
        //         ..........====T==*=........... returns 18
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 12
        //         ..........====T===*........... returns 19
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 16
        //         ..........====*====........... returns 15
        //
        //                 farside  nearside
        
        int farSide = (_pivotWidth - 1) / 2; 
        int nearSide = farSide + _pivotWidth - 1;
    
        if (_pivotPixel < farSide) {
            return _targetPivot - _pivotPixel;
        } else if (_pivotPixel >= farSide && _pivotPixel < nearSide) {
            return _targetPivot - 2 * farSide + _pivotPixel;
        } else {
            return _targetPivot + (2 * (_pivotWidth - 1) - _pivotPixel); 
       }
    }

public:
    BackwardScannerSprite(uint32_t _color) {
        pixelColor = _color;
        currentPixel = strip.numPixels() - 1;
        targetPivot = GetNextPivotPosition(currentPixel);
        pivoting = false;
        pivotCount = 0;
        generatedPivotTotal = random(MIN_PIVOT_COUNT, MAX_PIVOT_COUNT);
        scannerWidth = MIN_SCANNER_WIDTH + 2 * random(0, 1 + (MAX_SCANNER_WIDTH - MIN_SCANNER_WIDTH) / 2);
        
        pivotPixel = 1;
    }
    
    ~BackwardScannerSprite() {
    }
    
    virtual void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }

        if (currentPixel < -5) {
            MarkDone();
            return;
        }

        if (! (pivoting) && currentPixel > targetPivot) {
            strip.setPixelColor(currentPixel, pixelColor);
            strip.setPixelColor(currentPixel + 1, DimColor(strip.getPixelColor(currentPixel)));
            strip.setPixelColor(currentPixel + 2, DimColor(strip.getPixelColor(currentPixel + 1)));
            strip.setPixelColor(currentPixel + 3, DimColor(strip.getPixelColor(currentPixel + 2)));
            strip.setPixelColor(currentPixel + 4, DARK);
            currentPixel--;
        } else if (! (pivoting) && currentPixel == targetPivot) {
            // Reached the next pivot!
            pivoting = true;
            pivotPixel = 1;
            strip.setPixelColor(targetPivot, pixelColor);
            strip.setPixelColor(targetPivot + 1, DimColor(strip.getPixelColor(targetPivot)));
            strip.setPixelColor(targetPivot + 2, DimColor(strip.getPixelColor(targetPivot + 1)));
            strip.setPixelColor(targetPivot + 3, DimColor(strip.getPixelColor(targetPivot + 2)));
            strip.setPixelColor(targetPivot + 4, DARK);
        } else if (pivoting && pivotPixel <= scannerWidth * 2 - 1) {
            // pivotPixel counts up, even though currentPixel will be counting down. Yes, I know that's a bit weird.

            // Pivoting.
            int thisPos = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel);
            int lastPos1 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 1);
            int lastPos2 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 2);
            int lastPos3 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 3);
            int lastPos4 = getScannerPixelPosition(targetPivot, scannerWidth, pivotPixel - 4);
            
            strip.setPixelColor(thisPos, pixelColor);
            strip.setPixelColor(lastPos1, DimColor(strip.getPixelColor(thisPos)));
            strip.setPixelColor(lastPos2, DimColor(strip.getPixelColor(lastPos1)));
            strip.setPixelColor(lastPos3, DimColor(strip.getPixelColor(lastPos2)));
            strip.setPixelColor(lastPos4, DARK);

            pivotPixel++;
        } else if (pivoting && pivotPixel > scannerWidth * 2 - 1) {
            pivotCount++;

            strip.setPixelColor(currentPixel, pixelColor);
            strip.setPixelColor(currentPixel + 1, DimColor(strip.getPixelColor(currentPixel)));
            strip.setPixelColor(currentPixel + 2, DimColor(strip.getPixelColor(currentPixel + 1)));
            strip.setPixelColor(currentPixel + 3, DimColor(strip.getPixelColor(currentPixel + 2)));
            strip.setPixelColor(currentPixel + 4, DARK);
            
            if (pivotCount > MAX_PIVOT_COUNT) {
                // Transitioning out of pivot and back into chase -- maybe.
                pivoting = false;
                currentPixel = targetPivot - 1;
                targetPivot = GetNextPivotPosition(targetPivot);
                pivotCount = 0;
            } else {
                // Pivot again!
                pivotCount++;
                pivotPixel = 1;
            }
        }
        
        MarkUpdated();
    }  
};

class GravitySprite : public Sprite {
private:
    int currentPixel;
    float velocity = 0.01;
    float acceleration = 0.2;
    uint32_t pixelColor;
    bool directionForward;

public:
    GravitySprite(int startPixel, uint32_t color) {
        this->pixelColor = color;
        this->currentPixel = startPixel;
    }
    
    void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }

        strip.setPixelColor(currentPixel, DARK);
        
        currentPixel += velocity;
        velocity += acceleration;
        
        if (currentPixel >= strip.numPixels()) {
            MarkDone(); 
        }

        strip.setPixelColor(currentPixel, pixelColor);        

        strip.show();

        MarkUpdated();
    }
    
    void SetNextUpdateTime() {

      nextUpdateTime = 33;    
    }
};

class BoringSprite : public Sprite {
private:
    int currentPixel;
    uint32_t pixelColor;
    bool directionForward;
    
public:
    BoringSprite(int start, uint32_t pc) {
        currentPixel = start;
        pixelColor = pc;
        directionForward = true;
    }
    
    ~BoringSprite() {
    }

    void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }

        strip.setPixelColor(currentPixel - (directionForward ? 1 : -1), DARK);
        strip.setPixelColor(currentPixel, pixelColor);

        if (currentPixel >= strip.numPixels()) {
            // We've run off the end of the strip. We're done here.
            directionForward = false;
            currentPixel = strip.numPixels() - 1;
        } else if (currentPixel < 0) {
            // We've run off the front of the strip.  
            directionForward = true;
            currentPixel = 0;
        } else {
            currentPixel += (directionForward ? 1 : -1); 
        }

        strip.show();
     
        MarkUpdated();
    }
    
    void SetNextUpdateTime() {
        nextUpdateTime = 10 * random(MAX_UPDATE_INTERVAL_IN_MS);  
    }
};

class TestPatternSprite : public Sprite {
private:
    int currentPixel;
    uint32_t pixelColor;
    bool directionForward;
    
public:
    TestPatternSprite() {
        currentPixel = 0;
        pixelColor = 0xffffff;  // White
    }
    
    TestPatternSprite(int startPixel, uint32_t color) {
        currentPixel = startPixel;
        pixelColor = color;
    }
    
    ~TestPatternSprite() {
    }
    
    void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }

        // test();
        // JOSH: EDIT HERE...

        strip.setPixelColor(currentPixel - 1, DARK);
        strip.setPixelColor(currentPixel, pixelColor);

        if (currentPixel >= strip.numPixels()) {
            // We've run off the end of the strip. We're done here.
            MarkDone();
        }
        
        // Update the current pixel for next time.        
        currentPixel++;
     
        // ...TO HERE.
        
        MarkUpdated();
    }
};

class SpriteManager {
  public:
    SpriteManager() {
    }
    
    ~SpriteManager() {
    }
    
    int SpriteCount() {
        int count = 0;
        
        return sprites.size();
    }

    void Update() {
        for (int i = 0; i < sprites.size(); i++) {
            sprites[i]->Update(); 
        }

        // Add the number of bytes left.
        // int q = freeRam();
        
        /*
        int q = sprites.size();

        for (int i = 0; i < q; i++) {
            strip.setPixelColor(i, 0x202020); 
        }
        for (int i = q; i < MAXSPRITES; i++) {
            strip.setPixelColor(i, DARK);
        }
        
        strip.show();*/
    }

    // Add it to the first free spot we see.
    bool Add(Sprite *newSprite) {
        if (sprites.size() >= MAXSPRITES) {
            delete newSprite;
            return false;
        } 
        
        sprites.push_back(newSprite);
        return true;
    }

    void Clean() {
        for (int i = 0; i < sprites.size(); i++) {
            if (sprites[i]->IsDone()) {
                sprites.erase(sprites.begin() + i);
                i--;
            } 
        }
    }

    private:
    vector<Sprite *> sprites;
};

SpriteManager* manager;
InfraredSensor infraredSensor1;
InfraredSensor infraredSensor2;
Pushbutton* pushbutton;

void setup() {
    randomSeed(analogRead(0));  

    // JOSH: EDIT HERE FOR BASIC SETUP AS BEFORE...
    
    pinMode(NEOPIXEL_DATA_PIN, OUTPUT);      // declare LED as output
    pinMode(PIR_SENSOR_1_PIN, INPUT);     // declare sensor as input
    pinMode(PIR_SENSOR_2_PIN, INPUT);
 
    strip.begin();

    darkenStrip();

    manager = new SpriteManager();
    
    TestPatternSprite *s1 = new TestPatternSprite(0, 0xff0000);
    manager->Add(s1);
    
    TestPatternSprite *s2 = new TestPatternSprite(5, 0x00ff00);
    manager->Add(s2);
    
    TestPatternSprite *s3 = new TestPatternSprite(10, 0x0000ff);
    manager->Add(s3);

    /* Initialize pir1, pir2 here. */ 
    pushbutton = new Pushbutton(PUSHBUTTON_PIN);

    // ...END HERE.
}

bool booted = false;
int32_t starttime = millis();
bool loadedScannerSprite = false;

void loop() {  
    if (! booted) {
        if (millis() - starttime > (MAXSPRITES + 20) * MAX_UPDATE_INTERVAL_IN_MS) {
            booted = true;
        }

        manager->Update();
    }

    // TODO Clean up any old sprites that no longer have anything to do.
    manager->Clean();

    // TODO Poll input devices, determine if we have to add a new sprite.
    /*
    if (infraredSensor1.IsActuated()) {
        manager->Add(new ForwardScannerSprite(random(0xffffff)));
    }

    if (infraredSensor2.IsActuated()) {
        manager->Add(new BackwardScannerSprite(random(0xffffff))); 
    }
    */

    if (pushbutton->IsActuated()) {
        manager->Add(new TestPatternSprite()); 
    }

    if (random(3000) == 0) {
        Sprite* nextSprite = new GravitySprite(random(4), random(0xffffff));
        
        if (! manager->Add(nextSprite)) {
            delete nextSprite;  
        }
    }

    // Update all existing sprites.
    manager->Update();
}

void darkenStrip() {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, DARK); 
    }
    
    strip.show();
}

void test(uint32_t c) {
    uint32_t randomColor = c;     
 
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, randomColor); 
    }
    
    strip.show();
}

void testRandom() {
    uint32_t randomColor = random(0xffffff);     
 
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, randomColor); 
    }
    
    strip.show();
}

void showNumberOfSprites(int count) {
   int i;
   
   for (i = 0; i < count; i++) {
       strip.setPixelColor(i, 0x404040); 
   }
   for (int j = i; j < MAXSPRITES; j++) {
       strip.setPixelColor(i, DARK); 
   }
   
   strip.show();
}

// Calculate 50% dimmed version of a color (used by ScannerUpdate)
uint32_t DimColor(uint32_t color) {
    // Shift R, G and B components one bit to the right
    uint32_t dimColor = strip.Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
    return dimColor;
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color) {
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color) {
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color) {
    return color & 0xFF;
}

