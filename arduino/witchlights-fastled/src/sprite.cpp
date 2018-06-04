#include "sprite.h"

int LurkerSprite::Blink() {
  if (blinkDirection == 0) {
    if (eyeColor == eyeMaxColor) {
      // we are staring.
      // check to see if we need to blink
      if (millis() - lastBlinkTime >= blinkFrequency) {
        lastBlinkTime = millis();
        blinkFrequency = SetBlinkFrequency();  // reset blinkFrequency after every blink?
        return -1;
        // make this a random chance that gets more likely over time, based on
        // blinkFrequency?
      } else {
        return 0;
      }
    } else if (eyeColor == 0) {
      // we are mid-blink
      // check to see if our lifespan is up, and if so, mark done and exit
      if (blinkCount >= lifetimeBlinks) {
        this->MarkDone();
        SpawnBlinkChild();
        return 0;
      }
      // check to see if it's time to open our eyes
      if (millis() - lastBlinkTime >= blinkTiming) {
        // eyes have been closed long enough
        return 1;
      } else {
        // eyes are staying closed a little longer
        return 0;
      }
    }
  }

  if (blinkDirection == -1) {
    // have we reached the end of the blink?
    if (eyeColor <= 0) {
      // if eyeColor went negative, fix that
      eyeColor = 0;
      return 0;
    } else {
      // if not, keep blinking
      return -1;
    }
  }

  if (blinkDirection == 1) {
    if (eyeColor >= eyeMaxColor) {
      // we have just completed a single blink
      // add a blink to our lifetime count
      blinkCount++;
      // and record the time as the end of the last blink
      lastBlinkTime = millis();
      // and stop blinking
      return 0;
    } else {
      return 1;
    }
  } else {
    debug(7);
    return 0;  // if blinkDirection's value gets messed up somehow and is not -1,
               // 0, or 1, just set it to 0 for now.
  }
}

bool LurkerSprite::Update() {
  // debug(eyeColor);
  if (!this->UpdateNow()) {
    return false;
  }

  if (!this->allowCreation()) {
    this->MarkDone();
    return false;
  }

  // Decide if we're going to blink, and set the value to do it
  blinkDirection = Blink();

  // close or open the eyes a step by adding blinkDirection to eye color
  eyeColor += blinkDirection;

  this->eyes[0] = colorSets[colorPalette][eyeColor];         // colorSets[2][eyeColor];
  this->eyes[eyeWidth] = colorSets[colorPalette][eyeColor];  // colorSets[2][eyeColor];

  stripcpy(leds, eyes, currentPixel, eyeLength, eyeLength);
  return true;
}

bool LoopTestSprite::Update() {
  if (!this->UpdateNow()) {
    return false;
  }

  // debug(nextInflection);
  // Going from scanning to travel mode.
  if (isScanning && scanCount == scanCountTotal) {
    isScanning = false;
    currentPixel += 8;
    SetNextInflection();
    this->scanCount = 0;
    this->scanCountTotal = GetNewScanCountTotal();  // set to 1 for fragments
    this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
    leds[currentPixel - 6] = CRGB::Black;
    leds[currentPixel - 8] = CRGB::Black;
    leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of
                                           // the straggler when coming out of
                                           // scan mode.
    leds[currentPixel - 10] = CRGB::Black;
  }

  if (!isScanning) {
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
      // Safety. Since I don't trust my math, once we enter scanning mode,
      // ALWAYS go back to the constant speed for scanning
      // regardless of what the math said.
      updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
      isScanning = true;
      scanningFrame = 0;
      currentPixel -= 8;
    }

    if (currentPixel > NUM_LEDS) {
      this->MarkDone();
    }
  } else {
    stripcpy(leds, af_l_pulsar_a + afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * scanningFrame,
             currentPixel, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH,
             afc_l_pulsar_a_ANIMATION_FRAME_WIDTH);
    if (++scanningFrame == afc_l_pulsar_a_ANIMATION_FRAMES) {
      scanningFrame = 0;
      ++scanCount;
      // SetNextInflection();
    }
  }

  return true;
}

