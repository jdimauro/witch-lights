# Witch Lights Animation Progress: Timing And Framerates

I made the changes to the [Witch Lights][wl] animation [from last night][lastnote], and am happy with the progress so far. It's not done, but progress is being made. 

[wl]: https://witchlights.com/
[lastnote]: https://i2nk.co/notes/2018/4/24/rough-animation-tests-skid-stop

[Skid-stop animation third attempt 2018-04-25][ss3] on Vimeo

[ss3]: https://vimeo.com/266530739

This is a big improvement. Interestingly, on my other computer, the keyboard repeat rate is faster, which results in a higher framerate. That's making a big difference in the effect of the animation. 

In this video above, the sprite appears to accelerate a bit too quickly, and then when it stops short, the tail isn't long enough. 

[Skid-stop animation fourth attempt 2018-04-25][ss4] on Vimeo

[ss4]: https://vimeo.com/266531200

Above, I adjusted the frame rate down (by using the keyboard control panel), and now the speed of acceleration and braking seem ok. But, when the sprite stops short, the tail doesn't shrink from the left quickly enough. 

So: _framerate is a variable to control for_. 

I have to upload my code to the Witch Lights development board and watch the actual animation on the NeoPixels to see what kind of framerate I'm getting.

So, next actions:

* Connect development Arduino to my mac and upload the current code

* Connect a simple push-button up to the sensor pin on the Arduino to simulate the motion sensor

* Take the 5-meter cheapo NeoPixels I just got from AliExpress and connect them to my NeoPixel strip test rig, which will verify that the pixel strip is good

* Hook the (hopefully good) pixel strip to the development board, and power it on. If everything is good, I'll see a pride-flag test pattern

* Test the current known-good code with the push button, verify that it triggers sprite object creation and animation

* Create a new `Animation Test Sprite` class, copying the current Sprite object class, and tell the motion sensor objects to trigger generation of that class, not the default class

* Export the animation to CSV from Excel

* Run the [csv2cpp python script][csv2cpp] on that CSV file and copy the generated code

[csv2cpp]: https://github.com/jdimauro/witch-lights/blob/animation-experiments/python/csv2cpp.py

* Insert the generated code into the appropriate segment of the Witch Lights Arduino sketch, and compile/upload the code to make sure nothing breaks.

* Modify the `Animation Test Sprite` class so that in _Scanner_ mode, it looks at the array of strings tagged with the animation name. 

* Also change the global variables for animation frame width and animation frame count (create new globals for now, refactor this later)

* Compile and upload the code, which -- *in theory* -- should result in the Witch Lights animating my new sprite animation when I hit the go button. 

Simple. 

Later, I'll work out a master class for sprites with different animations, and make all the different animation sprites children of that, but for now it's gonna be messy. 

Because the point is to _see the framerate of the animation on the Arduino and NeoPixels_. 

And then I'll adjust the framerate on my computers (by messing with Keyboard preferences) to match as closely as possible. 

What a horrible hack this is. I'm absurdly proud. 