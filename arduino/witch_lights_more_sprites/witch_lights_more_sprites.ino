#include <Adafruit_NeoPixel.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

// JOSH: FEEL FREE TO EDIT CONSTANT VALUES HERE...

// Hardware configuration.
#define N_LEDS              750               // 30/meter test reel    

#define LED_PIN              13
#define NEOPIXEL_DATA_PIN    6                // Pin for neopixels
#define PIR_SENSOR_1_PIN     3
#define PIR_SENSOR_2_PIN     4
#define PUSHBUTTON_PIN       2

// Sensor timeouts.
#define INFRARED_SENSOR_TIMEOUT_IN_MS  2000
#define PUSHBUTTON_TIMEOUT_IN_MS  1000

// The maximum number of sprites run simultaneously.
#define MAXSPRITES           3

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
void writePixel(int, uint32_t);


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

    virtual uint32_t GetNextUpdateTime() {
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

class ScannerSprite : public Sprite {
protected:
    uint32_t pixelColor;
    int currentPixel;
    int targetPivot;
    int pivotCount;
    bool pivoting;
    int generatedPivotTotal = MIN_PIVOT_COUNT;
    int scannerWidth;

    // The number of the pixel for the purposes of the scanner. A PIVOT_WIDTH wide scanner
    // should cause this number to range from 0 to (PIVOT_WIDTH - 1) * 2 - 1.
    int pivotPixel;
    int pivotPixelMax;

    int getScannerPixelPositionRelative(int _pivotWidth, int _pivotPixel) {
        // Here, _pivotPixel is the "frame" of the animation, starting at 0.
        // A scanner width of 9 would allow the result of this function to vary
        // from +4 to -4 (nine possible values). There would be sixteen possible
        // steps in this animation before returning to the beginning of the 
        // animation.
        // 
        // Or, in general, a scanner width of n (must be an odd number) would allow
        // the result of this function to vary from -((n - 1) / 2) to +((n - 1) / 2), 
        // that is to say, n possible values. There would be 2*(n - 1) possible steps
        // in this animation before returning to the beginning. 
      
        // Examples:
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 1
        //         ..........====T*===........... returns 1
        
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 4
        //         ..........====T===*........... returns 4
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 5
        //         ..........====T==*=........... returns 3
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 11
        //         ..........=*==T====........... returns -3
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 12
        //         ..........*===T====........... returns -4
    
        //    targetPivot (T) = 15, pivotWidth (=) = 9, pivotPixel (*) = 16
        //         ..........====*====........... returns 0
        //
        //      -------> direction of sprite motion ------->

        int p = _pivotPixel % (2 * (_pivotWidth - 1));
        int maxDeviationFromCenter = (_pivotWidth - 1) / 2;
        
        if (p <= maxDeviationFromCenter) {
            return p;
        } else if (p > maxDeviationFromCenter && p <= 3 * maxDeviationFromCenter) {
            return (_pivotWidth - 1) - p;
        } else {
            return -(2 * (_pivotWidth - 1)) + p; 
       }
    }
};

class ForwardScannerSprite : public ScannerSprite {
private:
    int GetNextPivotPosition(int currentPivot) {
        return currentPivot + random(MIN_LEAP_DISTANCE_BETWEEN_PIVOTS, MAX_LEAP_DISTANCE_BETWEEN_PIVOTS) + 3;
    }
    
public:
    ForwardScannerSprite(uint32_t _color) {
        pixelColor = _color;
        currentPixel = 0;
        targetPivot = GetNextPivotPosition(currentPixel);
        pivoting = false;
        pivotCount = 0;
        generatedPivotTotal = random(MIN_PIVOT_COUNT, MAX_PIVOT_COUNT);
        scannerWidth = 2 * random((MIN_SCANNER_WIDTH - 1) / 2, (MAX_SCANNER_WIDTH - 1) / 2) + 1;
        
        pivotPixel = 0;
        pivotPixelMax = 2 * (scannerWidth - 1) * generatedPivotTotal;
    }
    
    ~ForwardScannerSprite() {
    }

    virtual uint32_t GetNextUpdateTime() {
        return random(MIN_UPDATE_INTERVAL_IN_MS, MAX_UPDATE_INTERVAL_IN_MS);
    }
    
    virtual void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }

        if (! pivoting) {
            if (currentPixel < targetPivot) {
                // Moving along as normal. TODO: Add a velocity here?
                writePixel(currentPixel, pixelColor);
                writePixel(currentPixel - 1, DARK);
                
                ++currentPixel;
                // TODO: Add trails.

                if (currentPixel > strip.numPixels()) {
                    // We've run off the end. We're done here.
                    MarkDone();
                }                
            } else if (currentPixel == targetPivot) {
                // Peanut butter pivot with a baseball bat!
               pivoting = true;
               pivotPixel = 0; 
            } 
        }

        if (pivoting) {
            int thisPixel = getScannerPixelPositionRelative(scannerWidth, pivotPixel);
            int lastPixel = getScannerPixelPositionRelative(scannerWidth, pivotPixel - 1);
            
            writePixel(targetPivot + thisPixel, pixelColor);
            writePixel(targetPivot + lastPixel, DARK);
            ++pivotPixel;
            
            if (pivotPixel >= 2 * (scannerWidth - 1)) {
                // Go again! Or, if we must, transition out of pivot and back into chase.
                if (pivotCount < pivotPixelMax) {
                    ++pivotCount;
                } else {
                    // We're done here. Go back to chase mode.
                    pivoting = false;
                    currentPixel = targetPivot;
                    targetPivot = GetNextPivotPosition(targetPivot);
                    pivotCount = 0;
                }
            }
        }

        strip.show();

        MarkUpdated();
    }  
};