void FaerieSprite::StartTravel() {
  // Transition from loop mode to travel mode
  isIdling = false;
  currentPixel += 0;
  isWaiting ? SetNextWaitTravelTarget() : SetNextInflection();
  // debug(1);
  // delay(500);
  // if (nextInflection < 150) debug(nextInflection);
  // delay(1000);
  nextInflection = CoerceTargetPixel(nextInflection);
  // debug(2);
  // delay(500);
  // if (nextInflection < 150) debug(nextInflection);
  // delay(1000);
  totalTravelDistance = DistanceFromDestination();
  this->idleCount = 0;
  this->idleCountTotal = GetNewidleCountTotal();  // set to 1 for fragments

  isWaiting ? this->updateInterval = SetWaitInterval()
            : this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
  this->brakePercentage = SetBrakePercentage();

  // debug(updateInterval);
  // need to set a bool here so that updateTravel() knows to fade the values
  // towards 848?
  // idleToTravel
  this->pixelA = 6;
  this->pixelB = 8;
  this->pixelC = 6;
  UpdatePattern();
}

bool FaerieSprite::UpdateTravel() {
  // First, write current CRGB pattern to leds
  stripcpy(leds, pattern, currentPixel, patternLength, patternLength);

  // set dim factor
  dimFactor = SetDimFactor(updateInterval);

  // step backwards and fade the trail
  DimTrail(currentPixel, dimFactor, -1);
  DimTrail(currentPixel + 3, dimFactor, 1);

  // Next up, prepare for the next UpdateTravel() by moving currentPixel, and
  // setting the next updateInterval value

  currentPixel += TravelDirection();
  currentDistance = DistanceFromDestination();
  // debug(currentDistance);

  updateInterval -= AccelerateTravel();
  // debug(updateInterval);

  if (updateInterval < minInterval) {
    updateInterval = minInterval;
  }

  if (updateInterval > maxInterval) {
    updateInterval = maxInterval;
  }

  // If we have reached the destination pixel, our next stop is running the idle
  // animation
  if (currentDistance == 0) {
    StartIdle();
  }

  // TODO: test with 150 pixels
  // Terminate if we go off the end of the strip
  if (currentPixel > NUM_LEDS) {
    FadeOutTrail(NUM_LEDS - 1, 255, -1);
    // leds[NUM_LEDS -1] = CRGB::Black;
    this->MarkDone();
  }

  return true;
}

bool FaerieSprite::StartIdle() {
  // If we're in wait mode, we'll count one instance of running an idle
  // animation as one "wait" cycle, which is a series of small moves, in random
  // directions, and idle cycles.
  if (isWaiting) {
    waitCount++;
  } else {
    isWaiting = true;
  }

  dimFactor = 128;

  // Transition from travel mode to loop mode
  updateInterval = 1;
  isIdling = true;
  idlingFrame = 0;

  currentPixel -= 0;  // pretty sure we don't need this unless we're doing char animation

  fadeSteps = 0;

  return true;
}

bool FaerieSprite::UpdateIdle() {
  if (fadeSteps >= colorInertia) {
    pixelA = IdlePixelA(idlingFrame);
    pixelB = IdlePixelB(idlingFrame);
    pixelC = IdlePixelC(idlingFrame);

    updateInterval += AccelerateIdle(idlingFrame);

    fadeSteps = 0;
    idlingFrame++;
  } else {
    fadeSteps++;
  }

  UpdatePattern();
  // UpdatePatternWithSparkles();

  stripcpy(leds, pattern, currentPixel, patternLength, patternLength);

  FadeOutTrail(currentPixel, dimFactor, -1);
  FadeOutTrail(currentPixel + 3, dimFactor, 1);

  if (EffectiveFrame(idlingFrame) == 0) {
    ++idleCount;
  }

  return true;
}

