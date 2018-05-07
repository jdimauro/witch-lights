# Loopy Debug is my new Frank Zappa cover band

Currently working on looping animations for when the sprites pause.

Here's one in Excel

And animated

I would show you what it looks like on the NeoPixels, but the Arduino crashes when it tries to spawn the sprite object, so I've gear shifted from animation design mode into debug mode. 

## How I understand what the code does

So what's happening is, the Arduino bootloads, runs through all the globals at the top, stuff like:

	#define afc_l_pulsar_a_ANIMATION_FRAME_WIDTH    24
	#define afc_l_pulsar_a_ANIMATION_FRAMES         22
	
Which is where I set the parameters for each animation. In this case, you've got 22 frames of animation, each one 24 pixels wide. It also sets all the other parameters to tune the global travel acceleration rate, sprite pixel velocity, and how far each sprite travels before playing the idle animation. 

We then go on to use the FastLED library to create a `CRGB` struct to contain the color values we will write to the NeoPixel strip: 

	CRGB leds[NUM_LEDS];

`CRGB` structs are a range of memory that contain 3 bytes for each LED: the red, green, and blue color values. So the amount of SRAM that the struct takes up is basically `[NUM_LEDS] * 3` bytes. 

To animate the pixels that I've been designing in Excel, what happens is, we define a `char` and a `CRGB` struct for each animation, and each is `FRAME_WIDTH * FRAMES` long. In the case of the pulsar loop above, that's 24 by 22, which is 528. 

A `char` is one byte per entry, and we already know a `CRGB` is three, so that means we're taking up 528 times 4 bytes, or 2.06K. And we have a total of 96K of SRAM to execute the entire system, including all pre-rendered animations. 

Actually, we have 32K. See, the Arduino Due has a 32K bank of SRAM, and a 64K bank of SRAM, and I'm not sure how they interact, but I *can* say for certain that if you define too many `CRGB` structs and `char`s, the Arduino Due crashes on boot, and our best guess for memory usage is that we're around the 30-33K mark. I'll get back to that in a bit. 

So we define all the animations that we're going to run in this build, and then we define our function prototypes and our object classes. I won't get into the structure of the classes, except to say that there's a `SpriteVector` and `SpriteManager` class, which combine to handle the creation, lifecycle, and deconstruction of each animation sprite. 

There is also the `LoopTestSprite` or `FragmentTestSprite` classes, which are subclasses of the `Sprite` class. Each different sprite class currently has code to move from point to point, and to play a pre-rendered animation for a certain number of repetitions when it reaches its destination pixel. It then moves to the next destination pixel, loops, and repeat until the whole thing goes off the pixel grid, at which point the object is marked `is_done`. At which point, `SpriteManager` deletes the object from memory on its next run-through. 

So that's where the classes are defined. If the Arduino passes safely through those, we reach the point where we create our permanent objects: 

	InfraredSensor *sensor1;
	InfraredSensor *sensor2;

	SpriteManager *spriteManager;

	bool isBooted;
	bool testSpritesCreated;

	int starttime = millis();

So we've got 2 infrared sensors per strip of the Witch Lights, and they get created and assigned the names `sensor1` and `sensor2`. 

We boot up `SpriteManager`, prime some `bool`s, and set the `starttime` value, and then it's time to run `setup()`. 

	void setup() {
	    createColorsets();
	    createAnimationFrames();

	    isBooted = false;
	    testSpritesCreated = false;


	    spriteManager = new SpriteManager();

	    sensor1 = new InfraredSensor(PIR_SENSOR_1_PIN);
	    sensor2 = new InfraredSensor(PIR_SENSOR_2_PIN);

	    resetStrip();
	}

`setup()` reads our arrays of predefined color sets into RAM, and then runs the `createAnimationFrames()` function. Which reads all of the animations defined within the function into the `char`s we created earlier. So now, in memory, we have a full set of pixel animations, in the form of `char` structs with the animation's name.

Setup also sets `isBooted` and `testSpritesCreated` to false, which are bits that trigger a test pattern at the beginning of `loop()` later. And it links the infrared sensors to their appropriate input pins, resets the NeoPixel strip, and we're ready for the main `loop()`.

`loop()` is the main engine of an Arduino project. It cycles forever, and each time, you have a chance to do some logic. In this case, we run a rainbow flag test pattern down the length of the NeoPixel strips, which gives us a nice way to test the strips as the installation is assembled. 

After that (which only runs once), we check `sensor1`, check `sensor2`, create the appropriate sprite object if either is triggered. So when someone walks by the Witch Lights, an animation sprite is called into memory. But the sprite object only knows how to respond to `Update()` calls from `SpriteManager`: it won't do anything on its own. 

