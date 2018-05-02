# Progress: Timing test

After some trial and error, I generated an `AnimationTestSprite` class, and re-defined all of the `char` and `CRGB` structs to load my generated pixel art animation into memory. And told the MotionSensor object to spawn one when it detects motion. (Or when I push the test button.)

And the Arduino Due crashed so hard, I had to force erase its memory. 

Welp. 

I'll get into what caused it later, but I hunted down the cause and fixed it. So I just watched my first pixel art animation on the live test rig. 

[Witch Lights Animation Test 1: Timing Test][video]:

[video]: https://vimeo.com/267517193

The frame rate is *much* faster than I thought. That's good. It can also be adjusted with an `UpdateInterval` variable, so that's also good. 