class BackwardScannerSprite : public ScannerSprite {
private:
    int GetNextPivotPosition(int currentPivot) {
        return currentPivot - random(MIN_LEAP_DISTANCE_BETWEEN_PIVOTS, MAX_LEAP_DISTANCE_BETWEEN_PIVOTS) + 3;
    }
    
public:
    BackwardScannerSprite(uint32_t _color) {
        pixelColor = _color;
        currentPixel = strip.numPixels() - 1;
        // targetPivot = GetNextPivotPosition(currentPixel);
        targetPivot = 20;
        pivoting = false;
        pivotCount = 0;
        generatedPivotTotal = random(MIN_PIVOT_COUNT, MAX_PIVOT_COUNT);
        // scannerWidth = 2 * random((MIN_SCANNER_WIDTH - 1) / 2, (MAX_SCANNER_WIDTH - 1) / 2) + 1;
        scannerWidth = 9;
        
        pivotPixel = 0;
        pivotPixelMax = 2 * (scannerWidth - 1) * generatedPivotTotal;
    }
    
    ~BackwardScannerSprite() {
    }

    virtual uint32_t GetNextUpdateTime() {
        return random(MIN_UPDATE_INTERVAL_IN_MS, MAX_UPDATE_INTERVAL_IN_MS);
    }
    
    virtual void Update() {
        if (IsDone() || (! UpdateNow())) {
            return; 
        }
 
        if (! pivoting) {
            if (currentPixel > targetPivot) {
                // Moving along as normal. TODO: Add a velocity here?
                writePixel(currentPixel, pixelColor);
//                writePixel(currentPixel + 1, DimColor(pixelColor));
//              writePixel(currentPixel + 2, DimColor(DimColor(pixelColor)));
//              writePixel(currentPixel + 3, DimColor(DimColor(DimColor(pixelColor))));
//              writePixel(currentPixel + 4, DARK);
                writePixel(currentPixel + 1, DARK);
                
                --currentPixel;
                // TODO: Add trails.

                if (currentPixel < -4) {
                    // We've run off the end. We're done here.
                    MarkDone();
                }                
            } else if (currentPixel == targetPivot) {
                // Peanut butter pivot with a baseball bat!
               pivoting = true;
               pivotPixel = 0; 
            } 
        }

        if (pivoting) {
            int thisPixel = getScannerPixelPositionRelative(scannerWidth, pivotPixel);
            int lastPixel = getScannerPixelPositionRelative(scannerWidth, pivotPixel - 1);
            int lastPixel2 = getScannerPixelPositionRelative(scannerWidth, pivotPixel - 2);
            int lastPixel3 = getScannerPixelPositionRelative(scannerWidth, pivotPixel - 3);
            int lastPixel4 = getScannerPixelPositionRelative(scannerWidth, pivotPixel - 3);

            writePixel(targetPivot - thisPixel, pixelColor);
/*            writePixel(targetPivot - lastPixel, DimColor(pixelColor));
            writePixel(targetPivot - lastPixel2, DimColor(DimColor(pixelColor)));
            writePixel(targetPivot - lastPixel3, DimColor(DimColor(DimColor(pixelColor))));
            writePixel(targetPivot - lastPixel4, DARK); */
            writePixel(targetPivot - lastPixel, DARK);
            
            ++pivotPixel;
            
            if (pivotPixel >= 2 * (scannerWidth - 1)) {
                // Go again! Or, if we must, transition out of pivot and back into chase.
                if (pivotCount < pivotPixelMax) {
                    ++pivotCount;
                } else {
                    // We're done here. Go back to chase mode.
                    pivoting = false;
                    currentPixel = targetPivot;
                    targetPivot = GetNextPivotPosition(targetPivot);
                    pivotCount = 0;
                }
            }
        }

        strip.show();

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

        writePixel(currentPixel, DARK);
        
        currentPixel += velocity;
        velocity += acceleration;
        
        if (currentPixel >= strip.numPixels()) {
            MarkDone(); 
        }

        writePixel(currentPixel, pixelColor);        

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

        writePixel(currentPixel - (directionForward ? 1 : -1), DARK);
        writePixel(currentPixel, pixelColor);

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

        writePixel(currentPixel - 1, DARK);
        writePixel(currentPixel, pixelColor);

        if (currentPixel >= strip.numPixels()) {
            // We've run off the end of the strip. We're done here.
            MarkDone();
        }
        
        // Update the current pixel for next time.        
        currentPixel++;
     
        // ...TO HERE.

        strip.show();
        
        MarkUpdated();
    }
};