bool FragmentTestSprite::Update() {
  if (!this->UpdateNow()) {
    return false;
  }

  // debug(2);
  // Going from scanning to travel mode.
  if (isScanning && scanCount == scanCountTotal) {  // >= ?
    isScanning = false;
    currentPixel += afc_l_pulsar_a_ANIMATION_FRAME_WIDTH;  // 8
    currentPixel -= 8;
    SetNextInflection();
    this->scanCount = 0;
    this->scanCountTotal = GetNewScanCountTotal();
    this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
    leds[currentPixel - 6] = CRGB::Black;
    leds[currentPixel - 8] = CRGB::Black;
    leds[currentPixel - 9] = CRGB::Black;  // I hate this. One-off to get rid of
                                           // the straggler when coming out of
                                           // scan mode.
    leds[currentPixel - 10] = CRGB::Black;
    // debug(3);
  }

  if (!isScanning) {
    // Traveling and continuing to travel.
    // debug(4);
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
      // debug(5);
      // Safety. Since I don't trust my math, once we enter scanning mode,
      // ALWAYS go back to the constant speed for scanning
      // regardless of what the math said.
      updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
      isScanning = true;
      scanningFrame = 0;
      currentPixel += 1;  // -8 normally
    }

    if (currentPixel > NUM_LEDS) {
      this->MarkDone();
    }
  } else {
    // debug(6);
    stripcpy(leds, af_l_pulsar_a + afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * scanningFrame,
             currentPixel, afc_l_pulsar_a_ANIMATION_FRAME_WIDTH,
             afc_l_pulsar_a_ANIMATION_FRAME_WIDTH);
    if (++scanningFrame == afc_l_pulsar_a_ANIMATION_FRAMES) {
      scanningFrame = 0;
      ++scanCount;
      // SetNextInflection();
    }
  }

  return true;
}

bool W8V1ScannerDebrisV1ReverseSprite::Update() {
  if (!this->UpdateNow()) {
    return false;
  }

  // Going from scanning to travel mode.
  if (isScanning && scanCount == scanCountTotal) {
    isScanning = false;
    scanCount = 0;
    currentPixel += 2;
    SetNextInflection();
    this->scanCount = 0;
    this->scanCountTotal = GetNewScanCountTotal();
    this->updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
    leds[currentPixel + 6] = CRGB::Black;
    leds[currentPixel + 8] = CRGB::Black;
    leds[currentPixel + 9] = CRGB::Black;  // I hate this. One-off to get rid of
                                           // the straggler when coming out of
                                           // scan mode.
    leds[currentPixel + 10] = CRGB::Black;
  }

  if (!isScanning) {
    // Traveling and continuing to travel.
    stripcpy(leds, pattern, currentPixel, patternLength, patternLength);
    currentPixel -= velocity;

    // Are we nearer the last inflection than the next inflection? If so, speed
    // up. Otherwise, slow down.
    if (currentPixel <= nextInflection - (SCANNER_DELAY_INTERVAL_IN_MS - 1)) {
      updateInterval += 1;
    } else {
      updateInterval -= 1;
    }

    if (updateInterval < 1) {
      updateInterval = 1;
    } else if (updateInterval > SPRITE_STARTING_DELAY_INTERVAL_IN_MS) {
      updateInterval = SPRITE_STARTING_DELAY_INTERVAL_IN_MS;
    }

    if (currentPixel <= nextInflection) {
      // Safety. Since I don't trust my math, once we enter scanning mode,
      // ALWAYS go back to the constant speed for scanning
      // regardless of what the math said.
      updateInterval = SCANNER_DELAY_INTERVAL_IN_MS;
      isScanning = true;
      scanningFrame = 0;
      currentPixel -= 3;
    }

    if (currentPixel <= -10) {
      this->MarkDone();
    }
  } else {
    stripcpy(leds, af_w8v1r + ANIMATION_FRAME_WIDTH * scanningFrame, currentPixel,
             ANIMATION_FRAME_WIDTH, ANIMATION_FRAME_WIDTH);
    if (++scanningFrame == ANIMATION_FRAMES) {
      scanningFrame = 0;
      ++scanCount;
      // SetNextInflection();
    }
  }

  return true;
}