So `loop()` calls `spriteManager->Update();` last of all, and then the loop repeats. 

Lots of stuff happens behind the scenes when you call `Update()` of course. But we'll get into that as we need it. Right now, I'm finishing the talk-through of the whole boot cycle of the `witchlights-fastled.ino` sketch, because right now, the Arduino is crashing before it draws the new animation sprite I just defined. 

## When your shit crashes

A few days ago, when I was just learning to create custom animation sprites, I tried to define all the animations I had converted at once. I knew animations took up SRAM, but thinking that I had 96K to play with, I wasn't concerned about SRAM usage yet. 

The Arduino crashed so hard I had to manually erase its flash RAM before I could reprogram it again. For reference: that's pretty bad. 

In that case, it crashed while creating the `char` and `CRGB` structs, before it reached `setup()`. So the Arduino would turn on, but nothing else would happen. 

Today's problem is different. 

Today, when I turn on the Arduino, it plays the test pattern, which means it's made it all the way through everything we just defined, to the `loop()`, and at least started the main loop. 

But when I activate `sensor1` with a pushbutton, nothing happens. 

So something ain't right. 

For comparison, here is what I have defined in memory for animation: 

	char afc_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
	CRGB af_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

	char afc_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];
	CRGB af_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];

	char afc_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];
	CRGB af_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];

	char afc_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];
	CRGB af_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];

Looking at this, we have:

* `afc_w8v1r` is the original animation sprite from last year, defined backwards for sprites heading in the "reverse" direction from `sensor2`. 

* `afc_f_slow_stop` is the [Better slow stop go][afc_f_slow_stop] animation that I was testing a couple days ago. 

* `afc_f_slow_stop_c` is an experimental variation of that animation, using manual anti-aliasing for slow pixel-to-pixel movements.

* `afc_l_pulsar_a` is the first loop test, featured at the top of this post. 

[afc_f_slow_stop]: https://i2nk.co/notes/2018/5/2/progress-better-slow-stop-go

When I first uploaded this sketch, in the `sensor1` check in `loop()`, we had:

    if (sensor1->IsActuated()) {
        Sprite *s1 = new FragmentTestSprite();
        // Sprite *s1 = new LoopTestSprite();

        if (! spriteManager->Add(s1)) {
            delete s1;
        }
    }

And pressing the sensor button would spawn a "slow stop go" sprite. Great. 

So I changed it to: 

    if (sensor1->IsActuated()) {
        // Sprite *s1 = new FragmentTestSprite();
        Sprite *s1 = new LoopTestSprite();

        if (! spriteManager->Add(s1)) {
            delete s1;
        }
    }

And now, when you hit the sensor button, nothing happens. 

Huh. 

OK, so. 

Change it back? Sprite spawns. Change it again? Nothing happens. 

So it's *consistent*, whatever it is. That's a bonus. Consistent means repeatable. 

My first thought was, what did I do wrong in the `LoopTestSprite()` class definition that I didn't do in `FragmentTestSprite()`? So I did a diff between the two. 

![](diff.png)

And for the life of me, I can't find anything in the `LoopTestSprite()` that's different *and* is Arduino-crashingly bad. That's the differences above. On the left, the sprite that runs, and on the right, the one that don't. 

Is it SRAM usage? 

`afc_f_slow_stop`, which *does* run, is 4.32K in SRAM. 

`afc_l_pulsar_a`, which *does not* run, is 2.06K in SRAM. 

We can't rule that out, but it seems unlikely. 

## So what now, smart guy?

Welp. 

Fortunately, there's a `debug()` function, which lights up a specific LED when you call it. 

In the past, this was used to show the size of the SRAM heap while the Arduino ran. That helped greatly to identify memory leaks. 

What I can do with it now is, I can place `debug(20)` at the start of `loop()`, `debug(21)` at the next step, 22 at the next, and so on, and put debug statements into the sprite construction logic of the sprite that *works*, and do a control experiment to make sure I can identify all the working pieces of that sprite doing their thing. As the sprite is constructed and runs through the logic to process the animation `char` into `CRGB` frames, the most easily-seen LEDs under my desk will turn on, one by one. 

(And I'll be checking that into git for sanity's sake.)

Once that's done, I will change the animation logic values in that sprite to point at the animation for `LoopTestSprite()`. 

Why? Because I just tested the debug logic, and if I used copy and paste or hand-typed all the debug into `LoopTestSprite()`, I'm running the risk of a typo or mistake causing it to light up the wrong LEDs or malfunction in some other unknown way. 

So I will modify the sprite to run the new animation, compile, and run it on the Arduino. At which point... I will probably learn something unexpected. That's how it usually happens. 

But for now, rain clouds are moving in, and I'm going to sit on my screened-in back porch and watch the rain. Animation can wait. 