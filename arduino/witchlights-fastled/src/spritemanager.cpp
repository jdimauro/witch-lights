#include "spritemanager.h"

void SpriteManager::Update() {
  updatedSomething = false;

  for (int i = 0; i < this->SpriteCount(); i++) {
    updatedSomething |= spriteVector->Get(i)->Update();
  }

  if (updatedSomething) {
    FastLED.show();
  }

  this->Clean();
}

bool SpriteManager::Add(Sprite *newSprite) {
  bool x = spriteVector->Add(newSprite);
  return x;
}

void SpriteManager::Clean() {
  for (int i = this->SpriteCount() - 1; i >= 0; i--) {
    if (spriteVector->Get(i)->IsDone()) {
      spriteVector->RemoveAt(i);
    }
  }
}
