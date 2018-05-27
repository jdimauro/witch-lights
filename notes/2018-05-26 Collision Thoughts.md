# Collisions

It may be desired to have sprites interact with each other when they come near each other. Most generally, these can be called "collisions" in the physics sense. The sprites needn't touch, or even be that close to each other.



## Definitions
A "collision" in this sense is the change in a sprite's default behavior due to the proximity of another sprite.

A collision occurs when a sprite's "trigger condition" is met. When this condition is true, the sprite will become "triggered" and begin its "collision behavior".

Collision behaviors may be a change in the sprite's velocity, a change in its physical properties (like color o shape), a change in its behavior (scanning or lurking), or even its death (setting `isDone` to true).

Most of what we think of as real-world collisions are bidirectional. Each object experiences the collision in its own way. However, this doesn't have to be the case. A bug minding its own business, when hit by a truck windshield, is effectively a unidirectional collision. While momentum is conserved, the momentum of the bug doesn't significantly change that of the truck, so this is effectively a one-sided collision.

It's the intention that each sprite, when triggering, is responsible for determining its own behavior after the collision. Sprites can only identify when they've been triggered, and act accordingly. This also matches the general expectation in object-oriented development that classes' behaviors be independent.



## Sample Code

This is literally just some basic thoughts on the matter and I haven't devoted more than two brain cycles to any of this. C#-ish pseudocode follows.

    SpriteManager::Update() {
        foreach (Sprite sprite in spriteVector) {
            sprite->Update();

            // Performance warning! This is effectively O(n^2), since each time we update we 
            // have to cross-check each other existing sprite. Worse, the other sprite may
            // have moved and caused us to trigger, so we can't even skip this if Update() 
            // returns false and tells us we didn't do anything.

            foreach (Sprite otherSprite in spriteVector except the current one) {
                otherSprite->HasCollided(sprite);
            }
        }
    }


    Sprite::HasCollided(Sprite *otherSprite) {
        // e.g., the positions of the two sprites are within a given distance and the sprite
        // is a scanner sprite. That is, this sprite is visible to scanners but invisible to 
        // other types.

        if (otherSprite->currentPixel - this->currentPixel < COLLISION_DISTANCE &&
            otherSprite instanceof ScannerSprite) {
            this->Collide();
        } 
    }


    Sprite::Collide() {
        // I've collided. What do I do?

        this->color = RED;  // Change my color.
        // or
        this->velocity = -this->velocity;   // Start going in the opposite direction.
        // or
        this->isScanning = ! this->isScanning;
    }