class SpriteManager {
  public:
    SpriteManager() {
        spritesArray = new Sprite*[MAXSPRITES];
    }
    
    ~SpriteManager() {
        delete[] spritesArray;
    }
    
    int SpriteCount() {
        return 1;
      
        int count = 0;
        
        for (int i = 0; i < MAXSPRITES; i++) {
           if (spritesArray[i] != NULL) {
               ++count; 
           } 
        }
        
        return count;
    }

    void Update() {
        for (int i = 0; i < this->SpriteCount(); i++) {
            spritesArray[i]->Update(); 
        }

        int q = this->SpriteCount();

        for (int i = 0; i < q; i++) {
            writePixel(i, 0xffffff); 
        }
        
        strip.show();
    }

    // Add it to the first free spot we see.
    bool Add(Sprite *newSprite) {
        if (this->SpriteCount() >= MAXSPRITES) {
            delete newSprite;
            return false;
        } 
        
        for (int i = 0; i < MAXSPRITES; i++) {
            if (spritesArray[i] != NULL) {
                spritesArray[i] = newSprite;
                return true;
            }
        }
    }

    void Clean() {
        for (int i = 0; i < this->SpriteCount(); i++) {
            if (spritesArray[i]->IsDone()) {
                Sprite *ptr = spritesArray[i];
                spritesArray[i] = NULL;

                delete ptr;    
            } 
        }
    }

    private:
    // vector<Sprite *> sprites;
    Sprite** spritesArray;
};

SpriteManager* manager;
InfraredSensor infraredSensor1;
InfraredSensor infraredSensor2;
Pushbutton* pushbutton;

void setup() {
    // JOSH: EDIT HERE FOR BASIC SETUP AS BEFORE...
    
    pinMode(LED_PIN, OUTPUT);      // declare LED as output
    pinMode(PIR_SENSOR_1_PIN, INPUT);     // declare sensor as input
    pinMode(PIR_SENSOR_2_PIN, INPUT);
    
    digitalWrite(LED_PIN, LOW);
    randomSeed(analogRead(0));  

    strip.begin();
    strip.show();

    darkenStrip();

    manager = new SpriteManager();

    TestPatternSprite *s1 = new TestPatternSprite(0, 0xff0000);
    manager->Add(s1);

    /*
    TestPatternSprite *s2 = new TestPatternSprite(5, 0x00ff00);
    manager->Add(s2);
    
    TestPatternSprite *s3 = new TestPatternSprite(10, 0x0000ff);
    manager->Add(s3);

/*
    manager->dd(new    pinMode(NEOPIXEL_DATA_PIN, OUTPUT);      // declare LED as output
 BackwardScannerSprite(random(0xffffff)));
*(/
*
 */
    /* Initialize pir1, pir2 here. */ 
//    pushbutton = new Pushbutton(PUSHBUTTON_PIN);

    // ...END HERE.
}

bool booted = false;
int32_t starttime = millis();
bool loadedScannerSprite = false;

int i = 0;
void loop3() {
    setPixelColor(i, 0xffffff);
    setPixelColor((i - 1) % 750, 0x000000);
    strip.show();
    delay(50);
    i = ++i % 750;  
}

void setPixelColor(uint16_t loc, uint32_t color)
{
  if (loc > 0 && loc < N_LEDS) {
    strip.setPixelColor(loc, color);
  }
}

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

    if (pushbutton->IsActuated()) {
        manager->Add(new TestPatternSprite()); 
    }

    if (random(5000) == 0) {
        Sprite* nextSprite = new ForwardScannerSprite(random(0xff0000f));
        //Sprite *nextSprite = new BackwardScannerSprite(random(0xffffff));
        
        if (! manager->Add(nextSprite)) {
            delete nextSprite;  
        }
    }
*/
    
/*    if (random(5000) == 0) {
        Sprite* nextSprite = new BackwardScannerSprite(random(0x0000ff));
         
        if (! manager->Add(nextSprite)) {
            delete nextSprite;
        }  
    }
    */

    // Update all existing sprites.
    manager->Update();
}

void darkenStrip() {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        writePixel(i, DARK); 
    }
    
    strip.show();
}

void test(uint32_t c) {
    uint32_t randomColor = c;     
 
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        writePixel(i, randomColor); 
    }
    
    strip.show();
}

void testRandom() {
    uint32_t randomColor = random(0xffffff);     
 
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        writePixel(i, randomColor); 
    }
    
    strip.show();
}

void showNumberOfSprites(int count) {
   int i;
   
   for (i = 0; i < count; i++) {
       writePixel(i, 0x404040); 
   }
   for (int j = i; j < MAXSPRITES; j++) {
       writePixel(i, DARK); 
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

void writePixel(int pixelNumber, uint32_t color) {
    if (pixelNumber >= 0 && pixelNumber < strip.numPixels()) {
        strip.setPixelColor(pixelNumber, color);
    } 
}
