#ifndef SPRITEVECTOR_H_
#define SPRITEVECTOR_H_

#include "sprite.h"

class SpriteVector {
 private:
  Sprite **sprites;
  int maxCapacity;
  int count;

 public:
  SpriteVector(int maxCap) {
    this->count = 0;
    this->maxCapacity = maxCap;
    sprites = new Sprite *[maxCap];

    for (int i = 0; i < maxCap; i++) {
      sprites[i] = NULL;
    }
  }

  ~SpriteVector() {}

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

  int Count() { return this->count; }

  boolean RemoveAt(int i) {
    Sprite *ptr = sprites[i];
    sprites[i] = NULL;
    delete ptr;

    for (int j = i + 1; j < count; j++) {
      sprites[j - 1] = sprites[j];
    }
    sprites[count - 1] = NULL;

    --this->count;

    return true;
  }
};

#endif