# Going Both Ways

Firefly is a week from Tuesday, and I need to finish one critical piece of functionality: I want my FaerieSprites to travel in both directions. Or, as the code comment says, `TODO: Faeries go both ways`. 

Here's what I'm having some trouble with: when sprites go off the "edge" of the addressible LED area that corresponds to their travelDirection, we mark the sprite "done" and it gets deleted from RAM. The logic in question is in the `UpdateTravel()` method; after we move CurrentPixel we check to see if we've reached the end of the road:

	// Terminate if we go off the end of the strip
	// TODO - make this work with whatever pixel is specified as the terminal pixel, with the comparison working with either direction?
	if (currentPixel > NUM_LEDS) {
		FadeOutTrail(NUM_LEDS - 1, 255, -1); // TODO - make this work on each end of the LED strip
		this->MarkDone();
	}

So the way I started to address this is, I edited the constructor for `FaerieSprite` and made it accept 3 parameters:

	FaerieSprite(int travelDirection, int startPixel, int endPixel) : Sprite() {

My thinking was, specify the general direction of travel, the pixel you're starting at, and the pixel you want to reach and terminate the sprite at. 

On reflection, though, to start with, I think we can get rid of `endPixel` as a parameter. 

Generally `endPixel` is going to be `NUM_LEDS` if `travelDirection` is 1, and it's going to be 0 if `travelDirection` is -1. And generally you want to check if `currentPixel > NUM_LEDS` if `travelDirection` is 1, and if `travelDirection` is -1, you want to check if `currentPixel < 0` instead. So that's a simple if / else statement. Unless there's a better, mathy way to do it. It's 4am and I haven't really been sleeping lately, so I'm not going to bang my head on it right now. Feel free to tell me a better way. 

_OK_, I think, _but what if I just check to see if `currentPixel < 0 || currentPixel > NUM_LEDS`?_ That way, if the sprite goes off either end of the "screen" (the length of the LED strip), we terminate it. That's nice and elegant, *but* there's a complication: when `FaerieSprite` spawns, its `startPixel` value is actually -3. That lets it "enter" from "offscreen", as it were, which is far more natural-looking a way of appearing than just flashing into existence in place. So if I implemented that comparison as the terminal logic, I'd need a way of preventing `FaerieSprite` from being terminated the instant it spawns. 

One idea that comes to mind is, add an `int updateCount` to the class, and put `updateCount++` in the `Update()` method. And only run the `currentPixel` comparison for `MarkDone()` if `updateCount > 10`. Or `25`, or something similar. The point is, give each sprite a grace period that lets it move into the "safe" zone of between 0 and `NUM_LEDS`. 

I'm pretty sure either would work. 