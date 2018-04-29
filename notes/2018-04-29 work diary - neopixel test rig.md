# Tool Kit: NeoPixel Reel Tester

One of the problems I anticipated facing while on-site with the Witch Lights is the failure of a NeoPixel strip. Because of the architecture of the lights, a bad strip will take out not just its five meter section of LEDs, but also all strips plugged in downstream. 

As one diagnostic tool in my on-site kit, I have this Arduino Uno, which is linked up to one of the waterproof 4-pin connectors that the Witch Lights pixel strips have on each end. To use it, you simply plug one end of a NeoPixel strip into it, and plug in the battery. 

It runs a color test pattern down the strip, which tells you that the strip works, and also that it doesn't have any bad pixels. So that's what I put it together for. 

Right now, though, it's coming out because I'm going to plug it into this new 5-meter pixel strip, which will be used for my workshop test rig. 