# Progress: Building the test rig  
### 3:33 PM — Sunday; April 29, 2018 ###

Here we are, about to upgrade the test rig on my workbench. 

The current build of [witchlights-fastled.ino][ino] does not run on the Arduino Mega. It gives memory errors. I don't want to get bogged down with memory optimization at this point in the game. So I'm replacing it with an Arduino Due.

[ino]: https://github.com/jdimauro/witch-lights/blob/master/arduino/witchlights-fastled/witchlights-fastled.ino

The Due I'm installing is a drop-in replacement module for the control board of the Witch Lights. That way, I'm testing on the exact hardware that will be in production. 

The LED strip is a five-meter WS2812B addressible LED strip, with 30 LEDs/meter. Just like the Adafruit NeoPixel strips I used in production, except without the waterproofing. Which won't matter in my home workspace, I sincerely hope.

***

And that was fast. 

The test rig is in place, and the WS2812B strip works fine. 

[Witch Lights test rig in place][rig]:

[rig]: https://vimeo.com/267138921

Next up is creating a new Sprite animation subclass, editing its `scanner` routine to map to the animation that I was [working on the other day][callback]. 

[callback]: https://i2nk.co/notes/2018/4/25/witch-lights-animation-progress-timing-and-framerates

Speaking of which, it looks like this exported into the arduino sketch format:

	//                       1234567890123456789012345678901234567890123456012345678901
	strcpy(afc_timing-test, "123456788                                                 ");
	strcat(afc_timing-test, "1234567888                                                ");
	strcat(afc_timing-test, "  1234567888                                              ");
	strcat(afc_timing-test, "     1234567888                                           ");
	strcat(afc_timing-test, "       123456677888                                       ");
	strcat(afc_timing-test, "        1223344556667788                                  ");
	strcat(afc_timing-test, "         111122233344556667788                            ");
	strcat(afc_timing-test, "              11111222233344556667788                     ");
	strcat(afc_timing-test, "                   11111222333444555666788                ");
	strcat(afc_timing-test, "                          1111112223344556788             ");
	strcat(afc_timing-test, "                                 11111223456788           ");
	strcat(afc_timing-test, "                                     11123456788          ");
	strcat(afc_timing-test, "                                        1123467831        ");
	strcat(afc_timing-test, "                                            12468531      ");
	strcat(afc_timing-test, "                                             11386421     ");
	strcat(afc_timing-test, "                                              1287543211  ");
	strcat(afc_timing-test, "                                               18865432211");
	strcat(afc_timing-test, "                                               886443211  ");
	strcat(afc_timing-test, "                                              88765431    ");
	strcat(afc_timing-test, "                                            4887654321    ");
	strcat(afc_timing-test, "                                            4886421       ");
	strcat(afc_timing-test, "                                            138831        ");
	strcat(afc_timing-test, "                                            138831        ");
	strcat(afc_timing-test, "                                            138831        ");
	strcat(afc_timing-test, "                                            138831        ");
	strcat(afc_timing-test, "                                            138831        ");
	
So that's some progress. Taking a break now. 