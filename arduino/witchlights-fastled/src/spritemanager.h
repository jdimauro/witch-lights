#ifndef SPRITEMANAGER_H_
#define SPRITEMANAGER_H_

#include <Arduino.h>

#include "FastLED.h"

#include "spritevector.h"

#define MAXSPRITES 20

class SpriteManager {
 private:
  bool updatedSomething = false;
  SpriteVector *spriteVector;

 public:
  SpriteManager() { spriteVector = new SpriteVector(MAXSPRITES); }

  ~SpriteManager() {
    // Don't bother. Should never be called.
  }

  int SpriteCount() { return spriteVector->Count(); }

  void Update();

  bool Add(Sprite *newSprite);

  // Garbage collection. Remove any sprites that have finished their animation
  // from the SpriteVector, in order to make room for others.
  void Clean();
};

#endif