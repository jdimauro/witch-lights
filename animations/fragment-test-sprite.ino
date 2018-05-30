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
        nextInflection += random(SCANNER_MIN_STOP_DISTANCE, SCANNER_MAX_STOP_DISTANCE + 1);
    }

    int GetNewScanCountTotal() {
        return random(SCANNER_MIN_SCANS, SCANNER_MAX_SCANS + 1);
    }

  public:
    FragmentTestSprite() : Sprite() {
        // Initial state.
        this->currentPixel = -8;  // The first pixel of the pattern is black.
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
        for (int i = 0; i < afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES; i++) {
            af_f_slow_stop[i] = afc_f_slow_stop[i] > ' ' ? colorSets[colorPalette][afc_f_slow_stop[i] - '0'] : CRGB::Black;
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

    bool Update() {
        if (! this->UpdateNow()) {
            return false;
        }

        // Going from scanning to travel mode.
        if (isScanning && scanCount == scanCountTotal) {
            isScanning = false;
            currentPixel += afc_f_slow_stop_ANIMATION_FRAME_WIDTH; // 8
            currentPixel -= 8;
            SetNextInflection();
            this->scanCount = 0;
            this->scanCountTotal = 1;
            this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            leds[currentPixel - 6] = CRGB::Black;
            leds[currentPixel - 8] = CRGB::Black;
            leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of the straggler when coming out of scan mode.
            leds[currentPixel - 10] = CRGB::Black;
        }

        if (! isScanning) {
            // Traveling and continuing to travel.
            stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
            ++currentPixel;

            if (currentPixel >= nextInflection - (SCANNER_DELAY_INTERVAL_IN_MS - 1)) {
                updateInterval += 1;
            } else {
                updateInterval -= 1;
            }

            if (updateInterval < 1) {
                updateInterval = 1;
            } else if (updateInterval > SPRITE_STARTING_DELAY_INTERVAL_IN_MS) {
                updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
            }

            // Transition from travel mode to scanning.
            if (currentPixel >= nextInflection) {
                // Safety. Since I don't trust my math, once we enter scanning mode, ALWAYS go back to the constant speed for scanning
                // regardless of what the math said.
                updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
                isScanning = true;
                scanningFrame = 0;
                currentPixel += 1; // -8 normally
            }

            if (currentPixel > NUM_LEDS) {
               this->MarkDone();
            }
        } else {
            stripcpy(leds, af_f_slow_stop + afc_f_slow_stop_ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel, afc_f_slow_stop_ANIMATION_FRAME_WIDTH, afc_f_slow_stop_ANIMATION_FRAME_WIDTH);
            if (++scanningFrame == afc_f_slow_stop_ANIMATION_FRAMES) {
                scanningFrame = 0;
                ++scanCount;
                SetNextInflection();
            }
        }

        return true;
    }
};

