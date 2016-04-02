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
    
    boolean Update() {
        if (IsDone() || (! UpdateNow())) {
            return false;
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

        return true;
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

    boolean Update() {
        if (IsDone() || (! UpdateNow())) {
            return false; 
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

        return true;
    }
    
    void SetNextUpdateTime() {
        nextUpdateTime = 10 * random(MAX_UPDATE_INTERVAL_IN_MS);  
    }
};


