# Progress: Building the test rig  
### 3:33 PM — Sunday; April 29, 2018 ###

Here we are, about to upgrade the test rig on my workbench. 

The current build of [witchlights-fastled.ino][ino] does not run on the Arduino Mega. It gives memory errors. I don't want to get bogged down with memory optimization at this point in the game. So I'm replacing it with an Arduino Due.

[ino]: https://github.com/jdimauro/witch-lights/blob/master/arduino/witchlights-fastled/witchlights-fastled.ino

The Due I'm installing is a drop-in replacement module for the control board of the Witch Lights. That way, I'm testing on the exact hardware that will be in production. 

The LED strip is a five-meter WS2812B addressible LED strip, with 30 LEDs/meter. Just like the Adafruit NeoPixel strips I used in production, except without the waterproofing. Which won't matter in my home workspace, I sincerely hope.

